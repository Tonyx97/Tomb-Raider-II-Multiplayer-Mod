#pragma once

#ifndef SERVER_H
#define SERVER_H

#include <assert.h>
#include <cstdio>
#include <cstring>
#include <stdlib.h>
#include <limits>
#include <stdio.h>
#include <string.h>
#include <thread>
#include <chrono>
#include <vector>
#include <unordered_map>
#include <any>

#include "slikenet/peerinterface.h"
#include "slikenet/statistics.h"
#include "slikenet/types.h"
#include "slikenet/BitStream.h"
#include "slikenet/sleep.h"
#include "slikenet/PacketLogger.h"
#include "slikenet/Gets.h"
#include "slikenet/linux_adapter.h"
#include "slikenet/osx_adapter.h"

#include <shared/net/messages.h>
#include <shared/net/bitstream.h>

class Server : protected BitStreaming
{
private:

	struct client_info
	{
		std::string address,
					name;
	};

	std::unordered_map<std::string, std::any> settings;
	std::unordered_map<uint64_t, client_info> clients;
	std::unordered_map<uint16_t, uint64_t> streamed_entities;

	bool sync_enemies = false,
		 sync_general_entities = false,
		 sync_blocks = false,
		 sync_doors = false,
		 sync_vehicles = false;

public:

	Server();
	~Server();

	bool init();

	template <typename T>
	bool get_setting(const std::string& value_name, T& value)
	{
		auto it = settings.find(value_name);
		if (it == settings.end())
			return false;

		try									{ value = std::any_cast<T>(it->second); return true; }
		catch (const std::bad_any_cast& e)	{ return false; }
	}

	template <typename T>
	bool set_setting(const std::string& value_name, T& value)
	{
		if (get_setting(value_name, value))
			return true;

		return false;
	}


	void dispatch_packets();
};

#endif