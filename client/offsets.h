#pragma once

#ifndef OFFSETS_H
#define OFFSETS_H

#include <stdint.h>

#include <shared/game/math.h>

template <typename T, auto V>
struct global_access
{
	static constexpr uintptr_t ptr = V;

	T& operator = (const T& new_value)	{ return *(T*)ptr = new_value; }
	T& operator * ()					{ return *(T*)ptr; }
	operator bool ()					{ return !!(*(T*)ptr); }
};

// game offsets

namespace offsets
{
	// tables

	namespace
	{
		constexpr uintptr_t ENTITY_TABLE = 0x5262F0;
		constexpr uintptr_t ENTITIES_COUNT = 0x5207C6;
		constexpr uintptr_t CURRENT_ENTITY_INDEX = 0x5207C6;
		constexpr uintptr_t PER_ENTITY_SIZE = 0x48;
		
		constexpr uintptr_t ROOM_TABLE = 0x52618C;
		constexpr uintptr_t PER_ROOM_SIZE = 0x50;
		constexpr uintptr_t ROOMS_COUNT = 0x526180;

		constexpr uintptr_t WEAPON_DATA_TABLE = 0x465AFC;
		constexpr uintptr_t PER_WEAPON_SIZE = 0x2E;

		constexpr uintptr_t ANIM_BONES_TABLE = 0x525BE8;

		constexpr uintptr_t PARTICLES_TABLE = 0x5207C0;
		constexpr uintptr_t CURRENT_PARTICLE_INDEX = 0x5207C4;
		constexpr uintptr_t LAST_PARTICLE_INDEX = 0x5207CA;
		constexpr uintptr_t PER_PARTICLE_SIZE = 0x24;

		constexpr uintptr_t ANIM_TABLE = 0x5258F4;
		constexpr uintptr_t PER_ANIM_SIZE = 0x20;

		constexpr uintptr_t HEALTH_TABLE = 0x522024;

		constexpr uintptr_t UPDATE_FN_TABLE = 0x522010;

		constexpr uintptr_t BOX_COLLISION_TABLE = 0x5263CC;
		constexpr uintptr_t PER_BOX_COL_SIZE = 0x8;

		constexpr uintptr_t SFX_ID_TABLE = 0x51E6E0;

		constexpr uintptr_t SFX_TABLE = 0x51E9C4;
		constexpr uintptr_t SFX_COUNT = 0x51E6C0;

		constexpr uintptr_t MODEL_OFFSET_TABLE = 0x522002;
		constexpr uintptr_t MODEL_ID_TABLE = 0x5252B0;
	}

	// globals

