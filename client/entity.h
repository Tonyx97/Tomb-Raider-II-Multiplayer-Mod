#pragma once

#ifndef ENTITY_H
#define ENTITY_H

#include <functional>
#include <atomic>
#include <any>

#include <shared/game/math.h>

#include <shared/net/game_net_structs.h>

#include <offsets.h>

enum eVelocityDirection : uintptr_t
{
	HORIZONTAL = offsets::XZ_VELOCITY,
	VERTICAL = offsets::Y_VELOCITY
};

enum eEntityModelID : unsigned short
{
	ID_NONE = -1,
	ID_LARA = 0,
	ID_LARA_PISTOLS,
	ID_LARA_HAIR,
	ID_LARA_SHOTGUN,
	ID_LARA_MAGNUMS,
	ID_LARA_UZIS,
	ID_LARA_M16,
	ID_LARA_GRENADE,
	ID_LARA_HARPOON,
	ID_LARA_FLARE,
	ID_LARA_SKIDOO,
	ID_LARA_BOAT,
	ID_LARA_EXTRA,
	ID_SKIDOO_FAST,
	ID_BOAT,
	ID_DOG,
	ID_CULT1,
	ID_CULT1A,
	ID_CULT1B,
	ID_CULT2,
	ID_CULT3,
	ID_MOUSE,
	ID_DRAGON_FRONT,
	ID_DRAGON_BACK,
	ID_GONDOLA,
	ID_SHARK,
	ID_EEL,
	ID_BIG_EEL,
	ID_BARRACUDA,
	ID_DIVER,
	ID_WORKER1,
	ID_WORKER2,
	ID_WORKER3,
	ID_WORKER4,
	ID_WORKER5,
	ID_JELLY,
	ID_SPIDER_or_WOLF,
	ID_BIG_SPIDER_or_BEAR,
	ID_CROW,
	ID_TIGER,
	ID_BARTOLI,
	ID_XIAN_LORD,
	ID_CHINESE2,
	ID_WARRIOR,
	ID_CHINESE4,
	ID_YETI,
	ID_GIANT_YETI,
	ID_EAGLE,
	ID_BANDIT1,
	ID_BANDIT2,
	ID_BANDIT2B,
	ID_SKIDOO_ARMED,
	ID_SKIDMAN,
	ID_MONK1,
	ID_MONK2,
	ID_FALLING_BLOCK1,
	ID_FALLING_BLOCK2,
	ID_FALLING_BLOCK3,
	ID_PENDULUM1,
	ID_SPIKES,
	ID_ROLLING_BALL1,
	ID_DARTS,
	ID_DART_EMITTER,
	ID_DRAW_BRIDGE,
	ID_TEETH_TRAP,
	ID_LIFT,
	ID_GENERAL,
	ID_MOVABLE_BLOCK1,
	ID_MOVABLE_BLOCK2,
	ID_MOVABLE_BLOCK3,
	ID_MOVABLE_BLOCK4,
	ID_BIG_BOWL,
	ID_WINDOW1,
	ID_WINDOW2,
	ID_WINDOW3,
	ID_WINDOW4,
	ID_PROPELLER1,
	ID_PROPELLER2,
	ID_HOOK,
	ID_FALLING_CEILING,
	ID_SPINNING_BLADE,
	ID_BLADE,
	ID_KILLER_STATUE,
	ID_ROLLING_BALL2,
	ID_ICICLE,
	ID_SPIKE_WALL,
	ID_SPRING_BOARD,
	ID_CEILING_SPIKES,
	ID_BELL,
	ID_WATER_SPRITE,
	ID_SNOW_SPRITE,
	ID_SKIDOO_LARA,
	ID_SWITCH_TYPE1,
	ID_SWITCH_TYPE2,
	ID_PROPELLER3,
	ID_PROPELLER4,
	ID_PENDULUM2,
	ID_WORKER_RESERVED1,
	ID_WORKER_RESERVED2,
	ID_LARA_SWAP,
	ID_TEXT_BOX,
	ID_ROLLING_BALL3,
	ID_DEATH_SLIDE,
	ID_SWITCH_TYPE3,
	ID_SWITCH_TYPE4,
	ID_SWITCH_TYPE5,
	ID_DOOR_TYPE1,
	ID_DOOR_TYPE2,
	ID_DOOR_TYPE3,
	ID_DOOR_TYPE4,
	ID_DOOR_TYPE5,
	ID_DOOR_TYPE6,
	ID_DOOR_TYPE7,
	ID_DOOR_TYPE8,
	ID_TRAPDOOR_TYPE1,
	ID_TRAPDOOR_TYPE2,
	ID_TRAPDOOR_TYPE3,
	ID_BRIDGE_FLAT,
	ID_BRIDGE_TILT1,
	ID_BRIDGE_TILT2,
	ID_PASSPORT_OPTION,
	ID_COMPASS_OPTION,
	ID_PHOTO_OPTION,
	ID_PLAYER1,
	ID_PLAYER2,
	ID_PLAYER3,
	ID_PLAYER4,
	ID_PLAYER5,
	ID_PLAYER6,
	ID_PLAYER7,
	ID_PLAYER8,
	ID_PLAYER9,
	ID_PLAYER10,
	ID_PASSPORT_CLOSED,
	ID_COMPASS_ITEM,
	ID_PISTOL_ITEM,
	ID_SHOTGUN_ITEM,
	ID_MAGNUM_ITEM,
	ID_UZI_ITEM,
	ID_HARPOON_ITEM,
	ID_M16_ITEM,
	ID_GRENADE_ITEM,
	ID_PISTOL_AMMO_ITEM,
	ID_SHOTGUN_AMMO_ITEM,
	ID_MAGNUM_AMMO_ITEM,
	ID_UZI_AMMO_ITEM,
	ID_HARPOON_AMMO_ITEM,
	ID_M16_AMMO_ITEM,
	ID_GRENADE_AMMO_ITEM,
	ID_SMALL_MEDIPACK_ITEM,
	ID_LARGE_MEDIPACK_ITEM,
	ID_FLARES_ITEM,
	ID_FLARE_ITEM,
	ID_DETAIL_OPTION,
	ID_SOUND_OPTION,
	ID_CONTROL_OPTION,
	ID_GAMMA_OPTION,
	ID_PISTOL_OPTION,
	ID_SHOTGUN_OPTION,
	ID_MAGNUM_OPTION,
	ID_UZI_OPTION,
	ID_HARPOON_OPTION,
	ID_M16_OPTION,
	ID_GRENADE_OPTION,
	ID_PISTOL_AMMO_OPTION,
	ID_SHOTGUN_AMMO_OPTION,
	ID_MAGNUM_AMMO_OPTION,
	ID_UZI_AMMO_OPTION,
	ID_HARPOON_AMMO_OPTION,
	ID_M16_AMMO_OPTION,
	ID_GRENADE_AMMO_OPTION,
	ID_SMALL_MEDIPACK_OPTION,
	ID_LARGE_MEDIPACK_OPTION,
	ID_FLARES_OPTION,
	ID_PUZZLE_ITEM1,
	ID_PUZZLE_ITEM2,
	ID_PUZZLE_ITEM3,
	ID_PUZZLE_ITEM4,
	ID_PUZZLE_OPTION1,
	ID_PUZZLE_OPTION2,
	ID_PUZZLE_OPTION3,
	ID_PUZZLE_OPTION4,
	ID_PUZZLE_HOLE1,
	ID_PUZZLE_HOLE2,
	ID_PUZZLE_HOLE3,
	ID_PUZZLE_HOLE4,
	ID_PUZZLE_DONE1,
	ID_PUZZLE_DONE2,
	ID_PUZZLE_DONE3,
	ID_PUZZLE_DONE4,
	ID_SECRET1,
	ID_SECRET2,
	ID_SECRET3,
	ID_KEY_ITEM1,
	ID_KEY_ITEM2,
	ID_KEY_ITEM3,
	ID_KEY_ITEM4,
	ID_KEY_OPTION1,
	ID_KEY_OPTION2,
	ID_KEY_OPTION3,
	ID_KEY_OPTION4,
	ID_KEY_HOLE1,
	ID_KEY_HOLE2,
	ID_KEY_HOLE3,
	ID_KEY_HOLE4,
	ID_PICKUP_ITEM1,
	ID_PICKUP_ITEM2,
	ID_PICKUP_OPTION1,
	ID_PICKUP_OPTION2,
	ID_SPHERE_OF_DOOM1,
	ID_SPHERE_OF_DOOM2,
	ID_SPHERE_OF_DOOM3,
	ID_ALARM_SOUND,
	ID_BIRD_TWEETER1,
	ID_DINO,
	ID_BIRD_TWEETER2,
	ID_CLOCK_CHIMES,
	ID_DRAGON_BONES1,
	ID_DRAGON_BONES2,
	ID_DRAGON_BONES3,
	ID_HOT_LIQUID,
	ID_EFFECT_RESERVED1,
	ID_MINE,
	ID_INV_BACKGROUND,
	ID_EFFECT_RESERVED2,
	ID_GONG_BONGER,
	ID_DETONATOR1,
	ID_DETONATOR2,
	ID_COPTER,
	ID_EXPLOSION,
	ID_SPLASH,
	ID_BUBBLES,
	ID_BUBBLE_EMITTER,
	ID_BLOOD,
	ID_DART_EFFECT,
	ID_FLARE_FIRE,
	ID_GLOW,
	ID_EFFECT_RESERVED3,
	ID_RICOCHET,
	ID_TWINKLE,
	ID_GUN_FLASH,
	ID_DUST,
	ID_BODY_PART,
	ID_CAMERA_TARGET,
	ID_WATERFALL,
	ID_MISSILE1,
	ID_MISSILE2,
	ID_MISSILE3,
	ID_ROCKET,
	ID_HARPOON_BOLT,
	ID_LAVA,
	ID_LAVA_EMITTER,
	ID_FLAME,
	ID_FLAME_EMITTER,
	ID_SKYBOX,
	ID_ALPHABET,
	ID_DYING_MONK,
	ID_DING_DONG,
	ID_LARA_ALARM,
	ID_MINI_COPTER,
	ID_WINSTON,
	ID_ASSAULT_DIGITS,
	ID_FINAL_LEVEL_COUNTER,
	ID_CUT_SHOTGUN,
	ID_EARTHQUAKE,
	ID_NUMBER_OBJECTS,
};

