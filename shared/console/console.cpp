#define _CRT_SECURE_NO_WARNINGS

#include "console.h"

#include <iostream>
#include <io.h>
#include <fcntl.h>
#include <thread>

static char g_anim_1[] = { '/', '-', '\\', '-', '|' };
static char g_anim_2[] = { '<', 'V', '>', '^' };

using namespace console;

void console::init()
{
	AllocConsole();
	freopen("CONOUT$", "w", stdout);

	SetConsoleTitle(L"TRIIO");

	Sleep(100);

	auto game_window = FindWindow(L"Dude:TombRaiderII:DDWndClass", nullptr);

	RECT game_window_rect;
	GetWindowRect(game_window, &game_window_rect);

	auto width = game_window_rect.right - game_window_rect.left;
	auto height = game_window_rect.bottom - game_window_rect.top;
	auto screen_width = GetSystemMetrics(SM_CXSCREEN);
	auto screen_height = GetSystemMetrics(SM_CYSCREEN);

	auto console_window = GetConsoleWindow();

	CONSOLE_FONT_INFOEX cfi{ 0 };
	cfi.cbSize = sizeof(cfi);

	GetCurrentConsoleFontEx(CONSOLE_OUT, FALSE, &cfi);

	cfi.dwFontSize.X = 0;
	cfi.dwFontSize.Y = 16;
	wcscpy(cfi.FaceName, L"Courier New");

	SetCurrentConsoleFontEx(GetStdHandle(STD_OUTPUT_HANDLE), FALSE, &cfi);
	SetConsoleTextAttribute(CONSOLE_OUT, white);

	SetWindowPos(console_window, 0, game_window_rect.right + 10, 350, screen_width - width - 150, 1000, 0);
	ShowWindow(console_window, SW_SHOW);

	BringWindowToTop(console_window);
	SetForegroundWindow(console_window);
	BringWindowToTop(game_window);
	SetForegroundWindow(game_window);

	show_cursor(false);
	enable_input(false);
}

void console::init_basic()
{
	SetConsoleTitle(L"TRIIO");

	auto screen_width = GetSystemMetrics(SM_CXSCREEN);
	auto screen_height = GetSystemMetrics(SM_CYSCREEN);

	auto console_window = GetConsoleWindow();

	CONSOLE_FONT_INFOEX cfi{ 0 };
	cfi.cbSize = sizeof(cfi);

	GetCurrentConsoleFontEx(CONSOLE_OUT, FALSE, &cfi);

	cfi.dwFontSize.X = 0;
	cfi.dwFontSize.Y = 16;
	wcscpy_s(cfi.FaceName, L"Courier New");

	SetCurrentConsoleFontEx(GetStdHandle(STD_OUTPUT_HANDLE), FALSE, &cfi);
	SetConsoleTextAttribute(CONSOLE_OUT, white);

	SetWindowPos(console_window, 0, screen_width / 2 - 400, screen_height / 2 - 400, 800, 800, 0);
	ShowWindow(console_window, SW_SHOW);

	BringWindowToTop(console_window);
	SetForegroundWindow(console_window);

	show_cursor(false);
	enable_input(false);
}

void console::show_cursor(bool show)
{
	CONSOLE_CURSOR_INFO info { 0 };
	GetConsoleCursorInfo(CONSOLE_OUT, &info);
	info.bVisible = show;
	SetConsoleCursorInfo(CONSOLE_OUT, &info);
}

void console::enable_input(bool enabled)
{
	DWORD mode = 0;
	GetConsoleMode(CONSOLE_IN, &mode);
	if (enabled)	mode |= ENABLE_ECHO_INPUT;
	else			mode &= ~ENABLE_ECHO_INPUT;
	SetConsoleMode(CONSOLE_IN, mode);
}

void console::unblock_input()
{
	DWORD tmp;
	INPUT_RECORD ir[2] { 0 };

	ir[0].EventType = KEY_EVENT;
	ir[0].Event.KeyEvent.bKeyDown = TRUE;
	ir[0].Event.KeyEvent.dwControlKeyState = 0;
	ir[0].Event.KeyEvent.uChar.UnicodeChar = VK_RETURN;
	ir[0].Event.KeyEvent.wRepeatCount = 1;
	ir[0].Event.KeyEvent.wVirtualKeyCode = VK_RETURN;
	ir[0].Event.KeyEvent.wVirtualScanCode = MapVirtualKey(VK_RETURN, MAPVK_VK_TO_VSC);
	ir[1] = ir[0];
	ir[1].Event.KeyEvent.bKeyDown = FALSE;

	WriteConsoleInput(GetStdHandle(STD_INPUT_HANDLE), ir, 2, &tmp);
}

void console::clear_screen(const vec2& pos, int16_t len)
{
	CONSOLE_SCREEN_BUFFER_INFO screen { 0 };
	DWORD written;
	FillConsoleOutputCharacterA(CONSOLE_OUT, ' ', 0xFF, COORD { pos.x, pos.y }, &written);
}

void console::set_cursor_pos(int16_t x, int16_t y)
{
	SetConsoleCursorPosition(CONSOLE_OUT, { x, y });
}

vec2 console::get_cursor_pos()
{
	CONSOLE_SCREEN_BUFFER_INFO cbsi { 0 };
	GetConsoleScreenBufferInfo(CONSOLE_OUT, &cbsi);
	return *(vec2*)&cbsi.dwCursorPosition;
}

vec2 console::get_screen_size()
{
	CONSOLE_SCREEN_BUFFER_INFO cbsi { 0 };
	GetConsoleScreenBufferInfo(CONSOLE_OUT, &cbsi);
	return *(vec2*)&cbsi.dwSize;
}

char* console::get_loading_anim(int8_t& size)
{
	size = sizeof(g_anim_1);
	return g_anim_1;
}

void console::animated_text(std::function<void(uint32_t)> func, std::function<bool()> cond_func, std::function<void()> end_func, std::function<bool()> error_func, std::function<void()> error_print_func, int32_t loop_time, bool reset_pos)
{
	const auto start = get_cursor_pos();

	enable_input(false);
	show_cursor(false);

	uint32_t i = 0;

	while (cond_func())
	{
		set_cursor_pos(start.x, start.y);

		if (error_func())
		{
			error_print_func();
			show_cursor(true);
			enable_input(true);
			return;
		}
		else func(i++);

		std::this_thread::sleep_for(std::chrono::milliseconds(loop_time));
	}

	if (reset_pos)
		set_cursor_pos(start.x, start.y);

	end_func();

	show_cursor(true);
	enable_input(true);
}

bool console::animated_text(std::function<void(uint32_t)> func, std::function<bool()> cond_func, std::function<bool()> end_func, int32_t loop_time, bool reset_pos)
{
	const auto start = get_cursor_pos();

	enable_input(false);
	show_cursor(false);

	uint32_t i = 0;

	while (cond_func())
	{
		set_cursor_pos(start.x, start.y);

		func(i++);

		std::this_thread::sleep_for(std::chrono::milliseconds(loop_time));
	}

	if (reset_pos)
	{
		set_cursor_pos(start.x, start.y);
		clear_screen(start, 0xFF);
	}

	const auto result = end_func();

	show_cursor(true);
	enable_input(true);

	return result;
}