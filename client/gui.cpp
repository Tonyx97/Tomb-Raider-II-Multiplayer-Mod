#include "gui.h"

#include <tuple>

#include <shared/console/console.h>

#include <client/client.h>

#include <offsets.h>
#include <entity.h>
#include <player.h>
#include <local_game.h>
#include <game_logic.h>
#include <commands.h>

void TextGUI::align_right(bool align)
{
	reinterpret_cast<void(*)(TextGUI*, bool)>(offsets::TEXT_ALIGN_RIGHT)(this, align);
}

void TextGUI::align_bottom(bool align)
{
	reinterpret_cast<void(*)(TextGUI*, bool)>(offsets::TEXT_ALIGN_BOTTOM)(this, align);
}

void TextGUI::align_center_h(bool align)
{
	reinterpret_cast<void(*)(TextGUI*, bool)>(offsets::TEXT_ALIGN_CENTER_H)(this, align);
}

void TextGUI::align_center_v(bool align)
{
	reinterpret_cast<void(*)(TextGUI*, bool)>(offsets::TEXT_ALIGN_CENTER_V)(this, align);
}

void TextGUI::set_scale(int32_t sx, int32_t sy)
{
	reinterpret_cast<void(*)(TextGUI*, int32_t, int32_t)>(offsets::TEXT_SET_SCALE)(this, sx, sy);
}

void TextGUI::set_flashing(bool enabled, uint16_t interval)
{
	reinterpret_cast<void(*)(TextGUI*, uint16_t, uint16_t)>(offsets::TEXT_SET_FLASHING)(this, enabled, interval);
}

void TextGUI::set_box()
{
	reinterpret_cast<void(*)(TextGUI*, uint32_t, uint16_t, uint32_t, uint16_t)>(offsets::TEXT_SET_BOX)(this, 1, 15, 0, 0);
}

void TextGUI::set_bg(int16_t extra_x, int16_t extra_y, int16_t off_x, int16_t off_y)
{
	reinterpret_cast<void(*)(TextGUI*, int16_t, int16_t, int16_t, int16_t, uint16_t, uint16_t, int32_t, uint16_t)>(offsets::TEXT_SET_BG)(this, extra_x, extra_y, off_x, off_y, 0, 0, 0, 0);
}

void TextGUI::set_position(int16_t x, int16_t y)
{
	*(int16_t*)((uintptr_t)this + 0xA) = x * get_scale_screen_x() >> 16;
	*(int16_t*)((uintptr_t)this + 0xC) = y * get_scale_screen_y() >> 16;
}

void TextGUI::set_text(const char* text)
{
	reinterpret_cast<void(*)(TextGUI*, const char*)>(offsets::TEXT_UPDATE)(this, text);
}

uint32_t TextGUI::get_width()
{
	return *(uint32_t*)((uintptr_t)this + 0x2E);
}

uint32_t TextGUI::get_height()
{
	return *(uint32_t*)((uintptr_t)this + 0x32);
}

uint32_t TextGUI::get_scale_screen_x(uint32_t scale)
{
	return reinterpret_cast<uint32_t(*)(uint32_t)>(offsets::TEXT_GET_SCALE_SCREEN_X)(scale);
}

uint32_t TextGUI::get_scale_screen_y(uint32_t scale)
{
	return reinterpret_cast<uint32_t(*)(uint32_t)>(offsets::TEXT_GET_SCALE_SCREEN_Y)(scale);
}

std::tuple<int16_t, int16_t> TextGUI::get_position()
{
	return { (*(int16_t*)((uintptr_t)this + 0xA) << 16) / get_scale_screen_x(),
			 (*(int16_t*)((uintptr_t)this + 0xC) << 16) / get_scale_screen_y() };
}

void GUI::init()
{
	commands.insert({ "name",			commands::change_name });
	commands.insert({ "tp",				commands::teleport });
	commands.insert({ "kill",			commands::kill });
	commands.insert({ "boom",			commands::boom });
	commands.insert({ "level",			commands::level });
	commands.insert({ "ff",				commands::friendly_fire });
	commands.insert({ "giveitem",		commands::give_item });
	commands.insert({ "godmode",		commands::godmode });
	commands.insert({ "heal",			commands::heal });
	commands.insert({ "infoxygen",		commands::infinite_oxygen });
	commands.insert({ "infammo",		commands::infinite_ammo });
	commands.insert({ "nextlevel",		commands::next_level });
	commands.insert({ "boatmaxvel",		commands::boat_max_velocity });
	commands.insert({ "skidoomaxvel",	commands::skidoo_max_velocity });
}

