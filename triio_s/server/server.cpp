#include "server.h"

#include <shared/game/math.h>

#include <shared/net/game_net_structs.h>

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
			printf("Incompatible protocol version\n");
			break;
		case ID_DISCONNECTION_NOTIFICATION:
		case ID_CONNECTION_LOST:
		{
			if (packet_id == ID_DISCONNECTION_NOTIFICATION)
				printf("Disconnection notification from %s (0x%llx)\n", p->systemAddress.ToString(true), p->guid.g);
			else printf("Connection lost from 0x%llx\n", p->guid.g);

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
			printf("New incoming connection from %s with GUID 0x%llx\n", p->systemAddress.ToString(true), p->guid.g);

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

			send_packet_broadcast(ID_SET_ACTIVATION_FLAGS, p->systemAddress, true, info);

			break;
		}
		case ID_VEHICLE_INFO:
		{
			vehicle_info info; read_packet(bs_in, info);

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

			send_packet_broadcast(ID_CREATE_BOAT_WATER_SPLASH, p->systemAddress, true, info);

			break;
		}
		default:
			printf("Unknown packet ID: %i\n", packet_id);
			break;
		}
	}
}