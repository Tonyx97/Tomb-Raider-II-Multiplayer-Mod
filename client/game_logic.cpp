#include "game_logic.h"

#include <intrin.h>

#include <mhwrapper.h>

#include <vehhook.h>

#include <shared/console/console.h>
#include <shared/debug/debug.h>
#include <shared/utils/utils.h>

#include <client/client.h>

#include <local_game.h>
#include <net_game_logic.h>
#include <audio.h>
#include <room.h>
#include <gui.h>
#include <keyboard.h>
#include <entity.h>
#include <player.h>

bool GameLogic::init()
{
	// hook the game controls and keys updating so we can lock controls to chat
	// and handle network packets even if we are in the main menu or inventory
	// (among other game logic things)

	if (!g_mh->hook_fn(hk_update_input, offsets::UPDATE_INPUT, &update_input))
		return dbg::mod_error(L"UPDATE_INPUT hook failed");

	// TODO

	if (!g_mh->hook_fn(hk_render_game, offsets::RENDER_GAME, &render_game))
		return dbg::mod_error(L"RENDER_GAME hook failed");

	// hook to handle multiple laras

	if (!g_mh->hook_fn(hk_render_lara, offsets::RENDER_LARA, &render_lara))
		return dbg::mod_error(L"RENDER_LARA hook failed");

	// hook to properly handle players sync when spawning/loading level

	if (!g_mh->hook_fn(hk_load_level, offsets::LOAD_LEVEL, &load_level))
		return dbg::mod_error(L"LOAD_LEVEL hook failed");

	// hook to sync enemies spawning

	if (!g_mh->hook_fn(hk_enable_enemy_ai, offsets::ENABLE_ENEMY_AI, &enable_enemy_ai))
		return dbg::mod_error(L"ENABLE_ENEMY_AI hook failed");

	// hook to sync enemies

	if (!g_mh->hook_fn(hk_update_enemy_logic, offsets::UPDATE_ENEMY_LOGIC, &update_enemy_logic))
		return dbg::mod_error(L"UPDATE_ENEMY_LOGIC hook failed");

	// hook to sync damage to entities (players & other entities)

	if (!g_mh->hook_fn(hk_damage_enemy, offsets::DAMAGE_ENEMY, &damage_enemy))
		return dbg::mod_error(L"DAMAGE_ENEMY hook failed");

	// hook to sync lighting (shooting, explosions etc)

	if (!g_mh->hook_fn(hk_render_light, offsets::RENDER_LIGHT, &render_light))
		return dbg::mod_error(L"RENDER_LIGHT hook failed");

	// hook to sync audios

	if (!g_mh->hook_fn(hk_play_audio, offsets::PLAY_AUDIO, &play_audio))
		return dbg::mod_error(L"PLAY_AUDIO hook failed");

	// hook to sync stopping audios

	if (!g_mh->hook_fn(hk_stop_audio, offsets::STOP_AUDIO, &stop_audio))
		return dbg::mod_error(L"STOP_AUDIO hook failed");

	// hook to sync water splashing

	if (!g_mh->hook_fn(hk_create_water_splash, offsets::CREATE_WATER_SPLASH, &create_water_splash))
		return dbg::mod_error(L"CREATE_WATER_SPLASH hook failed");

	// hook to sync entities triggering such as traps, doors etc

	if (!g_mh->hook_fn(hk_set_entity_active, offsets::SET_ENTITY_ACTIVE, &set_entity_active))
		return dbg::mod_error(L"SET_ENTITY_ACTIVE hook failed");

	// hook to sync the glass windows destroying/explosions

	if (!g_mh->hook_fn(hk_destroy_glass_window, offsets::DESTROY_GLASS_WINDOW, &destroy_glass_window))
		return dbg::mod_error(L"DESTROY_GLASS_WINDOW hook failed");

	// hook to sync room flipping (water, explosions or disasters changing rooms)

	if (!g_mh->hook_fn(hk_flip_room, offsets::FLIP_ROOM, &flip_room))
		return dbg::mod_error(L"FLIP_ROOM hook failed");

	// hook to sync boat water splash

	if (!g_mh->hook_fn(hk_create_boat_water_splash, offsets::CREATE_BOAT_WATER_SPLASH, &create_boat_water_splash))
		return dbg::mod_error(L"CREATE_BOAT_WATER_SPLASH hook failed");

	// hook to fully sync doors etc

	if (!g_veh->add_hook(offsets::SET_ACTIVATION_FLAGS_MF, mid_hk_set_activation_flags_asm))
		return dbg::mod_error(L"ASSIGN_NEW_ACTIVATION_FLAGS hook failed");
	
	// - patch lara flag bit testing to checks whether she is invisible or not
	// - we do not need to restore this bytes, as long as the mod is running
	//   it is fine to have this patch enabled

	utils::MemProt::execute_under_prot(offsets::INVISIBLE_LARA_CHECK, offsets::INVISIBLE_LARA_CHECK_SIZE, PAGE_EXECUTE_READWRITE,
	[]()
	{
		memset((void*)offsets::INVISIBLE_LARA_CHECK, 0x90, offsets::INVISIBLE_LARA_CHECK_SIZE);
	});

	// - patch the dynamic lights count to sync them properly manually resetting the count
	//   in the input hook where we dispatch network packets
	// - we do not need to restore this bytes, as long as the mod is running
	//   it is fine to have this patch enabled

	utils::MemProt::execute_under_prot(offsets::SET_DYNAMIC_LIGHTS_TO_ZERO, offsets::SET_DYNAMIC_LIGHTS_TO_ZERO_SIZE, PAGE_EXECUTE_READWRITE,
	[]()
	{
		memset((void*)offsets::SET_DYNAMIC_LIGHTS_TO_ZERO, 0x90, offsets::SET_DYNAMIC_LIGHTS_TO_ZERO_SIZE);
	});

	return true;
}

