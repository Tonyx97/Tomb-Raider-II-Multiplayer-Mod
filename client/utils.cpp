#include "utils.h"

#include <Windows.h>

namespace utils
{
	std::mt19937 g_mersenne;

	std::string rand_str(int32_t len)
	{
		static char alpha[] = "ABCDEFGHIJKLPQORSTUVWXYZabcdefghijkmnopqrstuvwxyz1234567890";

		std::string res;

		g_mersenne.seed(GetTickCount64());

		for (auto i = 0; i < len; ++i)
			res += alpha[g_mersenne() % (sizeof(alpha) - 1)];

		return res;
	}

	std::wstring rand_wstr(int32_t len)
	{
		static char alpha[] = "ABCDEFGHIJKLPQORSTUVWXYZabcdefghijkmnopqrstuvwxyz1234567890";

		std::wstring res;

		g_mersenne.seed(GetTickCount64());

		for (auto i = 0; i < len; ++i)
			res += alpha[g_mersenne() % (sizeof(alpha) - 1)];

		return res;
	}

	MemProt::MemProt(uintptr_t addr, size_t size, uint32_t prot, bool permanent)
	{
		this->permanent = permanent;

		ok = !!VirtualProtect((void*)(this->addr = addr), (this->size = size), prot, &old_prot);
	}

	MemProt::~MemProt()
	{
		if (!ok || permanent)
			return;

		VirtualProtect((void*)addr, size, old_prot, &old_prot);
	}

	void MemProt::execute_under_prot(uintptr_t addr, size_t size, uint32_t prot, const std::function<void()>& fn)
	{
		MemProt temp_obj(addr, size, prot);
		fn();
	}
};