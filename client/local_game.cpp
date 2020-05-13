#include "local_game.h"

#include <offsets.h>
#include <entity.h>
#include <player.h>
#include <game_logic.h>

void LocalGame::change_level(uint8_t level)
{
	reinterpret_cast<uint16_t(*)(int32_t, int32_t)>(offsets::LOAD_LEVEL)(level, 1);
}

void LocalGame::set_friendly_fire(bool enabled)
{
	auto& players = g_player_system->get_players();

	for (const auto& [guid, pm] : players)
		if (pm.player)
		{
			if (enabled)
				g_entity_system->enable_enemy_ai(pm.game_id);
			else g_entity_system->disable_enemy_ai(pm.game_id);
		}

	friendly_fire = enabled;
}

void LocalGame::render_light(const ivec3& position, uint16_t unk0, int32_t unk1)
{
	reinterpret_cast<uint32_t(*)(uint32_t, int32_t, uint32_t, uint16_t, int32_t)>(offsets::RENDER_LIGHT)(position.x, position.y, position.z, unk0, unk1);
}

void LocalGame::set_default_combat_anim()
{
	reinterpret_cast<int32_t(*)(uint32_t, int32_t)>(offsets::SET_WEAPONS_ARMS_ANIM_ID)(offsets::LEFT_ARM_COMBAT_ANIM_BASE, 5);
	reinterpret_cast<int32_t(*)(uint32_t, int32_t)>(offsets::SET_WEAPONS_ARMS_ANIM_ID)(offsets::RIGHT_ARM_COMBAT_ANIM_BASE, 5);
}

bool LocalGame::is_level_completed()
{
	return *(bool*)offsets::IS_LEVEL_COMPLETED;
}

bool LocalGame::is_infinite_ammo_enabled()
{
	return *(bool*)offsets::INFINITE_AMMO;
}

bool LocalGame::give_item(const std::string& item, uint32_t amount, std::string* item_real_name)
{
	for (const auto& [name, id] : g_items)
	{
		if (name.find(item) != std::string::npos)
		{
			if (item_real_name)
				*item_real_name = name;

			for (auto i = 0; i < amount; ++i)
			{
				reinterpret_cast<uint32_t(*)(int32_t)>(offsets::GIVE_ITEM)(id);
				reinterpret_cast<uint32_t(*)(int32_t)>(offsets::RENDER_ITEM_ON_SCREEN)(id);
			}

			return true;
		}
	}

	return false;
}

uint16_t LocalGame::get_health()
{
	auto lara = g_entity_system->get_lara();

	if (!lara)
		return 0;

	return lara->health;
}

uint16_t LocalGame::get_oxygen()
{
	return *(uint16_t*)offsets::OXYGEN;
}

uint16_t LocalGame::get_movement_state()
{
	return *offvar::movement_state;
}

uint16_t LocalGame::get_combat_state()
{
	return *offvar::combat_state;
}

int32_t LocalGame::get_weapon_id_from_slot(int32_t a1)
{
	int32_t result = a1 - 1;

	switch (a1)
	{
	case 1:
		result = 1;
		break;
	case 2:
		result = 4;
		break;
	case 3:
		result = 5;
		break;
	case 4:
		result = 3;
		break;
	case 6:
		result = 7;
		break;
	case 7:
		result = 8;
		break;
	case 5:
		result = 6;
		break;
	default: return result;
	}

	return result;
}

int32_t LocalGame::get_height(uint32_t floor, const ivec3& position)
{
	return reinterpret_cast<int32_t(*)(uint32_t, int32_t, int32_t, int32_t)>(offsets::GET_HEIGHT)(floor, position.x, position.y, position.z);
}

uint32_t LocalGame::get_floor(const ivec3& position, uint16_t& out_room_id)
{
	return reinterpret_cast<uint32_t(*)(int32_t, int32_t, int32_t, uint16_t*)>(offsets::GET_FLOOR)(position.x, position.y, position.z, &out_room_id);
}

uint32_t LocalGame::get_time_to_pause()
{
	return *(uint32_t*)offsets::TIME_TO_PAUSE;
}

Entity* LocalGame::get_aiming_entity()
{
	return *(Entity**)offsets::AIMING_ENTITY;
}