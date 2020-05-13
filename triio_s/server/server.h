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

	std::unordered_map<uint64_t, client_info> clients;
	std::unordered_map<uint16_t, uint64_t> streamed_entities;

public:

	Server();
	~Server();

	bool init();

	void dispatch_packets();
};

#endif