struct Entity
{
	int32_t floor_y;			// 0x0
	uint32_t _0x4,				// 0x4
			 mesh_bits;			// 0x8
	uint16_t model,				// 0xC
			 _0xe,				// 0xE
			 _0x10,				// 0x10
			 _0x12;				// 0x12
	uint16_t anim_id,			// 0x14
			 anim_frame,		// 0x16
			 room_id,			// 0x18
			 room_unk_data,		// 0x1A
			 interactive_id;	// 0x1C
	uint16_t xz_velocity,		// 0x1E
			 y_velocity,		// 0x20
			 health;			// 0x22
	char pad2[0x4];				// 0x24
	uint16_t activation_flags,	// 0x28
			 intensity;			// 0x2A
	uint16_t pad3,				// 0x2C
			 parent;			// 0x2E
	uintptr_t ai_manager;		// 0x30
	ivec3 position;				// 0x34
	small_ivec3 rotation;		// 0x40
	uint16_t flags;				// 0x46
};

struct EntitySystem
{
	void teleport_entity(Entity* entity, const ivec3& pos, const small_ivec3& rot, uint16_t room_id);
	void teleport_entity_to_room(Entity* entity, uint16_t room_id);
	void explode_entity(Entity* entity, uint32_t mesh_bits_mask = -1, uint16_t limb_explosion = 0);
	void remove_entity(uint16_t i);
	void move_entity_to_room(Entity* entity, uint16_t room_id);
	void move_entity_to_room(uint16_t entity_id, uint16_t room_id);
	void change_floor_height_on_entity(Entity* entity, int32_t height);
	void update_entity_anim(Entity* entity);

