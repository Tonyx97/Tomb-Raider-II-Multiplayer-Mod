#include "debug.h"

#include <Windows.h>

namespace dbg
{
	void mod_error_v(const wchar_t* msg)
	{
		MessageBoxW(nullptr, msg, L"TRIIO", MB_ICONERROR | MB_SYSTEMMODAL);
	}

	bool mod_error(const wchar_t* msg)
	{
		MessageBoxW(nullptr, msg, L"TRIIO", MB_ICONERROR | MB_SYSTEMMODAL);
		return false;
	}

	bool mod_warning(const wchar_t* msg)
	{
		MessageBoxW(nullptr, msg, L"TRIIO", MB_ICONWARNING | MB_SYSTEMMODAL);
		return true;
	}

	bool mod_ok(const wchar_t* msg)
	{
		MessageBoxW(nullptr, msg, L"TRIIO", MB_ICONINFORMATION | MB_OK | MB_SYSTEMMODAL);
		return true;
	}
};