	namespace
	{
		constexpr uintptr_t LARA = 0x5207BC;
		constexpr uintptr_t GAME_MEM_POOL = 0x51A0BC;
		constexpr uintptr_t GAME_MEM_POOL_MAX_SIZE = 0x519FF0;
		constexpr uintptr_t GAME_MEM_POOL_CURRENT_PTR = 0x519FF4;
		constexpr uintptr_t GAME_MEM_POOL_SIZE = 0x519FF8;
		constexpr uintptr_t GAME_MEM_POOL_FREE_SPACE = 0x519FFC;
		constexpr uintptr_t INVENTORY = 0x19FD44;
		constexpr uintptr_t CLICKED_ITEM_INVENTORY = 0x465A50;
		constexpr uintptr_t TIME_TO_PAUSE = 0x4D7798;
		constexpr uintptr_t USING_WEAPON_ID = 0x5206E4;
		constexpr uintptr_t COMBAT_STATE = 0x5206E2;
		constexpr uintptr_t CURRENT_WEAPON_ID = 0x5206E6;
		constexpr uintptr_t DRAWN_WEAPON_ID = 0x5206E8;
		constexpr uintptr_t LARA_LEFT_ARM_COMBAT_ANIM_FRAME = 0x52077C;
		constexpr uintptr_t LARA_RIGHT_ARM_COMBAT_ANIM_FRAME = 0x52078E;
		constexpr uintptr_t TOTAL_SHOTS = 0x51EE04;
		constexpr uintptr_t TOTAL_HITS = 0x51EE08;
		constexpr uintptr_t TOTAL_DIST_TRAVELLED = 0x51EE0C;
		constexpr uintptr_t TOTAL_KILLS = 0x51EE10;
		constexpr uintptr_t TOTAL_HEALS = 0x51EE13;
		constexpr uintptr_t SHOTGUN_AMMO = 0x5207A8;
		constexpr uintptr_t MAGNUMS_AMMO = 0x5207A0;
		constexpr uintptr_t UZI_AMMO = 0x5207A4;
		constexpr uintptr_t GRENADE_LAUNCHER_AMMO = 0x5207B0;
		constexpr uintptr_t HARPOON_AMMO = 0x5207AC;
		constexpr uintptr_t M16_AMMO = 0x5207B4;
		constexpr uintptr_t PISTOLS_AMMO = 0x52079C;
		constexpr uintptr_t INFINITE_AMMO = 0x51EE16;
		constexpr uintptr_t PRNG_SEED = 0x466BB0;
		constexpr uintptr_t IS_LEVEL_COMPLETED = 0x4D9EB4;
		constexpr uintptr_t CURRENT_ACTIVE_ENTITY_ID = 0x5207C8;
		constexpr uintptr_t READ_ENTITIES_COUNT = 0x4D7C28;
		constexpr uintptr_t FILE_OFFSET = 0x5206A4;
		constexpr uintptr_t AIMING_ENTITY = 0x520760;
		constexpr uintptr_t OXYGEN = 0x5206F6;
		constexpr uintptr_t MOVEMENT_STATE = 0x5206EC;
		constexpr uintptr_t CAMERA_POS = 0x526384;
		constexpr uintptr_t CAMERA_FOV = 0x4B2AA8;
		constexpr uintptr_t SFX_LOADED = 0x51E6C4;
		constexpr uintptr_t CAN_LOAD_SFX = 0x4D8D70;
		constexpr uintptr_t ASPECT_RATIO = 0x46C2F4;
		constexpr uintptr_t ZOOM_OUT = 0x526340;
		constexpr uintptr_t LAST_LARA_POSITION = 0x520710;
		constexpr uintptr_t EQUIPPED_BACK_WEAPON = 0x520706;
		constexpr uintptr_t HEAD_FIRING_MODEL_ID_INDEX = 0x5220F2;
		constexpr uintptr_t CURRENT_ROOM_ID = 0x52632C;
		constexpr uintptr_t IS_LARA_IN_WATER = 0x4D6F68;
		constexpr uintptr_t CAN_RENDER_LEFT_WEAPON_MUZZLE_FLASH = 0x520788;
		constexpr uintptr_t CAN_RENDER_RIGHT_WEAPON_MUZZLE_FLASH = 0x52079A;
		constexpr uintptr_t LARA_SHADOW_SIZE = 0x52202C;
		constexpr uintptr_t LEVEL_ID = 0x4D9EB0;
		constexpr uintptr_t LARA_TIME_DEAD = 0x5206FA;
		constexpr uintptr_t GAME_STAGE = 0x4644E0;								// 1 - ig/main menu | 0 - inventory | 2 - game over
		constexpr uintptr_t TOTAL_VISIBLE_ROOMS = 0x46C2F0;
		constexpr uintptr_t LEVEL_LOADED = 0x4D9EBC;
		constexpr uintptr_t MUSIC_VOLUME = 0x465A60;
		constexpr uintptr_t SFX_VOLUME = 0x465A5C;
		constexpr uintptr_t LARA_IS_WATER_ROOM = 0x526190;
		constexpr uintptr_t DYNAMIC_LIGHTS_COUNT = 0x525BEC;
		constexpr uintptr_t GAME_STATE = 0x4D7968;
		constexpr uintptr_t LEFT_ARM_COMBAT_ANIM_BASE = 0x520778;
		constexpr uintptr_t RIGHT_ARM_COMBAT_ANIM_BASE = 0x52078A;
		constexpr uintptr_t LARA_CURRENT_VEHICLE = 0x520702;
		
