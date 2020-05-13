#pragma once

#ifndef VEHHOOK_H
#define VEHHOOK_H

#include <memory>
#include <unordered_map>

struct VEHHook
{
	uintptr_t target = 0,
			  hook = 0;

	uint8_t original_byte = 0;
};

class VEHHKR
{
private:

	std::unordered_map<void*, VEHHook> hooks;

	uintptr_t addr = 0;

public:

	VEHHKR();
	~VEHHKR();

	bool add_hook(uintptr_t target, void* hook);
	bool remove_hook(uintptr_t target);

	decltype(hooks)& get_hooks() { return hooks; }
};

inline std::unique_ptr<VEHHKR> g_veh;

#endif