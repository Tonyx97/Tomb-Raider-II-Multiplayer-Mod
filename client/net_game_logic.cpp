#include "net_game_logic.h"

#include <client/client.h>

#include <console/console.h>

#include <debug.h>
#include <audio.h>
#include <gui.h>
#include <entity.h>
#include <player.h>
#include <local_game.h>
#include <game_logic.h>

void NetGameLogic::init()
{
	cl = g_client.get();
}

void NetGameLogic::on_client_connect()
{
	player_joined_left_info info;

	info.guid = cl->get_guid();

	strcpy_s(info.name, cl->get_name().c_str());
	
	cl->send_packet(ID_PLAYER_JOINED, info);
}

void NetGameLogic::on_chat_message()
{
	chat_message_info info; cl->read_packet_ex(info);

	g_gui->add_new_message(info.msg);
}

void NetGameLogic::on_player_list()
{
	player_list_info info; cl->read_packet_ex(info);

	for (auto i = 0; i < info.count; ++i)
	{
		PlayerMain player_main;

		player_main.name = info.names[i];
		player_main.player = nullptr;
		player_main.game_id = 0;

		g_player_system->add_player(info.guids[i], player_main);

		println(color::yellow, "player '%s' inserted", player_main.name.c_str());
	}
}

void NetGameLogic::on_player_joined()
{
	player_joined_left_info info; cl->read_packet_ex(info);

	PlayerMain player_main;

	player_main.name = info.name;
	player_main.player = nullptr;
	player_main.game_id = 0;

	g_player_system->add_player(info.guid, player_main);

	println(color::yellow, "player '%s' inserted by join", player_main.name.c_str());

	g_gui->add_new_message("%s joined the server", info.name);
}

void NetGameLogic::on_player_left()
{
	player_joined_left_info info; cl->read_packet_ex(info);

	auto& players = g_player_system->get_players();

	if (auto pm = g_player_system->get_player_by_guid(info.guid))
	{
		if (pm->player)
		{
			println(color::yellow, "[1] player '%s' memory deleted", pm->name.c_str());

			g_entity_system->disable_enemy_ai(pm->game_id);
			g_entity_system->remove_entity(pm->game_id);

			delete pm->player;
			pm->player = nullptr;
		}

		players.erase(info.guid);

		g_gui->add_new_message("%s left the server", info.name);
	}
}

void NetGameLogic::on_player_destroy()
{
	uint64_t player_guid; cl->read_packet_ex(player_guid);

	if (auto pm = g_player_system->get_player_by_guid(player_guid); pm && pm->player)
	{
		g_entity_system->remove_entity(pm->game_id);

		delete pm->player;
		pm->player = nullptr;

		println(color::yellow, "[2] player '%s' memory deleted", pm->name.c_str());
	}
}

void NetGameLogic::on_player_change_name()
{
	player_change_name_info info; cl->read_packet_ex(info);

	if (auto pm = g_player_system->get_player_by_guid(info.guid))
	{
		pm->name = info.new_name;

		g_gui->add_new_message("%s is now %s", info.old_name, info.new_name);
	}
}

void NetGameLogic::on_player_info()
{
	player_info info; cl->read_packet_ex(info);

	if (!offvar::level_loaded || info.level != *offvar::level_id)
		return;

	if (auto pm = g_player_system->get_player_by_guid(info.guid))
	{
		if (!pm->player)
		{
			pm->player = g_player_system->spawn_player(info.position,
													   info.rotation,
													   info.room_id,
													   pm->game_id);

			if (!pm->player)
				return dbg::mod_error_v(L"Failed spawning pending player");

			if (g_local_game->is_friendly_fire_enabled())
				g_entity_system->enable_enemy_ai(pm->game_id);
		}

		bool is_dead_now = g_entity_system->is_dead(info.health);

		// check if player had the weapons drawn to render the models
		// properly when dying

		if (pm->player->info.combat_state == 4 && info.combat_state == 0 && is_dead_now)
			pm->weapons_drawn_when_died = true;
		else if (!is_dead_now)
			pm->weapons_drawn_when_died = false;

		pm->player->info = info;

		if (info.is_exploded)
			g_entity_system->explode_entity(pm->player->handle, -1, 1);
	}
}

void NetGameLogic::on_enable_enemy_ai()
{
	enable_enemy_ai_info info; cl->read_packet_ex(info);

	if (!offvar::level_loaded || info.level != *offvar::level_id)
		return;

	auto enemy = g_entity_system->get_entity(info.id);

	if (enemy && g_game->enable_enemy_ai(info.id, false))
	{
		// make enemy visible
		enemy->flags &= 0xFFFB;
		enemy->flags |= 2;

		g_entity_system->set_entity_interactive(info.id);
	}
}

void NetGameLogic::on_enemy_info()
{
	enemy_info info; cl->read_packet_ex(info);

	if (!offvar::level_loaded || info.level != *offvar::level_id)
		return;

	if (Entity* enemy = g_entity_system->get_entity(info.id))
	{
		enemy->anim_id = info.anim_id;
		enemy->anim_frame = info.anim_frame;

		enemy->position = info.position;
		enemy->rotation = info.rotation;

		if (enemy->room_id != info.room_id)
			g_entity_system->move_entity_to_room(info.id, info.room_id);
	}
}

void NetGameLogic::on_damage_enemy()
{
	damage_enemy_info info; cl->read_packet_ex(info);

	if (!offvar::level_loaded || info.level != *offvar::level_id)
		return;

	if (Entity* entity = g_entity_system->get_entity(info.id))
		g_game->damage_enemy(entity, &info.impact_pos, info.damage);
}