	bool setup_entity(uint16_t id);
	bool set_entity_interactive(uint16_t id);
	bool enable_enemy_ai(uint16_t id);
	bool disable_enemy_ai(uint16_t id);
	bool is_dead(Entity* entity);
	bool is_dead(uint16_t hp);

	uint16_t get_entity_free_slot();

	uint32_t get_model_table();
	uint32_t get_model(uint32_t base_offset, uint32_t f_offset);

	uint16_t get_model_offset(uint32_t i);
	uint16_t get_index_from_entity(Entity* entity);

	template <typename F>
	void for_each(const F& fn)
	{
		for (uint16_t i = 0; i < get_count(); ++i)
			if (auto ent = get_entity(i))
				fn(i, ent);
	}

	uint16_t get_count();

	uintptr_t get_table();

	uint16_t* get_model_offset_table();

	Entity* spawn_entity(uint16_t model, const ivec3& pos, const small_ivec3& rot, uint16_t room, uint16_t& id);
	Entity* get_entity(uint16_t id);
	Entity* get_entity(eEntityModelID model_id);
	Entity* get_lara();

	bool get_all_entities_by_model(eEntityModelID model_id, std::vector<Entity*>& entities);
};

inline std::unique_ptr<EntitySystem> g_entity_system;

#endif