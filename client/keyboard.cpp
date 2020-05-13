#include "keyboard.h"

#include <Windows.h>

#include <mhwrapper.h>

#include <offsets.h>
#include <gui.h>

using wnd_proc_t = int(__stdcall*)(HWND, UINT, WPARAM, LPARAM);
wnd_proc_t wnd_proc = nullptr;

int __stdcall hk_window_proc(HWND hwnd, UINT msg, WPARAM wp, LPARAM lp)
{
	if (msg == WM_CHAR)
		g_gui->on_char_callback(wp);

	return wnd_proc(hwnd, msg, wp, lp);
}

bool Keyboard::init()
{
	return g_mh->hook_fn(hk_window_proc, offsets::GAME_WINDOW_PROC, &wnd_proc);
}

bool Keyboard::destroy()
{
	return g_mh->unhook_fn(offsets::GAME_WINDOW_PROC);
}

bool Keyboard::is_key_pressed(int32_t key)
{
	return !!(*(uint8_t*)(offsets::KEY_STATES + key) & 0x80);
}

bool Keyboard::is_key_pressed_w(int32_t key)
{
	return !!(GetAsyncKeyState(key) & 0x8000);
}