void GUI::on_char_callback(char c)
{
	if (checking_for_reset)
		return;
	
	// open chat
	if (!chat_msg_item && (c == 't' || c == 'T'))
	{
		chat_msg_item = create_text(chat_msg.c_str(), 15, -10);
		chat_msg_item->align_bottom(true);
		chat_msg_item->set_scale(35000, 40000);
		chat_msg_item->set_bg();

		typing = true;
	}
	else if (typing)
	{
		// close chat
		if (c == 0x1B)
			clear_chatting_state();
		else if (c == 0xD)
		{
			// send message / handle command

			if (!chat_msg.empty() && chat_msg.length() < 64)
			{
				// handle cmd
				if (*chat_msg.begin() == '/')
				{
					const auto first_space = chat_msg.find_first_of(" ");
					const bool has_params = (first_space != std::string::npos);

					std::string cmd(chat_msg.begin() + 1,
									has_params ? chat_msg.begin() + first_space : chat_msg.end());

					if (const auto it = commands.find(cmd); it != commands.end())
					{
						const auto cmd_fn = it->second;

						std::vector<std::string> params;

						if (has_params)
						{
							std::stringstream params_ss(std::string(chat_msg.begin() + first_space + 1, chat_msg.end()));

							std::string param;

							while (std::getline(params_ss, param, ' '))
								params.push_back(param);
						}

						switch (cmd_fn(params))
						{
						case CMD_INVALID_PARAMS:
						{
							add_new_message("Invalid parameters");
							break;
						}
						case CMD_FAIL:
						{
							println(color::red, "failed executing command '%s'", cmd.c_str());
							break;
						}
						}
					}
					else println(color::red, "Command '%s' does not exist", cmd.c_str());
				}
				else
				{
					// send message

					chat_message_info info;

					sprintf_s(info.msg, "%s: %s", g_client->get_name().c_str(), chat_msg.c_str());

					g_client->send_packet<chat_message_info>(ID_CHAT_MESSAGE, info);

					add_new_message(info.msg);
				}
			}

			clear_chatting_state();
			destroy_text(chat_msg_item);
		}
		else
		{
			// add new char to message

			if (c != 0x8 && chat_msg.length() < 60 - g_client->get_name().length())
				chat_msg += c;
			else if (c == 0x8 && !chat_msg.empty())
				chat_msg.pop_back();
		}
	}
}

void GUI::render_chat()
{
	if (typing && chat_msg_item)
		chat_msg_item->set_text(("- " + chat_msg).c_str());
}

void GUI::add_new_message(const char* msg)
{
	if (strlen(msg) >= 63)
		return;

	auto new_chat_msg_item = g_gui->create_text(msg, 15, -35);
	new_chat_msg_item->align_bottom(true);
	new_chat_msg_item->set_scale(37500, 42500);
	new_chat_msg_item->set_bg();

	if (chat_msgs.size() >= 10)
	{
		const auto msg_to_remove = chat_msgs.begin();
		g_gui->destroy_text(*msg_to_remove);
		chat_msgs.erase(msg_to_remove);
	}

	for (const auto c_text : chat_msgs)
	{
		const auto& [tpx, tpy] = c_text->get_position();
		c_text->set_position(tpx, tpy - 15);
	}

	chat_msgs.push_back(new_chat_msg_item);
}

void GUI::send_new_message(const char* msg)
{
	chat_message_info info;

	strcpy_s(info.msg, msg);

	g_client->send_packet(ID_CHAT_MESSAGE, info);
}

TextGUI* GUI::create_text(const char* text, int32_t x, int32_t y, uint16_t z)
{
	return reinterpret_cast<TextGUI*(*)(int32_t, int32_t, uint16_t, const char*)>(offsets::CREATE_GUI_TEXT)(x, y, z, text);
}

int32_t GUI::draw_line(int32_t fx, int32_t fy, int32_t tx, int32_t ty, uint32_t r, uint32_t g, uint32_t b, int32_t z)
{
	return reinterpret_cast<int32_t(*)(int32_t, int32_t, int32_t, int32_t, int32_t, uint8_t)>(offsets::DRAW_LINE)(fx, fy, tx, ty, z, get_color(r, g, b));
}

uint16_t GUI::get_color(uint32_t r, uint32_t g, uint32_t b)
{
	return reinterpret_cast<uint16_t(*)(uint32_t, uint32_t, uint32_t)>(offsets::GET_COLOR_FROM_RGB)(r, g, b);
}

bool GUI::destroy_text(TextGUI*& item)
{
	const bool destroyed = reinterpret_cast<bool(*)(TextGUI*)>(offsets::REMOVE_GUI_TEXT)(item);
	item = nullptr;
	return destroyed;
}

bool GUI::get_bool_from_string(const std::string& str)
{
	if (*(uint32_t*)str.data() == 'eurt')	return true;
	else									return !!std::atol(str.c_str());
}