		// renderer globals

		constexpr uintptr_t RENDERER_TYPE = 0x51BCC0;							// 1 - software | 2 - hardware
		constexpr uintptr_t RENDERER_4_3 = 0x51BCD5;
		constexpr uintptr_t RENDERER_16_9 = 0x51BCCC;
		constexpr uintptr_t PERSPECTIVE_CORRECT = 0x51BCD0;
		constexpr uintptr_t DITHER = 0x51BCD1;
		constexpr uintptr_t Z_BUFFER = 0x51BCD2;
		constexpr uintptr_t Z_BUFFER_DISTANCE = 0x46408C;
		constexpr uintptr_t BILINEAR_FILTER = 0x51BCD3;
		constexpr uintptr_t TRIPLE_BUFFER = 0x51BCD4;
		constexpr uintptr_t SCREEN_SX = 0x4B2AE4;
		constexpr uintptr_t SCREEN_SY = 0x47031C;
		constexpr uintptr_t SCREEN_SX_R = 0x466BE8;
		constexpr uintptr_t SCREEN_SY_R = 0x466BEA;
		constexpr uintptr_t VIEWPORT_CENTER_X = 0x470320;
		constexpr uintptr_t VIEWPORT_CENTER_Y = 0x470324;
		constexpr uintptr_t VIEWPORT_X = 0x4B2A00;
		constexpr uintptr_t VIEWPORT_Y = 0x47803C;
		constexpr uintptr_t VIEWPORT_SX = 0x4B29E0;
		constexpr uintptr_t VIEWPORT_SY = 0x4B2A0C;
		
		// gui globals

		constexpr uintptr_t GUI_ELEMENTS_COUNT = 0x51D6A0;
		constexpr uintptr_t GUI_TEXT_OFFSET = 0x36;
		constexpr uintptr_t GUI_STRINGS_TABLE = 0x51D6C0;
		constexpr uintptr_t GUI_DYNAMIC_STRINGS_TABLE = 0x521F50;
		constexpr uintptr_t IG_AMMO_TEXT_ELEMENT = 0x4D791C;
		constexpr uintptr_t INVENTORY_AMMO_TEXT_ELEMENT = 0x4D793C;
		constexpr uintptr_t PER_GUI_ELEMENT_SIZE = 0x3A;
		constexpr uintptr_t PER_STRING_SIZE = 0x40;
		constexpr uintptr_t COLOR_BLACK = 0x5216E0;
		constexpr uintptr_t COLOR_GREY = 0x5216E2;
		constexpr uintptr_t COLOR_WHITE = 0x5216E4;
		constexpr uintptr_t COLOR_RED = 0x5216E6;
		constexpr uintptr_t COLOR_ORANGE = 0x5216E8;
		constexpr uintptr_t COLOR_YELLOW = 0x5216EA;
		constexpr uintptr_t COLOR_BLACK_GREEN = 0x5216F8;
		constexpr uintptr_t COLOR_GREEN = 0x5216FA;
		constexpr uintptr_t COLOR_CYAN = 0x5216FC;
		constexpr uintptr_t COLOR_BLUE = 0x5216FE;
		constexpr uintptr_t COLOR_PURPLE = 0x521700;
		constexpr uintptr_t CAN_DRAW_MENU_BG_IMAGE = 0x5249FE;
		constexpr uintptr_t WEAPON_AMMO_DISPLAY_FORMAT = 0x4644F0;
		constexpr uintptr_t DISPLAY_INVENTORY_WEAPON_AMMO = 0x4D793C;
		constexpr uintptr_t MENU_SETTINGS_KEYS_TITLE_TEXT = 0x51A2F0;
		
