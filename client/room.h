#pragma once

#ifndef ROOM_H
#define ROOM_H

#include <memory>

#include <offsets.h>

struct Room
{
	uint16_t* data,
			* doors,
			* floor,
			* light,
			* mesh;

	ivec3 position;

	int32_t min_y,
			max_y;

	int16_t size_x,
			size_y,
			ambient_1,
			ambient_2,
			light_mode,
			lights,
			meshes,
			bound_left,
			bound_right,
			bound_top,
			bound_bottom,
			bound_active,
			left,
			right,
			top,
			bottom,
			item_number,
			fx_num,
			flipped_room,
			flags;
};

struct RoomSystem
{
	Room* get_room(uint16_t i);

	uintptr_t get_table();
};

inline std::unique_ptr<RoomSystem> g_room_system;

#endif