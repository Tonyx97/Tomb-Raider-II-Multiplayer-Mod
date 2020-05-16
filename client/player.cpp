#include "player.h"

#include <Windows.h>

#include <shared/utils/utils.h>

#include <entity.h>

void PlayerSystem::save_sync_data()
{
	sync_player_exploding = false;

	buffers.clear();
	small_ivec3s.clear();
	uints.clear();
	shorts.clear();
	bools.clear();

	for (auto& [offset, size] : buffers_offsets)	buffers.insert(buffers.end(), (uint8_t*)offset, (uint8_t*)offset + size);
	for (auto& offset : small_ivec3s_offsets)		small_ivec3s.insert({ offset, *(small_ivec3*)offset });
	for (auto& offset : uints_offsets)				uints.insert({ offset, *(uint32_t*)offset });
	for (auto& offset : shorts_offsets)				shorts.insert({ offset, *(uint16_t*)offset });
	for (auto& offset : bools_offsets)				bools.insert({ offset, *(bool*)offset });
}

void PlayerSystem::restore_sync_data()
{
	auto buffer_ptr = buffers.data();

	for (auto& [offset, value] : bools)				*(bool*)offset = std::any_cast<bool>(value);
	for (auto& [offset, value] : shorts)			*(uint16_t*)offset = std::any_cast<uint16_t>(value);
	for (auto& [offset, value] : uints)				*(uint32_t*)offset = std::any_cast<uint32_t>(value);
	for (auto& [offset, value] : small_ivec3s)		*(small_ivec3*)offset = std::any_cast<small_ivec3>(value);
	for (auto& [offset, size] : buffers_offsets)
	{
		memcpy((void*)offset, (void*)buffer_ptr, size);
		buffer_ptr += size;
	}
}

Player* PlayerSystem::spawn_player(const ivec3& pos, const small_ivec3& rot, uint16_t room, uint16_t& id)
{
	// make this function return immediately, otherwise it will assign the global
	// ptr of lara to the current entity created. Then restore so we can spawn
	// ourselves again

	utils::MemProt lara_setup_patch(offsets::LARA_SETUP_FN, 1, PAGE_EXECUTE_READWRITE);

	*(uint8_t*)(offsets::LARA_SETUP_FN) = 0xC3;

	auto entity = g_entity_system->spawn_entity(0, pos, rot, room, id);

	*(uint8_t*)(offsets::LARA_SETUP_FN) = 0x66;

	return (entity ? new Player(entity) : nullptr);
}

PlayerMain* PlayerSystem::get_player_by_guid(uint64_t guid)
{
	auto it = players.find(guid);
	return (it != players.end() ? &it->second : nullptr);
}