		// controls & keys globals

		constexpr uintptr_t KEYBOARD_DEVICE = 0x4D8550;
		constexpr uintptr_t KEY_CONTROLS = 0x51A1F8;
		constexpr uintptr_t JOYSTICK_BUTTONS = 0x51A200;
		constexpr uintptr_t JOYSTICK_PRESENT = 0x51BCD8;
		constexpr uintptr_t JOYSTICK_CAPS_AVAILABLE = 0x466478;
		constexpr uintptr_t JOYSTICK_X = 0x51A204;
		constexpr uintptr_t JOYSTICK_Y = 0x51A208;
		constexpr uintptr_t KEY_STATES = 0x51A0F8;
		
		// lara look/targeting rotations globals

		constexpr uintptr_t HEAD_ROTATION = 0x52076C;
		constexpr uintptr_t TORSO_ROTATION = 0x520772;
		constexpr uintptr_t LEFT_ARM_ROTATION = 0x520782;
		constexpr uintptr_t RIGHT_ARM_ROTATION = 0x520794;
		constexpr uintptr_t HAIR_TRANSFORMS = 0x521D40;
		constexpr uintptr_t HAIR_TRANSFORMS_SIZE = 0x78;

		// lara model ids globals

		constexpr uintptr_t HEAD_MODEL_ID = 0x52075C;
		constexpr uintptr_t TORSO_MODEL_ID = 0x520740;
		constexpr uintptr_t PELVIS_MODEL_ID = 0x520724;
		constexpr uintptr_t LEFT_HOLSTER_MODEL_ID = 0x520728;
		constexpr uintptr_t LEFT_LEG_MODEL_ID = 0x52072C;
		constexpr uintptr_t LEFT_FOOT_MODEL_ID = 0x520730;
		constexpr uintptr_t LEFT_UPPER_ARM_MODEL_ID = 0x520750;
		constexpr uintptr_t LEFT_ARM_MODEL_ID = 0x520754;
		constexpr uintptr_t LEFT_HAND_MODEL_ID = 0x520758;
		constexpr uintptr_t RIGHT_HOLSTER_MODEL_ID = 0x520734;
		constexpr uintptr_t RIGHT_LEG_MODEL_ID = 0x520738;
		constexpr uintptr_t RIGHT_FOOT_MODEL_ID = 0x52073C;
		constexpr uintptr_t RIGHT_UPPER_ARM_MODEL_ID = 0x520744;
		constexpr uintptr_t RIGHT_ARM_MODEL_ID = 0x520748;
		constexpr uintptr_t RIGHT_HAND_MODEL_ID = 0x52074C;
	}

	// functions

