#pragma once

#ifndef NET_GAME_LOGIC_H
#define NET_GAME_LOGIC_H

#include <memory>

struct NetGameLogic
{
private:

	class Client* cl = nullptr;

public:

	void init();

	// net game logic dispatching

	void on_client_connect();
	void on_chat_message();
	void on_player_list();
	void on_player_joined();
	void on_player_left();
	void on_player_destroy();
	void on_player_change_name();
	void on_player_info();
	void on_enable_enemy_ai();
	void on_enemy_info();
	void on_damage_enemy();
	void on_damage_player();
	void on_render_light();
	void on_play_audio();
	void on_stop_audio();
	void on_create_water_splash();
	void on_set_entity_active();
	void on_destroy_glass_window();
	void on_flip_room();
	void on_set_activation_flags();
	void on_vehicle_info();
	void on_create_boat_water_splash();
	
};

inline std::unique_ptr<NetGameLogic> g_net_game;

#endif