void NetGameLogic::on_damage_player()
{
	damage_player_info info; cl->read_packet_ex(info);

	if (!offvar::level_loaded || info.level != *offvar::level_id)
		return;

	if (info.guid != cl->get_guid())
	{
		// damage other players

		if (auto pm = g_player_system->get_player_by_guid(info.guid); pm && pm->player)
			g_game->damage_enemy(pm->player->handle, &info.impact_pos, info.damage);
	} // damage localplayer
	else g_game->damage_enemy(g_entity_system->get_lara(), &info.impact_pos, info.damage);
}

void NetGameLogic::on_render_light()
{
	render_light_info info; cl->read_packet_ex(info);

	if (!offvar::level_loaded || info.level != *offvar::level_id)
		return;

	g_game->render_light(info.position.x, info.position.y, info.position.z, info.unk0, info.unk1);
}

void NetGameLogic::on_play_audio()
{
	play_audio_info info; cl->read_packet_ex(info);

	if (!offvar::level_loaded || info.level != *offvar::level_id || *offvar::game_state)
		return;

	// constant per-tick sounds like uzi firing has to be queued to play
	// them on every frame while shooting to avoid sound stuttering

	switch (eAudioID(info.id))
	{
	case eAudioID::M16_FIRE:
	case eAudioID::UZI_FIRE:
	{
		auto& players = g_player_system->get_players();

		if (auto pm = g_player_system->get_player_by_guid(info.guid); pm && pm->player)
			pm->pending_sounds.push_back({ info.position, info.id, info.sfx });

		return;
	}
	}

	// normal sound

	g_game->play_audio((eAudioID)info.id, &info.position, info.sfx);
}

void NetGameLogic::on_stop_audio()
{
	stop_audio_info info; cl->read_packet_ex(info);

	if (!offvar::level_loaded || info.level != *offvar::level_id)
		return;

	g_game->stop_audio((eAudioID)info.id, info.unk0);
}

void NetGameLogic::on_create_water_splash()
{
	create_water_splash_info info; cl->read_packet_ex(info);

	if (!offvar::level_loaded || info.level != *offvar::level_id)
		return;

	auto& players = g_player_system->get_players();

	if (auto pm = g_player_system->get_player_by_guid(info.guid); pm && pm->player)
		g_game->create_water_splash(pm->player->handle);
}

void NetGameLogic::on_set_entity_active()
{
	set_entity_active_info info; cl->read_packet_ex(info);

	if (!offvar::level_loaded || info.level != *offvar::level_id)
		return;

	if (auto entity = g_entity_system->get_entity(info.id))
	{
		switch (info.type)
		{
		case offsets::GENERAL_ENTITY_TRIGGER:
		{
			entity->_0x4 = 0;

			entity->activation_flags = info.activation_flags;

			g_game->set_entity_active(info.id);

			entity->flags &= 0xFFFB;
			entity->flags |= 2;

			break;
		}
		case offsets::MOVABLE_BLOCK_TRIGGER:
		{
			entity->_0x10 = info._0x10;

			switch (info.direction)
			{
			case 0: entity->rotation.y = 0x0;	 break;
			case 1: entity->rotation.y = 0x4000; break;
			case 2: entity->rotation.y = 0x8000; break;
			case 3: entity->rotation.y = 0xC000; break;
			}

			g_game->set_entity_active(info.id);

			g_entity_system->change_floor_height_on_entity(entity, 0x400);

			entity->flags &= 0xFFFB;
			entity->flags |= 2;

			g_entity_system->update_entity_anim(entity);

			break;
		}
		}
	}
}

void NetGameLogic::on_destroy_glass_window()
{
	destroy_glass_window_info info; cl->read_packet_ex(info);

	if (!offvar::level_loaded || info.level != *offvar::level_id)
		return;

	g_game->destroy_glass_window(info.id);
}

void NetGameLogic::on_flip_room()
{
	flip_room_info info; cl->read_packet_ex(info);

	if (!offvar::level_loaded || info.level != *offvar::level_id)
		return;

	g_game->flip_room();
}

void NetGameLogic::on_set_activation_flags()
{
	set_activation_flags_info info; cl->read_packet_ex(info);

	if (!offvar::level_loaded || info.level != *offvar::level_id)
		return;

	if (auto entity = g_entity_system->get_entity(info.id))
	{
		entity->activation_flags = info.activation_flags;
		entity->flags = info.flags;
	}
}

void NetGameLogic::on_vehicle_info()
{
	vehicle_info info; cl->read_packet_ex(info);

	if (!offvar::level_loaded || info.level != *offvar::level_id)
		return;

	if (auto entity = g_entity_system->get_entity(info.id))
	{
		entity->anim_id = info.anim_id;
		entity->anim_frame = info.anim_frame;
		entity->xz_velocity = info.xz_velocity;
		entity->y_velocity = info.y_velocity;
		entity->position = info.position;
		entity->rotation = info.rotation;

		if (entity->room_id != info.room_id)
			g_entity_system->move_entity_to_room(info.id, info.room_id);
	}
}

void NetGameLogic::on_create_boat_water_splash()
{
	create_boat_water_splash_info info; cl->read_packet_ex(info);

	if (!offvar::level_loaded || info.level != *offvar::level_id)
		return;

	if (auto entity = g_entity_system->get_entity(info.id))
		g_game->create_boat_water_splash(entity);
}