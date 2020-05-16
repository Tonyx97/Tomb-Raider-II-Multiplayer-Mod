#pragma once

#ifndef DEBUG_H
#define DEBUG_H

namespace dbg
{
	void mod_error_v(const wchar_t* msg);

	bool mod_error(const wchar_t* msg, bool forced_exit = true);
	bool mod_warning(const wchar_t* msg);
	bool mod_ok(const wchar_t* msg);
}

#endif