bool GameLogic::destroy()
{
	// unhook all VEH hooks

	g_veh->remove_hook(offsets::SET_ACTIVATION_FLAGS_MF);

	// unhook all functions
	
	g_mh->unhook_fn(offsets::CREATE_BOAT_WATER_SPLASH);
	g_mh->unhook_fn(offsets::FLIP_ROOM);
	g_mh->unhook_fn(offsets::DESTROY_GLASS_WINDOW);
	g_mh->unhook_fn(offsets::SET_ENTITY_ACTIVE);
	g_mh->unhook_fn(offsets::CREATE_WATER_SPLASH);
	g_mh->unhook_fn(offsets::STOP_AUDIO);
	g_mh->unhook_fn(offsets::PLAY_AUDIO);
	g_mh->unhook_fn(offsets::RENDER_LIGHT);
	g_mh->unhook_fn(offsets::DAMAGE_ENEMY);
	g_mh->unhook_fn(offsets::UPDATE_ENEMY_LOGIC);
	g_mh->unhook_fn(offsets::ENABLE_ENEMY_AI);
	g_mh->unhook_fn(offsets::LOAD_LEVEL);
	g_mh->unhook_fn(offsets::RENDER_LARA);
	g_mh->unhook_fn(offsets::RENDER_GAME);
	g_mh->unhook_fn(offsets::UPDATE_INPUT);

	return true;
}

int32_t GameLogic::hk_update_input()
{
	static bool can_update = true;

	// - cancel game input while chatting
	// - avoid opening the inventory when we press escape
	//   to cancel the current chat message

	if (g_gui->is_typing())
		can_update = false;
	else if (g_gui->is_checking_for_reset())
		if (can_update = (!g_kb->is_key_pressed_w(VK_ESCAPE) && !g_kb->is_key_pressed_w(VK_RETURN)))
			g_gui->reset_check();

	// manually reset the number of dynamic lights
	// so we can sync lights properly

	offvar::dynamic_lights_count = 0;

	// receive all network packets

	g_client->dispatch_packets();

	// render chat

	g_gui->render_chat();

	// display initialized message

	if (offvar::game_state == 1)
	{
		static std::once_flag flag;

		std::call_once(flag, []() { g_gui->add_new_message("TRIIO initialized"); });
	}

	return (can_update ? update_input() : 0);
}

