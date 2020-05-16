#include "vehhook.h"

#include <Windows.h>

#include <shared/utils/utils.h>

LONG __stdcall global_veh(struct _EXCEPTION_POINTERS* exception)
{
	auto& hooks = g_veh->get_hooks();

	if (auto it = hooks.find(exception->ExceptionRecord->ExceptionAddress); it != hooks.end())
	{
		exception->ContextRecord->Eip = it->second.hook;
		return EXCEPTION_CONTINUE_EXECUTION;
	}

	return EXCEPTION_CONTINUE_SEARCH;
}

VEHHKR::VEHHKR()
{
	AddVectoredExceptionHandler(1, global_veh);
}

VEHHKR::~VEHHKR()
{
	for (const auto& [addr, hook_info] : hooks)
	{
		utils::MemProt prot((uintptr_t)addr, 1, PAGE_EXECUTE_READWRITE);
		*(uint8_t*)addr = hook_info.original_byte;
	}

	RemoveVectoredExceptionHandler(global_veh);
}

bool VEHHKR::add_hook(uintptr_t target, void* hook)
{
	utils::MemProt prot(target, 1, PAGE_EXECUTE_READWRITE);

	const auto original_byte = *(uint8_t*)target;

	*(uint8_t*)target = 0xCC;

	return hooks.insert({ (void*)target, { target, (uintptr_t)hook, original_byte } }).second;
}

bool VEHHKR::remove_hook(uintptr_t target)
{
	auto it = hooks.find((void*)target);

	if (it == hooks.end())
		return false;

	const auto& hook_info = it->second;

	utils::MemProt prot(hook_info.target, 1, PAGE_EXECUTE_READWRITE);

	*(uint8_t*)hook_info.target = hook_info.original_byte;

	hooks.erase(it);

	return true;
}