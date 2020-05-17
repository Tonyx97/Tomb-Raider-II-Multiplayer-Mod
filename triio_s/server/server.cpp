#include "server.h"

#include <fstream>
#include <stdio.h>
#include <string>
#include <iostream>
#include <regex>

#include <shared/game/math.h>
#include <shared/net/game_net_structs.h>
#include <shared/console/console.h>

Server::Server()
{
	peer = SLNet::RakPeerInterface::GetInstance();
}

Server::~Server()
{
	if (!peer)
		return;

	peer->Shutdown(100);

	SLNet::RakPeerInterface::DestroyInstance(peer);
}

bool Server::init()
{
	if (!peer)
		return false;

	peer->SetIncomingPassword("pass", sizeof("pass"));
	peer->SetTimeoutTime(10000, SLNet::UNASSIGNED_SYSTEM_ADDRESS);

	SLNet::SocketDescriptor socket_desc;

	socket_desc.port = GAME_PORT;
	socket_desc.socketFamily = AF_INET;

	if (peer->Startup(16, &socket_desc, 1) != SLNet::RAKNET_STARTED)
		return false;

	peer->SetMaximumIncomingConnections(16);
	peer->SetOccasionalPing(true);
	peer->SetUnreliableTimeout(1000);

	int settings_count = 0;

	auto dispatch_settings_file = [&](std::ifstream& file)
	{
		std::string line;

		while (std::getline(file, line))
		{
			std::smatch match;
			std::regex reg("([a-zA-Z0-9\\_]+)(\\([0-9]\\))\\=([a-zA-Z0-9\\_\\.]+)");

			if (std::regex_match(line, match, reg))
			{
				const auto value_name = match[1].str();

				switch (match[2].str()[1] - 0x30)
				{
				case 0:
				{
					const std::string value_str = match[3].str();

					bool value = false;

					switch (value_str.length())
					{
					case 1:
					{
						value = (*(uint8_t*)value_str.data() == '1');
						break;
					}
					case 4:
					{
						value = (*(uint32_t*)value_str.data() == 'eurt');
						break;
					}
					}

					println(color::cyan, "%s = %i", value_name.c_str(), value);

					settings.insert({ value_name, value });

					break;
				}
				case 1:
				{
					int value = std::atol(match[3].str().c_str());

					println(color::cyan, "%s = %i", value_name.c_str(), value);

					settings.insert({ value_name, value });

					break;
				}
				case 2:
				{
					float value = std::atof(match[3].str().c_str());

					println(color::cyan, "%s = %f", value_name.c_str(), value);

					settings.insert({ value_name, value });

					break;
				}
				}

				++settings_count;
			}
		}
	};

	println(color::yellow, "Settings:\n");

	if (auto settings_file = std::ifstream("settings.ini"))
	{
		dispatch_settings_file(settings_file);

		if (settings_count == 0)
			println(color::yellow, "None");

		println(color::yellow, "\nTotal settings: %i\n", settings_count);
	}
	else println(color::red, "settings.ini file does not exist, please re-download the file");

	if (!set_setting("sync_enemies", sync_enemies) ||
		!set_setting("sync_general_entities", sync_general_entities) ||
		!set_setting("sync_blocks", sync_blocks) ||
		!set_setting("sync_doors", sync_doors) ||
		!set_setting("sync_vehicles", sync_vehicles))
	{
		println(color::red, "There is a missing setting");
	}

	println(color::green, "TRIIO Server initialized\n");

	return (bs_ready = true);
}

