#pragma once

#ifndef BITSTREAMING_H
#define BITSTREAMING_H

#include <mutex>
#include <atomic>

#include <slikenet/peerinterface.h>
#include <slikenet/BitStream.h>

#include "messages.h"
#include "game_net_structs.h"

#define GAME_PORT 51515

struct BitStreaming
{
private:

	std::mutex mtx;

protected:

	SLNet::RakPeerInterface* peer = nullptr;

	SLNet::BitStream* current_bs = nullptr;

	std::atomic_bool bs_ready = false;

public:

	uint16_t get_packet_id(SLNet::Packet* p, SLNet::BitStream& bs_in)
	{
		if (!p)
			return UINT16_MAX;

		bs_in.IgnoreBytes(sizeof(SLNet::MessageID));

		if (p->data[0] == ID_ENGINE_PACKET_TYPE)
		{
			uint16_t packet_id = 0;
			bs_in.Read(packet_id);
			return packet_id;
		}

		return (uint16_t)p->data[0];
	}

	void push_args_to_bs() {}
	void push_args_to_bs(SLNet::BitStream&) {}

	template <typename T, typename... A>
	void push_args_to_bs(SLNet::BitStream& bs, const T& a1, A&&... args)
	{
		bs.Write<T>(a1);
		push_args_to_bs(bs, std::forward<A>(args)...);
	}

	template <typename... A>
	bool send_packet(ePacketID msg_id, A... args)
	{
		//std::lock_guard<std::mutex> lock(mtx);
		
		if (!peer)
			return false;

		SLNet::BitStream bs;

		if (msg_id > ID_ENGINE_PACKET_TYPE)
		{
			bs.Write<SLNet::MessageID>(ID_ENGINE_PACKET_TYPE);
			bs.Write<ePacketID>(msg_id);
		}
		else bs.Write<SLNet::MessageID>(msg_id);

		push_args_to_bs(bs, args...);

		return !!peer->Send(&bs, HIGH_PRIORITY, RELIABLE_ORDERED, 0, SLNet::UNASSIGNED_SYSTEM_ADDRESS, true);
	}

	template <typename... A>
	bool send_packet_broadcast(ePacketID msg_id, const SLNet::SystemAddress& bc_client, bool broadcast, const A&... args)
	{
		//std::lock_guard<std::mutex> lock(mtx);

		if (!peer)
			return false;

		SLNet::BitStream bs;

		if (msg_id > ID_ENGINE_PACKET_TYPE)
		{
			bs.Write<SLNet::MessageID>(ID_ENGINE_PACKET_TYPE);
			bs.Write<ePacketID>(msg_id);
		}
		else bs.Write<SLNet::MessageID>(msg_id);

		push_args_to_bs(bs, args...);

		return !!peer->Send(&bs, HIGH_PRIORITY, RELIABLE_ORDERED, 0, bc_client, broadcast);
	}

	void extract_args_from_bs() {}
	void extract_args_from_bs(SLNet::BitStream&) {}

	template <typename T, typename... A>
	void extract_args_from_bs(SLNet::BitStream& bs, T& a1, A&... args)
	{
		bs.Read<T>(a1);
		extract_args_from_bs(bs, std::forward<A&>(args)...);
	}

	template <typename... A>
	void read_packet(SLNet::BitStream& bs, A&... args)
	{
		if (!peer)
			return;

		extract_args_from_bs(bs, std::forward<A&>(args)...);
	}

	template <typename... A>
	void read_packet_ex(A&... args)
	{
		if (!peer)
			return;

		extract_args_from_bs(*current_bs, std::forward<A&>(args)...);
	}
};

#endif