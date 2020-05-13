#pragma once

#ifndef CLIENT_H
#define CLIENT_H

#include <shared/net/bitstream.h>

class Client : public BitStreaming
{
private:

	std::string name = "noname";

	uint64_t guid;

	std::atomic_bool connected = false;

public:

	Client();
	~Client();

	bool init();
	bool connect(const char* ip, uint16_t port);

	void dispatch_packets();
	void set_name(const std::string& val)	{ name = val; }

	bool is_connected() const				{ return connected; }

	uint64_t get_guid() const				{ return guid; }

	SLNet::BitStream& get_bs()				{ return *current_bs; }

	std::string get_name() const			{ return name; }
};

inline std::unique_ptr<Client> g_client;

#endif