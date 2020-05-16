#pragma once

#ifndef CONSOLE_H
#define CONSOLE_H

#define WIN32_LEAN_AND_MEAN
#include <Windows.h>

#include <functional>
#include <iostream>

#define STD_INPUT_HANDLE    ((DWORD)-10)
#define STD_OUTPUT_HANDLE   ((DWORD)-11)
#define STD_ERROR_HANDLE    ((DWORD)-12)

#define CONSOLE_OUT		GetStdHandle(STD_OUTPUT_HANDLE)
#define CONSOLE_IN		GetStdHandle(STD_INPUT_HANDLE)

#define TO_ASCII(x)		std::string(x.begin(), x.end())
#define TO_UNICODE(x)	std::wstring(x.begin(), x.end())

enum color : unsigned short
{
	black = 0x0,
	dark_blue = 0x1,
	dark_green = 0x2,
	dark_cyan = 0x3,
	dark_red = 0x4,
	dark_purple = 0x5,
	dark_yellow = 0x6,
	grey = 0x7,
	dark_grey = 0x8,
	blue = 0x9,
	green = 0xA,
	cyan = 0xB,
	red = 0xC,
	purple = 0xD,
	yellow = 0xE,
	white = 0xF,
};

namespace console
{
	struct vec2 { int16_t x = 0, y = 0; };

	void init();
	void init_basic();
	void show_cursor(bool show);
	void enable_input(bool enabled);
	void unblock_input();
	void clear_screen(const vec2& pos, int16_t len);
	void set_cursor_pos(int16_t x, int16_t y);

	vec2 get_cursor_pos();
	vec2 get_screen_size();

	char* get_loading_anim(int8_t& size);

	void animated_text(std::function<void(uint32_t)> func, std::function<bool()> cond_func, std::function<void()> end_func, std::function<bool()> error_func, std::function<void()> error_print_func, int32_t loop_time, bool reset_pos);
	bool animated_text(std::function<void(uint32_t)> func, std::function<bool()> cond_func, std::function<bool()> end_func, int32_t loop_time, bool reset_pos);

	template <typename... A>
	static void coutln(color clr, const char* text, A... args)
	{
		if (text)
		{
			char buf[1024] = { 0 };
			sprintf_s(buf, text, std::forward<A>(args)...);
			SetConsoleTextAttribute(CONSOLE_OUT, clr);
			printf_s("%s\n", buf);
		}
		else printf_s("\n");

		SetConsoleTextAttribute(CONSOLE_OUT, white);
	}

	template <typename... A>
	static void cout(color clr, const char* text, A... args)
	{
		char buf[1024] = { 0 };
		sprintf_s(buf, text, std::forward<A>(args)...);
		SetConsoleTextAttribute(CONSOLE_OUT, clr);
		printf_s("%s", buf);
		SetConsoleTextAttribute(CONSOLE_OUT, white);
	}
}

#define println	console::coutln
#define printd	console::cout

#endif