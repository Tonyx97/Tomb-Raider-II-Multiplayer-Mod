#include "commands.h"

#include <client/client.h>

#include <shared/registry/registry.h>
#include <shared/utils/utils.h>

#include <gui.h>
#include <entity.h>
#include <room.h>
#include <player.h>
#include <game_logic.h>
#include <local_game.h>

namespace commands
{
	eCmdResult change_name(const std::vector<std::string>& params)
	{
		if (params.size() != 1)
			return CMD_INVALID_PARAMS;

		const auto& new_name = params[0],
					old_name = g_client->get_name();

		if (new_name.length() > 16 || new_name.find(' ') != std::string::npos)
			return CMD_FAIL;

		g_client->set_name(new_name);

		g_registry->set_string("nickname", new_name);

		player_change_name_info info;

		strcpy_s(info.old_name, old_name.c_str());
		strcpy_s(info.new_name, new_name.c_str());

		info.guid = g_client->get_guid();

		g_client->send_packet(ID_PLAYER_CHANGE_NAME, info);

		g_gui->add_new_message(std::string("Name changed to " + new_name).c_str());

		return CMD_OK;
	}

	eCmdResult teleport(const std::vector<std::string>& params)
	{
		const auto lara = g_entity_system->get_lara();

		switch (params.size())
		{
		case 1:
		{
			const auto& player_name_or_room_id = params[0];

			if (auto room_id = (uint16_t)std::atol(player_name_or_room_id.c_str()); room_id > 0)
			{
				g_entity_system->teleport_entity_to_room(lara, room_id);

				g_gui->send_new_message("%s teleported to room %i", g_client->get_name().c_str(), room_id);
				g_gui->add_new_message("Teleported to '%i'", room_id);

				return CMD_OK;
			}

			const auto local_guid = g_client->get_guid();

			auto& players = g_player_system->get_players();

			std::string real_player_name = "unknown";

			auto it = std::find_if(players.begin(), players.end(), [&](const std::pair<uint64_t, PlayerMain>& p)
			{
					if (p.first == local_guid)
						return false;

					real_player_name = p.second.name;
					return (real_player_name.find(player_name_or_room_id) != std::string::npos);
			});

			if (it == players.end())
			{
				g_gui->add_new_message("Player %s not found", player_name_or_room_id.c_str());
				return CMD_FAIL;
			}

			if (const auto& player = it->second.player)
			{
				if (*offvar::level_id != player->info.level)
				{
					g_gui->add_new_message("%s is in a different level", real_player_name.c_str());
					return CMD_FAIL;
				}

				const auto player_handle = player->handle;

				g_entity_system->teleport_entity(lara, player_handle->position, small_ivec3(0, player_handle->rotation.y, 0), player_handle->room_id);

				g_gui->send_new_message("%s teleported to %s", g_client->get_name().c_str(), real_player_name.c_str());
				g_gui->add_new_message("Teleported to %s", real_player_name.c_str());

				return CMD_OK;
			}

			g_gui->add_new_message("Player %s has no valid handle", real_player_name.c_str());

			return CMD_FAIL;
		}
		case 3:
		{
			auto x = std::atol(params[0].c_str()),
				 y = std::atol(params[1].c_str()),
				 z = std::atol(params[2].c_str());

			g_entity_system->teleport_entity(lara, ivec3(x, y, z), lara->rotation, lara->room_id);

			g_gui->send_new_message("%s teleported to (%i, %i, %i)", g_client->get_name().c_str(), x, y, z);
			g_gui->add_new_message("Teleported");

			return CMD_OK;
		}
		case 4:
		{
			int32_t x = std::atol(params[0].c_str()),
					y = std::atol(params[1].c_str()),
					z = std::atol(params[2].c_str());

			auto room_id = (uint16_t)std::atol(params[3].c_str());

			g_entity_system->teleport_entity(lara, ivec3(x, y, z), lara->rotation, room_id);

			g_gui->send_new_message("%s teleported to (%i, %i, %i, [%i])", g_client->get_name().c_str(), x, y, z, room_id);
			g_gui->add_new_message("Teleported to '%i'", room_id);

			return CMD_OK;
		}
		}

		return CMD_INVALID_PARAMS;
	}

	eCmdResult kill(const std::vector<std::string>& params)
	{
		g_entity_system->get_lara()->health = 0;

		g_gui->send_new_message("%s killed themselves", g_client->get_name().c_str());
		g_gui->add_new_message("You killed yourself");

		return CMD_OK;
	}

	eCmdResult boom(const std::vector<std::string>& params)
	{
		g_entity_system->explode_entity(g_entity_system->get_lara(), -1, 1);

		g_player_system->sync_player_explosion();

		g_gui->send_new_message("%s exploded", g_client->get_name().c_str());
		g_gui->add_new_message("You exploded");

		return CMD_OK;
	}

	eCmdResult level(const std::vector<std::string>& params)
	{
		if (params.size() != 1)
			return CMD_INVALID_PARAMS;

		auto level_name = params[0];

		std::transform(level_name.begin(), level_name.end(), level_name.begin(), std::tolower);

		auto tp = [&](uint8_t lvl_id)
		{
			g_local_game->change_level(lvl_id);

			g_gui->send_new_message("%s moved to level '%s'", g_client->get_name().c_str(), level_name.c_str());
			g_gui->add_new_message("You moved to level '%s'", level_name.c_str());
		};

		const auto level_id = (uint8_t)std::atol(level_name.c_str());

		if (level_id == 0)
		{
			for (auto&& [name, id] : g_levels)
			{
				if (name.find(level_name) != std::string::npos)
				{
					level_name = name;
					tp(id);
					return CMD_OK;
				}
			}

			g_gui->add_new_message("Can't find level '%s'", level_name.c_str());

			return CMD_FAIL;
		}

		if (level_id < 0 || level_id > 18)
		{
			g_gui->add_new_message("Invalid level id %i", level_id);
			return CMD_FAIL;
		}

		tp(level_id);

		return CMD_OK;
	}

