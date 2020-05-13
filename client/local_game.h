#pragma once

#ifndef LOCAL_GAME_H
#define LOCAL_GAME_H

#include <memory>
#include <string>
#include <unordered_map>
#include <map>

struct Entity;
struct ivec3;

inline std::unordered_map<std::string, uint8_t> g_levels =
{
	{ "thegreatwall", 1 },
	{ "venice", 2 },
	{ "bartolishideout", 3 },
	{ "operahouse", 4 },
	{ "offshorerig", 5 },
	{ "divingarea", 6 },
	{ "40fathoms", 7 },
	{ "wreckofmariadoria", 8 },
	{ "livingquarters", 9 },
	{ "thedeck", 10 },
	{ "tibetanfoothills", 11 },
	{ "barkhangmonastery", 12 },
	{ "catacombsofthetalion", 13 },
	{ "icepalace", 14 },
	{ "templeofxian", 15 },
	{ "floatingislands", 16 },
	{ "dragonslair", 17 },
	{ "homesweethome", 18 },
};

inline std::map<std::string, uint16_t> g_items =
{
	{ "pistols", 135 },
	{ "shotgun", 136 },
	{ "magnums", 137 },
	{ "uzis", 138 },
	{ "harpoon", 139 },
	{ "m16", 140 },
	{ "grenadelauncher", 141 },
	{ "shotgunammo", 143 },
	{ "magnumsammo", 144 },
	{ "uzisammo", 145 },
	{ "harpoonammo", 146 },
	{ "m16ammo", 147 },
	{ "grenadelauncherammo", 148 },
	{ "smallmedkit", 149 },
	{ "largemedkit", 150 },
	{ "flares", 151 },
};

struct LocalGame
{
private:

	bool friendly_fire = false,
		 godmode = false,
		 infinite_oxygen = false;

public:

	void change_level(uint8_t level);
	void set_friendly_fire(bool enabled);
	void render_light(const ivec3& position, uint16_t unk0, int32_t unk1);
	void set_default_combat_anim();
	void set_god_mode(bool enabled)				{ godmode = enabled; }
	void set_infinite_oxygen(bool enabled)		{ infinite_oxygen = enabled; }

	bool is_level_completed();
	bool is_infinite_ammo_enabled();
	bool give_item(const std::string& item, uint32_t amount = 1, std::string* item_real_name = nullptr);
	bool is_friendly_fire_enabled() const		{ return friendly_fire; }
	bool is_godmode_enabled() const				{ return godmode; }
	bool is_infinite_oxygen_enabled() const		{ return infinite_oxygen; }

	uint16_t get_health();
	uint16_t get_oxygen();
	uint16_t get_movement_state();
	uint16_t get_combat_state();

	int32_t get_weapon_id_from_slot(int32_t a1);
	int32_t get_height(uint32_t floor, const ivec3& position);

	uint32_t get_floor(const ivec3& position, uint16_t& out_room_id);
	uint32_t get_time_to_pause();

	Entity* get_aiming_entity();
};

inline std::unique_ptr<LocalGame> g_local_game;

#endif