	namespace
	{
		constexpr uintptr_t GAME_WINDOW_PROC = 0x446260;
		constexpr uintptr_t ENABLE_ENEMY_AI = 0x432BC0;
		constexpr uintptr_t DISABLE_ENEMY_AI = 0x432B70;
		constexpr uintptr_t DAMAGE_ENEMY = 0x42F6E0;
		constexpr uintptr_t PLAY_AUDIO = 0x43F470;
		constexpr uintptr_t STOP_AUDIO = 0x43F910;
		constexpr uintptr_t SPAWN_BLOOD_PARTICLE = 0x41C5B0;
		constexpr uintptr_t UPDATE_ENTITY_ANIM = 0x4146C0;
		constexpr uintptr_t DOOR_UPDATE_FN = 0x435570;
		constexpr uintptr_t SET_ENTITY_ACTIVE = 0x427150;
		constexpr uintptr_t GET_FREE_ENTITY_SLOT = 0x426E50;
		constexpr uintptr_t RENDER_LARA = 0x419DD0;
		constexpr uintptr_t MOVE_ENTITY_TO_ROOM = 0x4271B0;
		constexpr uintptr_t MOVE_EFFECT_TO_ROOM = 0x427460;
		constexpr uintptr_t SET_WEAPONS_ARMS_ANIM_ID = 0x42D000;
		constexpr uintptr_t LARA_SETUP_FN = 0x430FB0;
		constexpr uintptr_t LOAD_LEVEL = 0x43A330;
		constexpr uintptr_t REMOVE_ENTITY = 0x426D30;
		constexpr uintptr_t SETUP_ENTITY = 0x426E90;
		constexpr uintptr_t UPDATE_ENEMY_LOGIC = 0x40F500;
		constexpr uintptr_t GET_COLOR_FROM_RGB = 0x44C2A0;
		constexpr uintptr_t RENDER_ITEM_ON_SCREEN = 0x421F40;
		constexpr uintptr_t GIVE_ITEM = 0x4242D0;
		constexpr uintptr_t CREATE_GUI_TEXT = 0x440530;
		constexpr uintptr_t REMOVE_GUI_TEXT = 0x440940;
		constexpr uintptr_t TEXT_ALIGN_RIGHT = 0x4407F0;
		constexpr uintptr_t TEXT_ALIGN_BOTTOM = 0x440810;
		constexpr uintptr_t TEXT_ALIGN_CENTER_H = 0x4407B0;
		constexpr uintptr_t TEXT_ALIGN_CENTER_V = 0x4407D0;
		constexpr uintptr_t TEXT_SET_SCALE = 0x440680;
		constexpr uintptr_t TEXT_SET_FLASHING = 0x4406A0;
		constexpr uintptr_t TEXT_SET_BOX = 0x440770;
		constexpr uintptr_t TEXT_SET_BG = 0x4406D0;
		constexpr uintptr_t DRAW_LINE = 0x409D80;
		constexpr uintptr_t TEXT_GET_SCALE_SCREEN_X = 0x440F40;
		constexpr uintptr_t TEXT_GET_SCALE_SCREEN_Y = 0x440F80;
		constexpr uintptr_t TEXT_UPDATE = 0x440640;
		constexpr uintptr_t PARTICLE_CLOSE_TO_LARA = 0x41C4B0;
		constexpr uintptr_t EXPLODE_ENTITY = 0x433410;
		constexpr uintptr_t UPDATE_INPUT = 0x44DAD0;
		constexpr uintptr_t RENDER_GAME = 0x418960;
		constexpr uintptr_t RENDER_LIGHT = 0x41C0D0;
		constexpr uintptr_t CREATE_WATER_SPLASH = 0x41CA70;
		constexpr uintptr_t CREATE_BOAT_WATER_SPLASH = 0x40D0F0;
		constexpr uintptr_t DESTROY_GLASS_WINDOW = 0x434EB0;
		constexpr uintptr_t CHANGE_FLOOR_HEIGHT = 0x434160;
		constexpr uintptr_t FLIP_ROOM = 0x416610;
		constexpr uintptr_t GET_FLOOR = 0x414B40;
		constexpr uintptr_t GET_HEIGHT = 0x414E50;
		constexpr uintptr_t TRIGGER_SWITCH = 0x438E30;
		constexpr uintptr_t TEST_TRIGGERS = 0x4151C0;
		constexpr uintptr_t BOAT_ANIM = 0x40D930;
		
		
		
		
		
	}

	// entity

	namespace
	{
		constexpr uintptr_t ROOM_DATA = 0x0;
		constexpr uintptr_t MODEL = 0xC;
		constexpr uintptr_t ANIM_ID = 0x14;
		constexpr uintptr_t ANIM_FRAME = 0x16;
		constexpr uintptr_t ROOM_ID = 0x18;
		constexpr uintptr_t ROOM_UNK_DATA = 0x1A;
		constexpr uintptr_t XZ_VELOCITY = 0x1E;
		constexpr uintptr_t Y_VELOCITY = 0x20;
		constexpr uintptr_t HEALTH = 0x22;
		constexpr uintptr_t POSITION = 0x34;
		constexpr uintptr_t ROTATION = 0x40;
	}