void* GameLogic::hk_load_level(int32_t a1, int32_t a2)
{
	// clear whole chat

	g_gui->clear_chatting_state();
	g_gui->clear_chat();

	// destroy this player character for all players

	g_client->send_packet(ID_PLAYER_DESTROY, g_client->get_guid());

	// despawn players when we load/unload a level to do a proper cleanup

	for (auto& [guid, pm] : g_player_system->get_players())
	{
		if (!pm.player)
			continue;

		g_entity_system->remove_entity(pm.game_id);

		delete pm.player;
		pm.player = nullptr;
	}

	const auto res = load_level(a1, a2);

	// set the combat animation base to avoid the weird arms transform
	// while drawing or using a weapon after loading a level

	g_local_game->set_default_combat_anim();

	return res;
}

int32_t GameLogic::hk_render_game()
{
	return render_game();
}

int32_t GameLogic::hk_render_lara(Entity* local_lara)
{
	if (!g_client->is_connected())
		return render_lara(local_lara);

	// setup the general localplayer data

	static uint16_t last_vehicle_id = -1;

	player_info localplayer_info = {
										g_client->get_guid(),
										{},
										local_lara->position,
										local_lara->rotation,
										*offvar::head_rotation,
										*offvar::torso_rotation,
										*offvar::left_arm_rotation,
										*offvar::right_arm_rotation,
										*offvar::left_hand_model_id,
										*offvar::right_hand_model_id,
										*offvar::key_controls,
										local_lara->mesh_bits,
										local_lara->floor_y,
										local_lara->anim_id,
										local_lara->anim_frame,
										local_lara->room_id,
										local_lara->health,
										local_lara->activation_flags,
										*offvar::combat_state,
										*offvar::left_arm_combat_anim_frame,
										*offvar::right_arm_combat_anim_frame,
										*offvar::using_weapon_id,
										*offvar::equipped_back_weapon,
										*offvar::can_render_left_weapon_muzzle_flash,
										*offvar::can_render_right_weapon_muzzle_flash,
										*offvar::head_firing_model_id_index,
										*offvar::lara_current_vehicle,
										*offvar::level_id,
										*offvar::movement_state == 1 || *offvar::movement_state == 2,
										g_player_system->is_player_sync_explosion_enabled(),
								   };

	/*if (g_kb->is_key_pressed_w(VK_NUMPAD1))
	{
		eEntityModelID vehicle_type = eEntityModelID(*offvar::level_id == g_levels["tibetanfoothills"] ?
													 eEntityModelID::ID_SKIDOO_FAST : eEntityModelID::ID_BOAT);

		g_entity_system->for_each([&](uint16_t index, Entity* ent)
		{
			if (ent->model == vehicle_type)
				println(color::red, "vehicle %i 0x%x", index, ent);
		});
	}

	if (g_kb->is_key_pressed_w(VK_NUMPAD2))
	{
		println(color::red, "current room 0x%x", g_room_system->get_room(local_lara->room_id));
	}*/

	auto current_veh_id = *offvar::lara_current_vehicle;

	if (last_vehicle_id != current_veh_id)
	{
		// tell the server we left the vehicle

		if (current_veh_id == MAXUINT16)
			g_client->send_packet<vehicle_info>(ID_VEHICLE_INFO, { ivec3 {}, small_ivec3 {}, last_vehicle_id, 0, 0,	0, 0 });

		last_vehicle_id = current_veh_id;
	}

	// send current vehicle info

	if (current_veh_id != MAXUINT16)
	{
		std::vector<Entity*> boats;

		eEntityModelID vehicle_type = eEntityModelID(*offvar::level_id == g_levels["tibetanfoothills"] ?
													 eEntityModelID::ID_SKIDOO_FAST : eEntityModelID::ID_BOAT);

		if (g_entity_system->get_all_entities_by_model(vehicle_type, boats))
		{
			for (auto& boat : boats)
			{
				g_client->send_packet<vehicle_info>(ID_VEHICLE_INFO,
													{
														boat->position,
														boat->rotation,
														g_entity_system->get_index_from_entity(boat),
														boat->anim_id,
														boat->anim_frame,
														boat->room_id,
														boat->xz_velocity,
														boat->y_velocity,
														*offvar::level_id
													});
			}
		}
	}

	// copy all hair transforms to send them

	memcpy(localplayer_info.hair_transforms, (void*)offsets::HAIR_TRANSFORMS, offsets::HAIR_TRANSFORMS_SIZE);

	// send general localplayer data and hair transforms

	g_client->send_packet<player_info>(ID_PLAYER_INFO, localplayer_info);

	// update some localplayer logic

	if (g_local_game->is_godmode_enabled())
		local_lara->health = 30000;

	if (g_local_game->is_infinite_oxygen_enabled())
		*offvar::oxygen = 1800;

	// save our lara game memory data to restore after rendering other players

	g_player_system->save_sync_data();

	// render other players laras using each player variables
	// only loop players who spawned in the local level

	g_player_system->for_each([&](uint64_t guid, PlayerMain& pm)
	{
		const auto player = pm.player;
		const auto game_id = pm.game_id;
		const auto player_entity = player->handle;

		const auto& player_info = player->info;

		// update mesh bits & check if player exploded

		player_entity->mesh_bits = player_info.mesh_bits;
		player_entity->activation_flags = player_info.activation_flags;

		if (player_info.activation_flags & 0x300)
			return;

		// check if player is invisible
		// we can even create our own flags since the game only checks bit 0 in 0x29 (byte) field of lara entity

		if (player_info.activation_flags & 0x100)
			return;

		const auto current_weapon = player_info.using_weapon_id;
		const auto key_controls = player_info.key_controls;
		const bool changed_room = (player_entity->room_id != player_info.room_id),
				   is_holding_fire_key = (key_controls & 0x40);

		player_entity->floor_y = player_info.floor_y;
		player_entity->anim_id = player_info.anim_id;
		player_entity->anim_frame = player_info.anim_frame;
		player_entity->health = player_info.health;

		if (changed_room)
		{
			player_entity->position = player_info.position;

			g_entity_system->move_entity_to_room(game_id, player_info.room_id);
		}
		else player_entity->position.interpolate_to(player_info.position, 0.8f);

		player_entity->rotation = player_info.rotation;

		// use global_access capabilities to directly access game variables

		offvar::head_rotation = player_info.head_rot;
		offvar::torso_rotation = player_info.torso_rot;
		offvar::left_arm_rotation = player_info.left_arm_rot;
		offvar::right_arm_rotation = player_info.right_arm_rot;

		offvar::combat_state = player_info.combat_state;
		offvar::using_weapon_id = player_info.using_weapon_id;
		offvar::equipped_back_weapon = player_info.equipped_back_weapon;
		offvar::can_render_left_weapon_muzzle_flash = player_info.can_render_left_weapon_muzzle_flash;
		offvar::can_render_right_weapon_muzzle_flash = player_info.can_render_right_weapon_muzzle_flash;

		offvar::is_lara_in_water = player_info.is_in_water;

		// update arms animation (only for duals for now)

		switch (current_weapon)
		{
		case 1:
		case 2:
		case 3:
			offvar::left_arm_combat_anim_frame = player_info.left_arm_combat_anim_frame;
			offvar::right_arm_combat_anim_frame = player_info.right_arm_combat_anim_frame;
			break;
		case 4:
		case 5:
		case 6:
		case 7: // TODO
			break;
		}

		// get hands model id index from the current weapon (only duals for now)

		int32_t hands_model_id_index = 0;

		switch (player_info.using_weapon_id)
		{
		case 1:
		case 2:
		case 3:
			hands_model_id_index = 0x18 * g_local_game->get_weapon_id_from_slot(player_info.using_weapon_id);
			break;
		case 4:
		case 5:
		case 6:
		case 7: // TODO
			break;
		}

		// set the hand and holster models

		auto render_dual_weapons = [&](bool left, bool right)
		{
			if (left)	offvar::left_hand_model_id = g_entity_system->get_model(g_entity_system->get_model_offset(hands_model_id_index), 0x34);
			else		offvar::left_hand_model_id = g_entity_system->get_model(g_entity_system->get_model_offset(0), 0x34);
			if (right)	offvar::right_hand_model_id = g_entity_system->get_model(g_entity_system->get_model_offset(hands_model_id_index), 0x28);
			else		offvar::right_hand_model_id = g_entity_system->get_model(g_entity_system->get_model_offset(0), 0x28);
		};

		auto render_weapons_in_holsters = [&](bool left, bool right)
		{
			if (left)	offvar::left_holster_model_id = g_entity_system->get_model(g_entity_system->get_model_offset(hands_model_id_index), 0x4);
			else		offvar::left_holster_model_id = g_entity_system->get_model(g_entity_system->get_model_offset(0), 0x4);
			if (right)	offvar::right_holster_model_id = g_entity_system->get_model(g_entity_system->get_model_offset(hands_model_id_index), 0x10);
			else		offvar::right_holster_model_id = g_entity_system->get_model(g_entity_system->get_model_offset(0), 0x10);
		};

		switch (player_info.combat_state)
		{
		case 0: // weapons holstered / player died with weapons in hands
			if (!pm.weapons_drawn_when_died)
			{
				render_dual_weapons(false, false);
				render_weapons_in_holsters(true, true);

				break;
			}
			
			[[fallthrough]];

		case 4: // weapons drawn
			render_dual_weapons(true, true);
			render_weapons_in_holsters(false, false);
			break;
		case 2: // drawing/holstering weapons
		case 3:
		{
			const bool can_render_left_weapon = player_info.left_arm_combat_anim_frame >= 13,
					   can_render_right_weapon = player_info.right_arm_combat_anim_frame >= 13;

			render_dual_weapons(can_render_left_weapon, can_render_right_weapon);
			render_weapons_in_holsters(!can_render_left_weapon, !can_render_right_weapon);

			break;
		}
		}

		// change head model id while "shooting"

		offvar::head_model_id = (player_info.combat_state == 4 && is_holding_fire_key)
								 ? g_entity_system->get_model(player_info.head_firing_model_id_index, 0x38)
								 : g_entity_system->get_model(g_entity_system->get_model_offset(0), 0x38);

		for (auto&& pending_sound : pm.pending_sounds)
			play_audio((eAudioID)pending_sound.id, &pending_sound.position, pending_sound.sfx);

		if (!is_holding_fire_key)
			pm.pending_sounds.clear();

		// set the hair transforms for this player

		memcpy((void*)offsets::HAIR_TRANSFORMS, player_info.hair_transforms, offsets::HAIR_TRANSFORMS_SIZE);

		// render the lara of this player

		render_lara(player_entity);
	});

	// restore game memory to render our lara and render her

	g_player_system->restore_sync_data();

	// render our player if is visible and did not explode

	return !(local_lara->activation_flags & 0x100) ? render_lara(local_lara) : 0;
}

