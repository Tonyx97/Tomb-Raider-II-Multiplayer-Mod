#include "room.h"

Room* RoomSystem::get_room(uint16_t i)
{
	return (Room*)(get_table() + i * offsets::PER_ROOM_SIZE);
}

uintptr_t RoomSystem::get_table()
{
	return *(uintptr_t*)offsets::ROOM_TABLE;
}