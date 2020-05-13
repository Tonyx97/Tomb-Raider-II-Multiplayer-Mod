#include <intrin.h>

#include "entity.h"

#include <shared/net/game_net_structs.h>

#include <console/console.h>

#include <client/client.h>

#include <utils.h>
#include <keyboard.h>
#include <room.h>
#include <player.h>
#include <gui.h>
#include <game_logic.h>
#include <local_game.h>

void EntitySystem::teleport_entity(Entity* entity, const ivec3& pos, const small_ivec3& rot, uint16_t room_id)
{
	entity->position = pos;
	entity->rotation = rot;

	move_entity_to_room(get_index_from_entity(entity), room_id);
}

void EntitySystem::teleport_entity_to_room(Entity* entity, uint16_t room_id)
{
	if (auto room = g_room_system->get_room(room_id))
	{
		auto fpos = room->position;

		fpos.y = room->min_y;
		fpos.x += room->bound_left;
		fpos.z += room->bound_left;

		bool found_correct_spot = false;

		for (auto x = 0; x < room->size_x && !found_correct_spot; fpos.x += 1000, ++x)
			for (auto z = 0; z < room->size_y && !found_correct_spot; fpos.z += 1000, ++z)
				if (auto floor = g_local_game->get_floor(fpos, room_id))
					if (auto height = g_local_game->get_height(floor, fpos);
							 height != -32512)
					{
						entity->position = ivec3(fpos.x, height, fpos.z);
						found_correct_spot = true;
					}

		if (found_correct_spot)
			move_entity_to_room(get_index_from_entity(entity), room_id);
	}
}

void EntitySystem::explode_entity(Entity* entity, uint32_t mesh_bits_mask, uint16_t limb_dist_explosion)
{
	reinterpret_cast<bool(*)(uint16_t, uint32_t, uint16_t)>(offsets::EXPLODE_ENTITY)(get_index_from_entity(entity), mesh_bits_mask, limb_dist_explosion);

	entity->health = 0;
	entity->activation_flags |= (3 << 8);
}

void EntitySystem::remove_entity(uint16_t id)
{
	reinterpret_cast<int32_t(*)(uint16_t)>(offsets::REMOVE_ENTITY)(id);
}

void EntitySystem::move_entity_to_room(Entity* entity, uint16_t room_id)
{
	move_entity_to_room(get_index_from_entity(entity), room_id);
}

void EntitySystem::move_entity_to_room(uint16_t entity_id, uint16_t room_id)
{
	reinterpret_cast<int32_t(*)(uint16_t, uint16_t)>(offsets::MOVE_ENTITY_TO_ROOM)(entity_id, room_id);
}

void EntitySystem::change_floor_height_on_entity(Entity* entity, int32_t height)
{
	reinterpret_cast<uint16_t(*)(Entity*, int32_t)>(offsets::CHANGE_FLOOR_HEIGHT)(entity, height);
}

void EntitySystem::update_entity_anim(Entity* entity)
{
	reinterpret_cast<int32_t(*)(Entity*)>(offsets::UPDATE_ENTITY_ANIM)(entity);
}

bool EntitySystem::setup_entity(uint16_t id)
{
	return !!reinterpret_cast<int32_t(*)(uint16_t)>(offsets::SETUP_ENTITY)(id);
}

bool EntitySystem::set_entity_interactive(uint16_t id)
{
	return g_game->set_entity_active(id);
}

bool EntitySystem::enable_enemy_ai(uint16_t id)
{
	return g_game->enable_enemy_ai(id, false);
}

bool EntitySystem::disable_enemy_ai(uint16_t id)
{
	return !!reinterpret_cast<int32_t(*)(uint16_t)>(offsets::DISABLE_ENEMY_AI)(id);
}

bool EntitySystem::is_dead(Entity* entity)
{
	return (entity->health == 0 || entity->health == -1);
}

bool EntitySystem::is_dead(uint16_t hp)
{
	return (hp == 0 || hp == UINT16_MAX);
}

uint16_t EntitySystem::get_entity_free_slot()
{
	return reinterpret_cast<uint16_t(*)()>(offsets::GET_FREE_ENTITY_SLOT)();
}

uint32_t EntitySystem::get_model_table()
{
	return *(uint32_t*)offsets::MODEL_ID_TABLE;
}

uint32_t EntitySystem::get_model(uint32_t base_offset, uint32_t f_offset)
{
	return *(uint32_t*)(get_model_table() + 4 * base_offset + f_offset);
}

uint16_t EntitySystem::get_model_offset(uint32_t i)
{
	return get_model_offset_table()[i];
}

uint16_t EntitySystem::get_index_from_entity(Entity* entity)
{
	for (uint16_t i = 0; i < get_count(); ++i)
		if (get_entity(i) == entity)
			return i;
	return -1;
}

uint16_t EntitySystem::get_count()
{
	return *(uint16_t*)offsets::ENTITIES_COUNT;
}

uintptr_t EntitySystem::get_table()
{
	return *(uintptr_t*)offsets::ENTITY_TABLE;
}

uint16_t* EntitySystem::get_model_offset_table()
{
	return (uint16_t*)offsets::MODEL_OFFSET_TABLE;
}

Entity* EntitySystem::spawn_entity(uint16_t model, const ivec3& pos, const small_ivec3& rot, uint16_t room, uint16_t& id)
{
	if ((id = get_entity_free_slot()) != -1)
	{
		const auto entity = (Entity*)(get_table() + offsets::PER_ENTITY_SIZE * id);

		entity->model = model;
		entity->position = pos;
		entity->rotation = rot;
		entity->room_id = room;

		setup_entity(id);

		entity->activation_flags = 256;
		entity->intensity = -1;
		entity->pad3 = 4096;
		entity->parent = 223;
		entity->flags &= 65534u;

		set_entity_interactive(id);

		println(color::cyan, "\n************");
		println(color::cyan, "Entity 0x%x with ID %i created successfully (model: %i)", entity, id, model);
		println(color::cyan, "Position: (%i %i %i)", pos.x, pos.y, pos.z);
		println(color::cyan, "************\n");

		return entity;
	}

	return nullptr;
}

Entity* EntitySystem::get_entity(uint16_t i)
{
	if (i >= 0 && i < 256)
		return (Entity*)(get_table() + (i * offsets::PER_ENTITY_SIZE));
	return nullptr;
}

Entity* EntitySystem::get_entity(eEntityModelID model_id)
{
	for (auto i = 0; i < 256; ++i)
		if (auto entity = (Entity*)(get_table() + (i * offsets::PER_ENTITY_SIZE)); entity->model == model_id)
			return entity;
	return nullptr;
}

Entity* EntitySystem::get_lara()
{
	return *(Entity**)offsets::LARA;
}

bool EntitySystem::get_all_entities_by_model(eEntityModelID model_id, std::vector<Entity*>& entities)
{
	for (auto i = 0; i < 256; ++i)
		if (auto entity = (Entity*)(get_table() + (i * offsets::PER_ENTITY_SIZE)); entity->model == model_id)
			entities.push_back(entity);

	return !entities.empty();
}