int32_t GameLogic::hk_enable_enemy_ai(uint16_t id, int32_t unk)
{
	g_client->send_packet<enable_enemy_ai_info>(ID_ENABLE_ENEMY_AI, { g_client->get_guid(), id, *offvar::level_id });

	return enable_enemy_ai(id, unk);
}

uint32_t GameLogic::hk_update_enemy_logic(uint16_t id, uint16_t rot_unk, uint32_t unk)
{
	if (auto entity = g_entity_system->get_entity(id))
	{
		g_client->send_packet<enemy_info>(ID_ENEMY_INFO,
										{
											entity->position,
											entity->rotation,
											id,
											entity->anim_id,
											entity->anim_frame,
											entity->room_id,
											entity->health,
											*offvar::level_id
										});
	}

	return update_enemy_logic(id, rot_unk, unk);
}

uint16_t GameLogic::hk_damage_enemy(Entity* entity, ivec3* impact_pos, uint16_t damage)
{
	if (entity->model != 0)
	{
		// normal enemies

		g_client->send_packet<damage_enemy_info>(ID_DAMAGE_ENEMY,
												{
													impact_pos ? *impact_pos : ivec3 {},
													g_entity_system->get_index_from_entity(entity),
													damage,
													*offvar::level_id
												});
	}
	else
	{
		// players

		auto& players = g_player_system->get_players();

		auto it = std::find_if(players.begin(), players.end(), [&](const std::pair<uint64_t, PlayerMain>& p)
		{
			return (p.second.player->handle == entity);
		});

		if (it != players.end())
		{
			g_client->send_packet<damage_player_info>(ID_DAMAGE_PLAYER,
													{
														impact_pos ? *impact_pos : ivec3 {},
														it->first,
														(uint16_t)(damage * 50),
														*offvar::level_id
													});
		}
	}

	return damage_enemy(entity, impact_pos, damage);
}

