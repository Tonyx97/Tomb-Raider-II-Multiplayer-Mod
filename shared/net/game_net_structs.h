#pragma once

#include <shared/game/math.h>

struct player_list_info
{
	uint64_t guids[16];

	char names[16][16];

	int16_t count = 0;
};

struct player_joined_left_info
{
	char name[16];

	uint64_t guid;
};

struct player_info
{
	uint64_t guid;
	
	uint8_t hair_transforms[0x78];

	ivec3 position;

	small_ivec3 rotation,
				head_rot,
				torso_rot,
				left_arm_rot,
				right_arm_rot;

	uint32_t left_hand_model_id,
			 right_hand_model_id,
			 key_controls,
			 mesh_bits;

	int32_t floor_y;

	uint16_t anim_id,
			 anim_frame,
			 room_id,
			 health,
			 activation_flags,
			 combat_state,
			 left_arm_combat_anim_frame,
			 right_arm_combat_anim_frame,
			 using_weapon_id,
			 equipped_back_weapon,
			 can_render_left_weapon_muzzle_flash,
			 can_render_right_weapon_muzzle_flash,
			 head_firing_model_id_index,
			 vehicle_id;

	uint8_t level;

	bool is_in_water,
		 is_exploded;
};

struct player_change_name_info
{
	char old_name[16],
		 new_name[16];

	uint64_t guid;
};

struct enable_enemy_ai_info
{
	uint64_t streamer;

	uint16_t id;

	uint8_t level;
};

struct enemy_info
{
	ivec3 position;

	small_ivec3 rotation;

	uint16_t id,
			 anim_id,
			 anim_frame,
			 room_id,
			 health;

	uint8_t level;
};

struct damage_enemy_info
{
	ivec3 impact_pos;

	uint16_t id,
			 damage;

	uint8_t level;
};

struct damage_player_info
{
	ivec3 impact_pos;

	uint64_t guid;

	uint16_t damage;

	uint8_t level;
};

struct render_light_info
{
	ivec3 position;

	int32_t unk1;

	uint16_t unk0;

	uint8_t level;
};

struct play_audio_info
{
	ivec3 position;

	uint64_t guid;

	uint32_t id,
			 sfx;

	uint16_t vehicle_id;

	uint8_t level;
};

struct stop_audio_info
{
	uint32_t id,
			 unk0;

	uint8_t level;
};

struct create_water_splash_info
{
	uint64_t guid;

	uint8_t level;
};

struct set_entity_active_info
{
	uint32_t type;

	uint16_t id,
			 activation_flags,
			 flags,
			 _0xe,
			 _0x10,
			 direction;
	
	uint8_t level;
};

struct destroy_glass_window_info
{
	uint32_t id;

	uint8_t level;
};

struct flip_room_info
{
	uint8_t level;
};

struct set_activation_flags_info
{
	uint16_t id,
			 activation_flags,
			 flags;

	uint8_t level;
};

struct vehicle_info
{
	ivec3 position;

	small_ivec3 rotation;

	uint16_t id,
			 anim_id,
			 anim_frame,
			 room_id,
			 xz_velocity,
			 y_velocity;

	uint8_t level;
};

struct create_boat_water_splash_info
{
	uint16_t id;

	uint8_t level;
};

struct chat_message_info
{
	char msg[64] { 0 };
};