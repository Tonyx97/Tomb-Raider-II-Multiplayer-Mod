#pragma once

#ifndef GAME_LOGIC_H
#define GAME_LOGIC_H

#include <memory>
#include <unordered_map>

struct Entity;
struct ivec3;

enum class eAudioID : uint32_t;

struct GameLogic
{
private:

	using update_input_t = int32_t(*)();
	using load_level_t = void*(*)(int32_t, int32_t);
	using render_game_t = int32_t(*)();
	using render_lara_t = int32_t(*)(Entity*);
	using enable_enemy_ai_t = int32_t(*)(uint16_t, int32_t);
	using update_enemy_logic_t = uint32_t(*)(uint16_t, uint16_t, uint32_t);
	using damage_enemy_t = uint16_t(*)(Entity*, ivec3*, uint16_t);
	using render_light_t = uint32_t(*)(uint32_t, int32_t, uint32_t, uint16_t, int32_t);
	using play_audio_t = uint32_t(*)(eAudioID, ivec3*, int32_t);
	using stop_audio_t = uint32_t(*)(eAudioID, uint32_t);
	using create_water_splash_t = uint32_t(*)(Entity*);
	using set_entity_active_t = Entity*(*)(uint16_t);
	using destroy_glass_window_t = bool(*)(uint16_t);
	using flip_room_t = uint64_t(*)();
	using trigger_switch_t = bool(*)(uint16_t, uint16_t);
	using create_boat_water_splash_t = int32_t(*)(Entity*);

	// normal hooks

	static int32_t hk_update_input();
	static void* hk_load_level(int32_t, int32_t);
	static int32_t hk_render_game();
	static int32_t hk_render_lara(Entity* entity);
	static int32_t hk_enable_enemy_ai(uint16_t, int32_t);
	static uint32_t hk_update_enemy_logic(uint16_t, uint16_t, uint32_t);
	static uint16_t hk_damage_enemy(Entity*, ivec3*, uint16_t);
	static uint32_t hk_render_light(uint32_t x, int32_t y, uint32_t z, uint16_t unk0, int32_t unk1);
	static uint32_t hk_play_audio(eAudioID id, ivec3* position, uint32_t sfx);
	static uint32_t hk_stop_audio(eAudioID id, uint32_t unk0);
	static uint32_t hk_create_water_splash(Entity* entity);
	static Entity* hk_set_entity_active(uint16_t index);
	static bool hk_destroy_glass_window(uint16_t index);
	static uint64_t hk_flip_room();
	static int32_t hk_create_boat_water_splash(Entity* entity);

	// mid hooks

	static void mid_hk_set_activation_flags_asm();
	static void mid_hk_set_activation_flags();

public:

	GameLogic()		{}
	~GameLogic()	{}

	bool init();
	bool destroy();

	inline static update_input_t update_input = nullptr;
	inline static load_level_t load_level = nullptr;
	inline static render_game_t render_game = nullptr;
	inline static render_lara_t render_lara = nullptr;
	inline static enable_enemy_ai_t enable_enemy_ai = nullptr;
	inline static update_enemy_logic_t update_enemy_logic = nullptr;
	inline static damage_enemy_t damage_enemy = nullptr;
	inline static render_light_t render_light = nullptr;
	inline static play_audio_t play_audio = nullptr;
	inline static stop_audio_t stop_audio = nullptr;
	inline static create_water_splash_t create_water_splash = nullptr;
	inline static set_entity_active_t set_entity_active = nullptr;
	inline static destroy_glass_window_t destroy_glass_window = nullptr;
	inline static flip_room_t flip_room = nullptr;
	inline static create_boat_water_splash_t create_boat_water_splash = nullptr;

};

inline std::unique_ptr<GameLogic> g_game;

#endif