uint32_t GameLogic::hk_render_light(uint32_t x, int32_t y, uint32_t z, uint16_t unk0, int32_t unk1)
{
	g_client->send_packet<render_light_info>(ID_RENDER_LIGHT,
											{
												ivec3(x, y, z),
												unk1,
												unk0,
												*offvar::level_id
											});

	return render_light(x, y, z, unk0, unk1);
}

uint32_t GameLogic::hk_play_audio(eAudioID id, ivec3* position, uint32_t sfx)
{
	switch (id)
	{
	case eAudioID::FLARE:
	case eAudioID::UNDERWATER_SWIM_IN:
	case eAudioID::UNDERWATER_SWIM_OUT:
	case eAudioID::BREAKING_GLASS:
	case eAudioID::UNDERWATER:
	case eAudioID::WATER_FLOODING:
	case eAudioID::LAVA:
	case eAudioID::FLAME:
	case eAudioID::HELICOPTER:
	case eAudioID::BIRDS:
	case eAudioID::BIRDS_2:	return play_audio(id, position, sfx);
	}

	const auto ret_addr = (uintptr_t)_ReturnAddress();

	if (ret_addr >= offsets::IGNORE_SOUNDS_BOAT_BEGIN && ret_addr < offsets::IGNORE_SOUNDS_BOAT_END)
	{
		/*g_client->send_packet<play_audio_info>(ID_PLAY_AUDIO,
												{
													position ? *position : ivec3 {},
													g_client->get_guid(),
													(uint32_t)id,
													sfx,
													*offvar::lara_current_vehicle,
													*offvar::level_id
												});*/

		return play_audio(id, position, sfx);
	}

	if (ret_addr >= offsets::IGNORE_SOUNDS_ANIM_IN_RANGE_BEGIN && ret_addr < offsets::IGNORE_SOUNDS_ANIM_IN_RANGE_END ||
		ret_addr >= offsets::IGNORE_SOUNDS_ROLLING_SPIDLES_IN_RANGE_BEGIN && ret_addr < offsets::IGNORE_SOUNDS_ROLLING_SPIDLES_IN_RANGE_END ||
		ret_addr >= offsets::IGNORE_SOUNDS_DISK_EMITTER_IN_RANGE_BEGIN && ret_addr < offsets::IGNORE_SOUNDS_DISK_EMITTER_IN_RANGE_END ||
		ret_addr >= offsets::IGNORE_SOUNDS_DISK_IN_RANGE_BEGIN && ret_addr < offsets::IGNORE_SOUNDS_DISK_IN_RANGE_END ||
		ret_addr >= offsets::IGNORE_SOUNDS_SFX_BEGIN && ret_addr < offsets::IGNORE_SOUNDS_SFX_END ||
		ret_addr >= offsets::IGNORE_SOUNDS_SKIDOO_1_BEGIN && ret_addr < offsets::IGNORE_SOUNDS_SKIDOO_1_END ||
		ret_addr >= offsets::IGNORE_SOUNDS_SKIDOO_2_BEGIN && ret_addr < offsets::IGNORE_SOUNDS_SKIDOO_2_END ||
		ret_addr >= offsets::IGNORE_SOUNDS_INVENTORY_BEGIN && ret_addr < offsets::IGNORE_SOUNDS_INVENTORY_END ||
		ret_addr >= offsets::IGNORE_SOUNDS_ROLLING_BOULDER_BEGIN && ret_addr < offsets::IGNORE_SOUNDS_ROLLING_BOULDER_END ||
		ret_addr >= offsets::IGNORE_SOUNDS_PROPELLER_BEGIN && ret_addr < offsets::IGNORE_SOUNDS_PROPELLER_END ||
		ret_addr == offsets::IGNORE_SOUNDS_SKIDOO_EXPLODE ||
		ret_addr == offsets::IGNORE_SOUNDS_SPIKE_WALL_MOVING)
	{
		return play_audio(id, position, sfx);
	}
	
	g_client->send_packet<play_audio_info>(ID_PLAY_AUDIO,
											{
												position ? *position : ivec3 {},
												g_client->get_guid(),
												(uint32_t)id,
												sfx,
												MAXUINT16,
												*offvar::level_id
											});
	
	/*if (position)
		 println(color::yellow, "[3D] %i | (%i %i %i) %i [0x%x]", id, position->x, position->y, position->z, sfx, _ReturnAddress());
	else println(color::yellow, "[2D] %i | %i [0x%x]", id, sfx, _ReturnAddress());*/

	return play_audio(id, position, sfx);
}