void Server::dispatch_packets()
{
	for (SLNet::Packet* p = peer->Receive(); p; peer->DeallocatePacket(p), p = peer->Receive())
	{
		SLNet::BitStream bs_in(p->data, p->length, false);

		switch (const auto packet_id = get_packet_id(p, bs_in))
		{
		case ID_INCOMPATIBLE_PROTOCOL_VERSION:
			println(color::red, "Incompatible protocol version\n");
			break;
		case ID_DISCONNECTION_NOTIFICATION:
		case ID_CONNECTION_LOST:
		{
			if (packet_id == ID_DISCONNECTION_NOTIFICATION)
				println(color::yellow, "Disconnection notification from %s (0x%llx)", p->systemAddress.ToString(true), p->guid.g);
			else println(color::yellow, "Connection lost from 0x%llx", p->guid.g);

			player_joined_left_info info;

			if (auto it = clients.find(p->guid.g); it != clients.end())
			{
				info.guid = p->guid.g;

				strcpy_s(info.name, it->second.name.c_str());

				send_packet_broadcast(ID_PLAYER_LEFT, p->systemAddress, true, info);
			}

			for (const auto& [id, client_id] : streamed_entities)
				if (client_id == p->guid.g)
					streamed_entities.erase(id);

			clients.erase(p->guid.g);

			break;
		}
		case ID_NEW_INCOMING_CONNECTION:
		{
			println(color::green, "New connection from %s (0x%llx)", p->systemAddress.ToString(true), p->guid.g);

			player_list_info player_list {};

			for (const auto& [guid, ci] : clients)
			{
				player_list.guids[player_list.count] = guid;
				strcpy_s(player_list.names[player_list.count], ci.name.c_str());
				player_list.count++;
			}

			send_packet_broadcast(ID_PLAYER_LIST, p->systemAddress, false, player_list);

			clients.insert({ p->guid.g, { p->systemAddress.ToString(true), "" } });

			peer->SetTimeoutTime(2500, p->systemAddress);

			break;
		}
		case ID_CHAT_MESSAGE:
		{
			chat_message_info info; read_packet(bs_in, info);

			send_packet_broadcast(ID_CHAT_MESSAGE, p->systemAddress, true, info);

			break;
		}
		case ID_PLAYER_JOINED:
		{
			player_joined_left_info info; read_packet(bs_in, info);

			if (auto it = clients.find(p->guid.g); it != clients.end())
				it->second.name = info.name;

			send_packet_broadcast(ID_PLAYER_JOINED, p->systemAddress, true, info);

			break;
		}
		case ID_PLAYER_INFO:
		{
			player_info info; read_packet(bs_in, info);

			send_packet_broadcast(ID_PLAYER_INFO, p->systemAddress, true, info);

			break;
		}
		case ID_PLAYER_DESTROY:
		{
			uint64_t info; read_packet(bs_in, info);

			send_packet_broadcast(ID_PLAYER_DESTROY, p->systemAddress, true, info);

			break;
		}
		case ID_PLAYER_CHANGE_NAME:
		{
			player_change_name_info info; read_packet(bs_in, info);

			if (auto it = clients.find(p->guid.g); it != clients.end())
				it->second.name = info.new_name;

			send_packet_broadcast(ID_PLAYER_CHANGE_NAME, p->systemAddress, true, info);

			break;
		}
		case ID_ENABLE_ENEMY_AI:
		{
			enable_enemy_ai_info info; read_packet(bs_in, info);

			if (!sync_enemies)
				break;

			auto it = streamed_entities.find(info.id);

			if (it == streamed_entities.end())
				it = streamed_entities.insert({ info.id, p->guid.g }).first;

			if (it->second == p->guid.g)
				send_packet_broadcast(ID_ENABLE_ENEMY_AI, p->systemAddress, true, info);

			break;
		}
		case ID_ENEMY_INFO:
		{
			enemy_info info; read_packet(bs_in, info);

			if (!sync_enemies)
				break;

			if (auto it = streamed_entities.find(info.id);
				it != streamed_entities.end() && it->second == p->guid.g)
			{
				send_packet_broadcast(ID_ENEMY_INFO, p->systemAddress, true, info);
			}

			break;
		}
		case ID_DAMAGE_ENEMY:
		{
			damage_enemy_info info; read_packet(bs_in, info);

			if (!sync_enemies)
				break;

			send_packet_broadcast(ID_DAMAGE_ENEMY, p->systemAddress, true, info);

			break;
		}
		case ID_DAMAGE_PLAYER:
		{
			damage_player_info info; read_packet(bs_in, info);

			send_packet_broadcast(ID_DAMAGE_PLAYER, p->systemAddress, true, info);

			break;
		}
		case ID_RENDER_LIGHT:
		{
			render_light_info info; read_packet(bs_in, info);

			send_packet_broadcast(ID_RENDER_LIGHT, p->systemAddress, true, info);

			break;
		}
		case ID_PLAY_AUDIO:
		{
			play_audio_info info; read_packet(bs_in, info);
			
			if (info.vehicle_id != MAXUINT16)
			{
				if (auto it = streamed_entities.find(info.vehicle_id);
					it != streamed_entities.end() && it->second == p->guid.g)
				{
					send_packet_broadcast(ID_PLAY_AUDIO, p->systemAddress, true, info);
				}
			}
			else send_packet_broadcast(ID_PLAY_AUDIO, p->systemAddress, true, info);

			break;
		}
		case ID_STOP_AUDIO:
		{
			stop_audio_info info; read_packet(bs_in, info);

			send_packet_broadcast(ID_STOP_AUDIO, p->systemAddress, true, info);

			break;
		}
		case ID_CREATE_WATER_SPLASH:
		{
			create_water_splash_info info; read_packet(bs_in, info);

			send_packet_broadcast(ID_CREATE_WATER_SPLASH, p->systemAddress, true, info);

			break;
		}
		case ID_SET_ENTITY_ACTIVE:
		{
			set_entity_active_info info; read_packet(bs_in, info);

			if ((!sync_general_entities && info.type == 0x41554D) ||
				(!sync_blocks && info.type == 0x433D22))
				break;

			send_packet_broadcast(ID_SET_ENTITY_ACTIVE, p->systemAddress, true, info);

			break;
		}
		case ID_DESTROY_GLASS_WINDOW:
		{
			destroy_glass_window_info info; read_packet(bs_in, info);

			send_packet_broadcast(ID_DESTROY_GLASS_WINDOW, p->systemAddress, true, info);

			break;
		}
		case ID_FLIP_ROOM:
		{
			flip_room_info info; read_packet(bs_in, info);

			send_packet_broadcast(ID_FLIP_ROOM, p->systemAddress, true, info);

			break;
		}
		case ID_SET_ACTIVATION_FLAGS:
		{
			set_activation_flags_info info; read_packet(bs_in, info);

			if (!sync_doors)
				break;

			send_packet_broadcast(ID_SET_ACTIVATION_FLAGS, p->systemAddress, true, info);

			break;
		}
		case ID_VEHICLE_INFO:
		{
			vehicle_info info; read_packet(bs_in, info);

			if (!sync_vehicles)
				break;

			// use (anim_id == 0) as indicator that the player
			// left the vehicle so we can release the control etc

			if (info.anim_id != 0)
			{
				auto it = streamed_entities.find(info.id);

				if (it == streamed_entities.end())
					it = streamed_entities.insert({ info.id, p->guid.g }).first;

				if (it->second == p->guid.g)
					send_packet_broadcast(ID_VEHICLE_INFO, p->systemAddress, true, info);
			}
			else if (auto it = streamed_entities.find(info.id);
					 it != streamed_entities.end() && it->second == p->guid.g)
			{
				streamed_entities.erase(it);
			}

			break;
		}
		case ID_CREATE_BOAT_WATER_SPLASH:
		{
			create_boat_water_splash_info info; read_packet(bs_in, info);

			if (!sync_vehicles)
				break;

			send_packet_broadcast(ID_CREATE_BOAT_WATER_SPLASH, p->systemAddress, true, info);

			break;
		}
		default:
			println(color::red, "Unknown packet ID: %i", packet_id);
			break;
		}
	}
}