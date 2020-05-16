#include "client.h"

#include <cstdio>
#include <stdlib.h>
#include <chrono>

#include <shared/game/math.h>

#include <shared/console/console.h>
#include <shared/registry/registry.h>

#include <entity.h>
#include <net_game_logic.h>
#include <game_logic.h>
#include <gui.h>

Client::Client()
{
	peer = SLNet::RakPeerInterface::GetInstance();
}

Client::~Client()
{
	if (!peer)
		return;

	peer->Shutdown(100);

	SLNet::RakPeerInterface::DestroyInstance(peer);
}

bool Client::init()
{
	if (!peer)
		return false;

	SLNet::SocketDescriptor socket_desc(0, 0);
	
	socket_desc.socketFamily = AF_INET;

	if (peer->Startup(1, &socket_desc, 1) != SLNet::RAKNET_STARTED)
		return false;

	peer->SetOccasionalPing(true);

	name = g_registry->get_string("nickname");

	return true;
}

bool Client::connect(const char* ip, uint16_t port)
{
	return (peer->Connect(ip, port, "pass", sizeof("pass")) == SLNet::CONNECTION_ATTEMPT_STARTED);
}

void Client::dispatch_packets()
{
	for (SLNet::Packet* p = peer->Receive(); p; peer->DeallocatePacket(p), p = peer->Receive())
	{
		SLNet::BitStream bs_in(p->data, p->length, false);

		const auto packet_id = get_packet_id(p, bs_in);

		current_bs = &bs_in;

		switch (packet_id)
		{
		case ID_DISCONNECTION_NOTIFICATION:
			println(color::red, "Disconnection notification");
			connected = bs_ready = false;
			break;
		case ID_ALREADY_CONNECTED:
			println(color::red, "ID already connected with guid 0x%llx", p->guid.g);
			break;
		case ID_INCOMPATIBLE_PROTOCOL_VERSION:
			println(color::red, "ID_INCOMPATIBLE_PROTOCOL_VERSION");
			break;
		case ID_REMOTE_DISCONNECTION_NOTIFICATION:
			println(color::red, "Remote disconnection notification");
			break;
		case ID_REMOTE_CONNECTION_LOST:
			println(color::red, "Remote connection lost");
			break;
		case ID_REMOTE_NEW_INCOMING_CONNECTION:
			println(color::red, "Remote new incoming connection");
			break;
		case ID_CONNECTION_BANNED:
			println(color::red, "You are banned from this server");
			break;
		case ID_CONNECTION_ATTEMPT_FAILED:
			println(color::red, "Connection attempt failed");
			break;
		case ID_NO_FREE_INCOMING_CONNECTIONS:
			println(color::red, "No free incoming connections");
			break;
		case ID_INVALID_PASSWORD:
			println(color::red, "Invalid password");
			break;
		case ID_CONNECTION_LOST:
			println(color::red, "Connection lost");
			connected = bs_ready = false;
			break;
		case ID_CONNECTION_REQUEST_ACCEPTED:
		{
			println(color::green, "Connected to server %s (0x%llx)", p->systemAddress.ToString(true), peer->GetMyGUID().g);
			
			guid = peer->GetMyGUID().g;
			
			connected = bs_ready = true;

			g_net_game->on_client_connect();

			break;
		}
		case ID_CHAT_MESSAGE:				g_net_game->on_chat_message();				break;
		case ID_PLAYER_LIST:				g_net_game->on_player_list();				break;
		case ID_PLAYER_JOINED:				g_net_game->on_player_joined();				break;
		case ID_PLAYER_LEFT:				g_net_game->on_player_left();				break;
		case ID_PLAYER_DESTROY:				g_net_game->on_player_destroy();			break;
		case ID_PLAYER_CHANGE_NAME:			g_net_game->on_player_change_name();		break;
		case ID_PLAYER_INFO:				g_net_game->on_player_info();				break;
		case ID_ENABLE_ENEMY_AI:			g_net_game->on_enable_enemy_ai();			break;
		case ID_ENEMY_INFO:					g_net_game->on_enemy_info();				break;
		case ID_DAMAGE_ENEMY:				g_net_game->on_damage_enemy();				break;
		case ID_DAMAGE_PLAYER:				g_net_game->on_damage_player();				break;
		case ID_RENDER_LIGHT:				g_net_game->on_render_light();				break;
		case ID_PLAY_AUDIO:					g_net_game->on_play_audio();				break;
		case ID_STOP_AUDIO:					g_net_game->on_stop_audio();				break;
		case ID_CREATE_WATER_SPLASH:		g_net_game->on_create_water_splash();		break;
		case ID_SET_ENTITY_ACTIVE:			g_net_game->on_set_entity_active();			break;
		case ID_DESTROY_GLASS_WINDOW:		g_net_game->on_destroy_glass_window();		break;
		case ID_FLIP_ROOM:					g_net_game->on_flip_room();					break;
		case ID_SET_ACTIVATION_FLAGS:		g_net_game->on_set_activation_flags();		break;
		case ID_VEHICLE_INFO:				g_net_game->on_vehicle_info();				break;
		case ID_CREATE_BOAT_WATER_SPLASH:	g_net_game->on_create_boat_water_splash();	break;
		default:
			println(color::red, "Unknown packet ID: %i", packet_id);
			break;
		}
	}
}