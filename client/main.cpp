#include <cstdio>
#include <cstring>
#include <stdlib.h>
#include <limits>
#include <thread>
#include <chrono>
#include <intrin.h>

#include <client/client.h>

#include <console/console.h>

#include <debug.h>
#include <mhwrapper.h>
#include <vehhook.h>
#include <registry.h>
#include <net_game_logic.h>
#include <local_game.h>
#include <game_logic.h>
#include <entity.h>
#include <room.h>
#include <player.h>
#include <gui.h>
#include <keyboard.h>

HINSTANCE g_dll_instance = nullptr;

int main_thread()
{
	console::init();

	g_mh.reset(new MHWrapper());
	g_veh.reset(new VEHHKR());
	g_registry.reset(new Registry());
	g_gui.reset(new GUI());
	g_net_game.reset(new NetGameLogic());
	g_local_game.reset(new LocalGame());
	g_game.reset(new GameLogic());
	g_kb.reset(new Keyboard());
	g_player_system.reset(new PlayerSystem());
	g_entity_system.reset(new EntitySystem());
	g_room_system.reset(new RoomSystem());
	g_client.reset(new Client());

	// init all systems

	g_registry->init();
	g_gui->init();
	g_net_game->init();

	if (!g_mh->init())
		return dbg::mod_error(L"Hooking system init failed");

	if (!g_game->init())
		return dbg::mod_error(L"Game logic init failed");

	if (!g_client->init())
		return dbg::mod_error(L"Client init failed");

	// connect to the server

	if (!g_client->connect("212.114.52.62", GAME_PORT))
		return dbg::mod_error(L"Connection setup failed");

	std::this_thread::sleep_for(std::chrono::seconds(1));

	// check if we are connected to the server

	for (auto i = 0; (i < 5 && !g_client->is_connected()); ++i)
		std::this_thread::sleep_for(std::chrono::seconds(1));

	if (!g_client->is_connected())
		return dbg::mod_error(L"Connection to the server failed");

	if (!g_kb->init())
		return dbg::mod_error(L"Input system init failed");

	// idle this thread

	while (!g_kb->is_key_pressed_w(VK_F8) && g_client->is_connected())
		std::this_thread::sleep_for(std::chrono::microseconds(7812));

	// clean and destroy systems

	g_kb->destroy();
	g_game->destroy();
	g_mh->destroy();
	g_registry->destroy();

	g_client.reset();
	g_room_system.reset();
	g_entity_system.reset();
	g_player_system.reset();
	g_kb.reset();
	g_game.reset();
	g_local_game.reset();
	g_net_game.reset();
	g_gui.reset();
	g_registry.reset();
	g_veh.reset();
	g_mh.reset();

	FreeLibraryAndExitThread(g_dll_instance, 0);

	return 0;
}

BOOL __stdcall DllMain(HINSTANCE dll, DWORD reason, LPVOID reserved)
{
	if (reason == DLL_PROCESS_ATTACH)
	{
		g_dll_instance = dll;

		CreateThread(nullptr, 0, (LPTHREAD_START_ROUTINE)main_thread, nullptr, 0, nullptr);
	}

	return true;
}