uint32_t GameLogic::hk_stop_audio(eAudioID id, uint32_t unk0)
{
	g_client->send_packet<stop_audio_info>(ID_STOP_AUDIO,
											{
												(uint32_t)id,
												unk0,
												*offvar::level_id
											});

	return stop_audio(id, unk0);
}

uint32_t GameLogic::hk_create_water_splash(Entity* entity)
{
	g_client->send_packet<create_water_splash_info>(ID_CREATE_WATER_SPLASH,
													{
														g_client->get_guid(),
														*offvar::level_id
													});

	return create_water_splash(entity);
}

Entity* GameLogic::hk_set_entity_active(uint16_t index)
{
	auto ent = g_entity_system->get_entity(index);

	if (ent->model == 0)
		return set_entity_active(index);

	auto type = (uint32_t)_ReturnAddress();

	switch (type)
	{
	case offsets::GENERAL_ENTITY_TRIGGER:
	case offsets::MOVABLE_BLOCK_TRIGGER:	break;
	default:								return set_entity_active(index);
	}

	g_client->send_packet<set_entity_active_info>(ID_SET_ENTITY_ACTIVE,
												{
													type,
													index,
													ent->activation_flags,
													ent->flags,
													ent->_0xe,
													ent->_0x10,
													uint16_t((g_entity_system->get_lara()->rotation.y + 0x2000) >> 14),
													*offvar::level_id
												});

	println(color::yellow, "ID_SET_ENTITY_ACTIVE:\n- index(%i) | model(%i) | ptr(0x%x) | type(0x%x)", index, ent->model, ent, type);
	println(color::yellow, "- _0xE: %i\n", ent->_0xe);
	println(color::yellow, "- _0x10: %i\n", ent->_0x10);

	return set_entity_active(index);
}

