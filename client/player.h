#pragma once

#ifndef PLAYER_H
#define PLAYER_H

#include <string>
#include <vector>
#include <unordered_map>
#include <any>
#include <memory>

#include <shared/game/math.h>

#include <shared/net/game_net_structs.h>

#include <offsets.h>

struct Entity;

struct Player
{
	Entity* handle;

	player_info info;

	Player(Entity* h) : handle(h) {}
};

struct PlayerPendingSoundInfo
{
	ivec3 position;

	uint32_t id = -1,
			 sfx;
};

struct PlayerMain
{
	std::string name;

	Player* player;

	uint16_t game_id;

	std::vector<PlayerPendingSoundInfo> pending_sounds;

	bool weapons_drawn_when_died = false;
};

struct PlayerSystem
{
private:

	struct buffer_info
	{
		uintptr_t offset;
		uint32_t size;
	};

	static constexpr buffer_info buffers_offsets[] = { { offsets::HAIR_TRANSFORMS, offsets::HAIR_TRANSFORMS_SIZE } };

	static constexpr uintptr_t small_ivec3s_offsets[]	= { offsets::HEAD_ROTATION,
															offsets::TORSO_ROTATION,
															offsets::LEFT_ARM_ROTATION,
															offsets::RIGHT_ARM_ROTATION };

	static constexpr uintptr_t uints_offsets[] = { offsets::HEAD_MODEL_ID,
												   offsets::LEFT_HAND_MODEL_ID,
												   offsets::RIGHT_HAND_MODEL_ID,
												   offsets::LEFT_HOLSTER_MODEL_ID,
												   offsets::RIGHT_HOLSTER_MODEL_ID };

	static constexpr uintptr_t shorts_offsets[] = { offsets::COMBAT_STATE,
													offsets::LARA_LEFT_ARM_COMBAT_ANIM_FRAME,
													offsets::LARA_RIGHT_ARM_COMBAT_ANIM_FRAME,
													offsets::USING_WEAPON_ID,
													offsets::EQUIPPED_BACK_WEAPON,
													offsets::CAN_RENDER_LEFT_WEAPON_MUZZLE_FLASH,
													offsets::CAN_RENDER_RIGHT_WEAPON_MUZZLE_FLASH };

	static constexpr uintptr_t bools_offsets[] = { offsets::IS_LARA_IN_WATER };

	std::vector<uint8_t> buffers;

	std::unordered_map<uintptr_t, std::any> small_ivec3s,
											uints,
											shorts,
											bools;

	std::unordered_map<uint64_t, PlayerMain> players;

	bool sync_player_exploding = false;

public:

	void save_sync_data();
	void restore_sync_data();

	void sync_player_explosion()							{ sync_player_exploding = true; }

	void add_player(uint64_t guid, PlayerMain& pm)			{ players.insert({ guid, pm }); }

	bool is_player_sync_explosion_enabled() const			{ return sync_player_exploding; }

	Player* spawn_player(const ivec3& pos, const small_ivec3& rot, uint16_t room, uint16_t& id);

	PlayerMain* get_player_by_guid(uint64_t guid);

	decltype(players)& get_players()						{ return players; };

	template <typename F>
	void for_each(const F& fn)
	{
		for (auto& [guid, pm] : players)
			if (pm.player)
				fn(guid, pm);
	}
};

inline std::unique_ptr<PlayerSystem> g_player_system;

#endif
