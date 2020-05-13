#pragma once

#ifndef GUI_H
#define GUI_H

#include <algorithm>
#include <memory>
#include <string>
#include <functional>
#include <vector>
#include <unordered_map>
#include <sstream>
#include <atomic>
#include <cctype>

struct TextGUI
{
	void align_right(bool align);
	void align_bottom(bool align);
	void align_center_h(bool align);
	void align_center_v(bool align);
	void set_scale(int32_t sx, int32_t sy);
	void set_flashing(bool enabled, uint16_t interval = 0);
	void set_box();
	void set_bg(int16_t x_offset = 0, int16_t extra_y = 0, int16_t off_x = 0, int16_t off_y = 0);
	void set_position(int16_t x, int16_t y);
	void set_text(const char* text);

	uint32_t get_width();
	uint32_t get_height();
	uint32_t get_scale_screen_x(uint32_t scale = 0x10000);
	uint32_t get_scale_screen_y(uint32_t scale = 0x10000);

	std::tuple<int16_t, int16_t> get_position();
};

struct GUI
{
private:

	std::unordered_map<std::string, std::function<enum eCmdResult(const std::vector<std::string>&)>> commands;

	std::vector<TextGUI*> chat_msgs;

	std::string chat_msg;

	TextGUI* chat_msg_item = nullptr;

	std::atomic_bool typing = false,
					 checking_for_reset = false;

public:

	void init();
	void on_char_callback(char c);
	void render_chat();
	void add_new_message(const char* msg);
	void send_new_message(const char* msg);

	template <typename... A>
	void add_new_message(const char* msg, A... args)
	{
		char buffer[128] { 0 };
		sprintf_s(buffer, msg, args...);
		add_new_message(buffer);
	}

	template <typename... A>
	void send_new_message(const char* msg, A... args)
	{
		char buffer[128]{ 0 };
		sprintf_s(buffer, msg, args...);
		send_new_message(buffer);
	}
	
	void clear_chatting_state()			{ destroy_text(chat_msg_item); chat_msg.clear(); typing = false; checking_for_reset = true; }
	void reset_check()					{ checking_for_reset = false; }
	void clear_chat()					{ chat_msgs.clear(); }

	TextGUI* create_text(const char* text, int32_t x, int32_t y, uint16_t z = 0);
	TextGUI* get_chat_msg_item()		{ return chat_msg_item; }

	int32_t draw_line(int32_t fx, int32_t fy, int32_t tx, int32_t ty, uint32_t r, uint32_t g, uint32_t b, int32_t z = 0);

	uint16_t get_color(uint32_t r, uint32_t g, uint32_t b);

	bool destroy_text(TextGUI*& item);
	bool get_bool_from_string(const std::string& str);
	bool is_typing() const				{ return typing; }
	bool is_checking_for_reset() const	{ return checking_for_reset; }

	std::string& get_chat_msg()			{ return chat_msg; }
};

inline std::unique_ptr<GUI> g_gui;

#endif