bool GameLogic::hk_destroy_glass_window(uint16_t index)
{
	g_client->send_packet<destroy_glass_window_info>(ID_DESTROY_GLASS_WINDOW, { index, *offvar::level_id });

	return destroy_glass_window(index);
}

uint64_t GameLogic::hk_flip_room()
{
	if (uintptr_t(_ReturnAddress()) == offsets::FLIP_ROOM_TRIGGER)
		g_client->send_packet<flip_room_info>(ID_FLIP_ROOM, { *offvar::level_id });

	return flip_room();
}

int32_t GameLogic::hk_create_boat_water_splash(Entity* entity)
{
	g_client->send_packet<create_boat_water_splash_info>(ID_CREATE_BOAT_WATER_SPLASH,
														{
															g_entity_system->get_index_from_entity(entity),
															*offvar::level_id
														});

	return create_boat_water_splash(entity);
}

void __declspec(naked) GameLogic::mid_hk_set_activation_flags_asm()
{
	static constexpr uintptr_t target = offsets::SET_ACTIVATION_FLAGS_MF + 0x4;

	__asm
	{
		mov[esi + 28h], ax

		pushad

		call mid_hk_set_activation_flags

		popad

		jmp target
	}
}

void GameLogic::mid_hk_set_activation_flags()
{
	Entity* entity = nullptr;
	uint16_t entity_id = 0;
	uint16_t new_activation_flags = 0;

	__asm
	{
		mov new_activation_flags, ax
		mov entity, esi
		mov entity_id, di
	}

	if (!entity || entity_id == 0)
		return;

	auto send_activation_flags = [&]()
	{
		g_client->send_packet<set_activation_flags_info>(ID_SET_ACTIVATION_FLAGS,
														{
															entity_id,
															entity->activation_flags,
															entity->flags,
															*offvar::level_id
														});

		println(color::red, "[0x%x | %i | 0x%x | 0x%x]", entity, entity->model, entity->activation_flags, entity->flags);
	};

	if (entity->model >= eEntityModelID::ID_DOOR_TYPE1 && entity->model <= eEntityModelID::ID_BRIDGE_TILT2)
		return send_activation_flags();
}