	// inventory

	namespace
	{
		constexpr uintptr_t SELECTED_ITEM = 0x10;
	}

	// patches

	namespace
	{
		constexpr uintptr_t INVISIBLE_LARA_CHECK = 0x418B89;
		constexpr uintptr_t INVISIBLE_LARA_CHECK_SIZE = 0xF;

		constexpr uintptr_t SET_DYNAMIC_LIGHTS_TO_ZERO = 0x41456A;
		constexpr uintptr_t SET_DYNAMIC_LIGHTS_TO_ZERO_SIZE = 0x6;
	}

	// return addresses

	namespace
	{
		// sound ignoring using ret addresses

		constexpr uintptr_t IGNORE_SOUNDS_ANIM_IN_RANGE_BEGIN = UPDATE_ENTITY_ANIM;
		constexpr uintptr_t IGNORE_SOUNDS_ANIM_IN_RANGE_END = 0x4149F7;

		constexpr uintptr_t IGNORE_SOUNDS_ROLLING_SPIDLES_IN_RANGE_BEGIN = 0x441640;
		constexpr uintptr_t IGNORE_SOUNDS_ROLLING_SPIDLES_IN_RANGE_END = 0x4417BA;

		constexpr uintptr_t IGNORE_SOUNDS_DISK_EMITTER_IN_RANGE_BEGIN = 0x4428F0;
		constexpr uintptr_t IGNORE_SOUNDS_DISK_EMITTER_IN_RANGE_END = 0x442A2D;

		constexpr uintptr_t IGNORE_SOUNDS_DISK_IN_RANGE_BEGIN = 0x442A30;
		constexpr uintptr_t IGNORE_SOUNDS_DISK_IN_RANGE_END = 0x442B84;
		
		constexpr uintptr_t IGNORE_SOUNDS_SFX_BEGIN = 0x41C540;
		constexpr uintptr_t IGNORE_SOUNDS_SFX_END = 0x41C5A7;

		constexpr uintptr_t IGNORE_SOUNDS_BOAT_BEGIN = 0x40DAA0;
		constexpr uintptr_t IGNORE_SOUNDS_BOAT_END = 0x40E0C9;

		constexpr uintptr_t IGNORE_SOUNDS_SKIDOO_1_BEGIN = 0x43E6B0;
		constexpr uintptr_t IGNORE_SOUNDS_SKIDOO_1_END = 0x43EAEF;

		constexpr uintptr_t IGNORE_SOUNDS_SKIDOO_2_BEGIN = 0x43DFF0;
		constexpr uintptr_t IGNORE_SOUNDS_SKIDOO_2_END = 0x43E2A8;

		constexpr uintptr_t IGNORE_SOUNDS_INVENTORY_BEGIN = 0x422060;
		constexpr uintptr_t IGNORE_SOUNDS_INVENTORY_END = 0x4231FD;

		constexpr uintptr_t IGNORE_SOUNDS_ROLLING_BOULDER_BEGIN = 0x441C20;
		constexpr uintptr_t IGNORE_SOUNDS_ROLLING_BOULDER_END = 0x441F66;

		constexpr uintptr_t IGNORE_SOUNDS_PROPELLER_BEGIN = 0x4414B0;
		constexpr uintptr_t IGNORE_SOUNDS_PROPELLER_END = 0x44163F;
		
		constexpr uintptr_t IGNORE_SOUNDS_SKIDOO_EXPLODE = 0x43E33E;

		constexpr uintptr_t IGNORE_SOUNDS_SPIKE_WALL_MOVING = 0x44128E;

		// triggering system filtering using ret addresses

		constexpr uintptr_t GENERAL_ENTITY_TRIGGER = 0x41554D;
		constexpr uintptr_t MOVABLE_BLOCK_TRIGGER = 0x433D22;

