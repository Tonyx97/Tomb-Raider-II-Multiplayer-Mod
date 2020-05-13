#pragma once

#ifndef COMMANDS_H
#define COMMANDS_H

#include <vector>
#include <string>

enum eCmdResult
{
	CMD_INVALID_PARAMS,
	CMD_FAIL,
	CMD_OK,
};

namespace commands
{
	eCmdResult change_name(const std::vector<std::string>& params);
	eCmdResult teleport(const std::vector<std::string>& params);
	eCmdResult kill(const std::vector<std::string>& params);
	eCmdResult boom(const std::vector<std::string>& params);
	eCmdResult level(const std::vector<std::string>& params);
	eCmdResult friendly_fire(const std::vector<std::string>& params);
	eCmdResult give_item(const std::vector<std::string>& params);
	eCmdResult godmode(const std::vector<std::string>& params);
	eCmdResult heal(const std::vector<std::string>& params);
	eCmdResult infinite_oxygen(const std::vector<std::string>& params);
	eCmdResult infinite_ammo(const std::vector<std::string>& params);
	eCmdResult next_level(const std::vector<std::string>& params);
	eCmdResult boat_max_velocity(const std::vector<std::string>& params);
	eCmdResult skidoo_max_velocity(const std::vector<std::string>& params);
};

#endif