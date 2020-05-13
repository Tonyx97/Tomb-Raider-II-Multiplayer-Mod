#pragma once

#ifndef MHWRAPPER_H
#define MHWRAPPER_H

#include <memory>

#define WIN32_LEAN_AND_MEAN
#include <MinHook.h>

struct MHWrapper
{
	bool init()		{ return (MH_Initialize() == MH_OK); }
	bool destroy()	{ return (MH_Uninitialize() == MH_OK); }

	template <typename T, typename F>
	bool hook_fn(T trampoline, uintptr_t target, F* o_fn)
	{
		if (MH_CreateHook((void*)target, (void*)trampoline, (void**)o_fn) != MH_OK)
			return false;

		return (MH_EnableHook((void*)target) == MH_OK);
	}

	bool unhook_fn(uintptr_t target)
	{
		return (MH_RemoveHook((void*)target) == MH_OK);
	}
};

inline std::unique_ptr<MHWrapper> g_mh;

#endif