		// filtering room flipping

		constexpr uintptr_t FLIP_ROOM_TRIGGER = 0x415820;
	}

	// addresses mid func

	namespace
	{
		constexpr uintptr_t SET_ACTIVATION_FLAGS_MF = 0x415466;
		constexpr uintptr_t BOAT_SET_MAX_VELOCITY = 0x40D88B;
		constexpr uintptr_t SKIDOO_SET_MAX_VELOCITY = 0x43DE0F;
	}
};

// direct access to offsets

namespace offvar
{
	inline global_access<small_ivec3, offsets::HEAD_ROTATION> head_rotation;
	inline global_access<small_ivec3, offsets::TORSO_ROTATION> torso_rotation;
	inline global_access<small_ivec3, offsets::LEFT_ARM_ROTATION> left_arm_rotation;
	inline global_access<small_ivec3, offsets::RIGHT_ARM_ROTATION> right_arm_rotation;

	inline global_access<uint32_t, offsets::KEY_CONTROLS> key_controls;
	inline global_access<uint32_t, offsets::HEAD_MODEL_ID> head_model_id;
	inline global_access<uint32_t, offsets::LEFT_HAND_MODEL_ID> left_hand_model_id;
	inline global_access<uint32_t, offsets::RIGHT_HAND_MODEL_ID> right_hand_model_id;
	inline global_access<uint32_t, offsets::LEFT_HOLSTER_MODEL_ID> left_holster_model_id;
	inline global_access<uint32_t, offsets::RIGHT_HOLSTER_MODEL_ID> right_holster_model_id;
	inline global_access<uint32_t, offsets::DYNAMIC_LIGHTS_COUNT> dynamic_lights_count;
	inline global_access<uint32_t, offsets::IS_LEVEL_COMPLETED> level_completed;
	inline global_access<uint32_t, offsets::BOAT_SET_MAX_VELOCITY> boat_set_max_velocity;
	inline global_access<uint32_t, offsets::SKIDOO_SET_MAX_VELOCITY> skidoo_set_max_velocity;

	inline global_access<uint16_t, offsets::HEAD_FIRING_MODEL_ID_INDEX> head_firing_model_id_index;
	inline global_access<uint16_t, offsets::MOVEMENT_STATE> movement_state;
	inline global_access<uint16_t, offsets::COMBAT_STATE> combat_state;
	inline global_access<uint16_t, offsets::USING_WEAPON_ID> using_weapon_id;
	inline global_access<uint16_t, offsets::EQUIPPED_BACK_WEAPON> equipped_back_weapon;
	inline global_access<uint16_t, offsets::CAN_RENDER_LEFT_WEAPON_MUZZLE_FLASH> can_render_left_weapon_muzzle_flash;
	inline global_access<uint16_t, offsets::CAN_RENDER_RIGHT_WEAPON_MUZZLE_FLASH> can_render_right_weapon_muzzle_flash;
	inline global_access<uint16_t, offsets::LARA_LEFT_ARM_COMBAT_ANIM_FRAME> left_arm_combat_anim_frame;
	inline global_access<uint16_t, offsets::LARA_RIGHT_ARM_COMBAT_ANIM_FRAME> right_arm_combat_anim_frame;
	inline global_access<uint16_t, offsets::LARA_CURRENT_VEHICLE> lara_current_vehicle;
	inline global_access<uint16_t, offsets::OXYGEN> oxygen;

	inline global_access<uint8_t, offsets::LEVEL_ID> level_id;

	inline global_access<bool, offsets::IS_LARA_IN_WATER> is_lara_in_water;
	inline global_access<bool, offsets::LEVEL_LOADED> level_loaded;
	inline global_access<bool, offsets::GAME_STATE> game_state;
	inline global_access<bool, offsets::INFINITE_AMMO> infinite_ammo;
};

#endif