	eCmdResult friendly_fire(const std::vector<std::string>& params)
	{
		if (params.size() != 1)
			return CMD_INVALID_PARAMS;

		bool enabled = g_gui->get_bool_from_string(params[0]);

		g_local_game->set_friendly_fire(enabled);

		const char* enabled_text = (enabled ? "enabled" : "disabled");

		g_gui->send_new_message("%s %s FF", g_client->get_name().c_str(), enabled_text);
		g_gui->add_new_message("Friendly-fire %s", enabled_text);

		return CMD_OK;
	}

	eCmdResult give_item(const std::vector<std::string>& params)
	{
		if (params.size() != 1 && params.size() != 2)
			return CMD_INVALID_PARAMS;

		const auto& item_name = params[0];

		const auto item_amount = (params.size() == 2 ? std::atol(params[1].c_str()) : 1);

		std::string item_real_name;

		if (!g_local_game->give_item(item_name, item_amount, &item_real_name))
		{
			g_gui->add_new_message("Item '%s' does not exist", item_name.c_str());
			return CMD_FAIL;
		}

		g_gui->send_new_message("Item '%s' was given to '%s'", item_real_name.c_str(), g_client->get_name().c_str());
		g_gui->add_new_message("You received '%s'", item_real_name.c_str());

		return CMD_OK;
	}

	eCmdResult godmode(const std::vector<std::string>& params)
	{
		if (params.size() != 1)
			return CMD_INVALID_PARAMS;

		bool enabled = g_gui->get_bool_from_string(params[0]);

		g_local_game->set_god_mode(enabled);

		const char* enabled_text = (enabled ? "enabled" : "disabled");

		g_gui->send_new_message("%s %s god-mode", g_client->get_name().c_str(), enabled_text);
		g_gui->add_new_message("God-mode %s", enabled_text);

		return CMD_OK;
	}

	eCmdResult heal(const std::vector<std::string>& params)
	{
		g_entity_system->get_lara()->health = 1000;

		return CMD_OK;
	}

	eCmdResult infinite_oxygen(const std::vector<std::string>& params)
	{
		if (params.size() != 1)
			return CMD_INVALID_PARAMS;

		bool enabled = g_gui->get_bool_from_string(params[0]);

		g_local_game->set_infinite_oxygen(enabled);

		const char* enabled_text = (enabled ? "enabled" : "disabled");

		g_gui->send_new_message("%s %s infinite oxygen", g_client->get_name().c_str(), enabled_text);
		g_gui->add_new_message("Infinite oxygen %s", enabled_text);

		return CMD_OK;
	}

	eCmdResult infinite_ammo(const std::vector<std::string>& params)
	{
		if (params.size() != 1)
			return CMD_INVALID_PARAMS;

		bool enabled = g_gui->get_bool_from_string(params[0]);

		*offvar::infinite_ammo = enabled;

		const char* enabled_text = (enabled ? "enabled" : "disabled");

		g_gui->send_new_message("%s %s infinite ammo", g_client->get_name().c_str(), enabled_text);
		g_gui->add_new_message("Infinite ammo %s", enabled_text);

		return CMD_OK;
	}

	eCmdResult next_level(const std::vector<std::string>& params)
	{
		*offvar::level_completed = 1;

		g_gui->send_new_message("%s moved to next level", g_client->get_name().c_str());

		return CMD_OK;
	}

	eCmdResult boat_max_velocity(const std::vector<std::string>& params)
	{
		if (params.size() != 1)
		{
			g_gui->add_new_message("Boat max velocity: %i", *offvar::boat_set_max_velocity);
			return CMD_OK;
		}

		auto max_vel = std::atol(params[0].c_str());

		utils::MemProt::execute_under_prot(offsets::BOAT_SET_MAX_VELOCITY, 4, PAGE_EXECUTE_READWRITE, [&]()
		{
			*offvar::boat_set_max_velocity = max_vel;
		});

		g_gui->send_new_message("%s changed boat max velocity to %i", g_client->get_name().c_str(), max_vel);
		g_gui->add_new_message("Boat max velocity: %i", max_vel);

		return CMD_OK;
	}

	eCmdResult skidoo_max_velocity(const std::vector<std::string>& params)
	{
		if (params.size() != 1)
		{
			g_gui->add_new_message("Skidoo max velocity: %i", *offvar::skidoo_set_max_velocity);
			return CMD_OK;
		}

		auto max_vel = std::atol(params[0].c_str());

		utils::MemProt::execute_under_prot(offsets::SKIDOO_SET_MAX_VELOCITY, 4, PAGE_EXECUTE_READWRITE, [&]()
		{
			*offvar::skidoo_set_max_velocity = max_vel;
		});

		g_gui->send_new_message("%s changed skidoo max velocity to %i", g_client->get_name().c_str(), max_vel);
		g_gui->add_new_message("Skidoo max velocity: %i", max_vel);

		return CMD_OK;
	}
};