#pragma once

#ifndef KEYBOARD_H
#define KEYBOARD_H

#include <memory>

#include <key_list.h>

struct Keyboard
{
	bool init();
	bool destroy();
	bool is_key_pressed(int32_t key);
	bool is_key_pressed_w(int32_t key);
};

inline std::unique_ptr<Keyboard> g_kb;

#endif