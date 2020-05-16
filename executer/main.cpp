#include <Windows.h>
#include <TlHelp32.h>
#include <ShlObj_core.h>

#include <iostream>
#include <functional>
#include <memory>
#include <fstream>
#include <filesystem>
#include <chrono>
#include <fstream>
#include <sstream>
#include <intrin.h>
#include <thread>

#include <shared/console/console.h>
#include <shared/registry/registry.h>
#include <shared/debug/debug.h>

#define DEBUG 0

uint32_t get_pid(const wchar_t* name)
{
	auto snap_handle = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, NULL);

	std::unique_ptr<HANDLE, std::function<void(HANDLE*)>> snap(&snap_handle, [](HANDLE* h) -> void { CloseHandle(*h); });

	PROCESSENTRY32W entry;
	entry.dwSize = sizeof(PROCESSENTRY32W);

	if (Process32FirstW(*snap.get(), &entry))
		do {
			if (!wcscmp(entry.szExeFile, name))
				return entry.th32ProcessID;
			
		} while (Process32NextW(*snap.get(), &entry));

	return 0;
}

bool inject_dll(uint32_t pid, const std::wstring& dll_path)
{
	HANDLE token;
	TOKEN_PRIVILEGES tkp;

	if (OpenProcessToken(GetCurrentProcess(), TOKEN_ADJUST_PRIVILEGES | TOKEN_QUERY, &token))
	{
		LookupPrivilegeValue(nullptr, SE_DEBUG_NAME, &tkp.Privileges[0].Luid);
		tkp.PrivilegeCount = 1;
		tkp.Privileges[0].Attributes = SE_PRIVILEGE_ENABLED;

		AdjustTokenPrivileges(token, 0, &tkp, sizeof(tkp), nullptr, nullptr);
	}

	size_t dll_size = (dll_path.length() * 2) + 2;

	HANDLE game_handle = OpenProcess(PROCESS_ALL_ACCESS, FALSE, pid);

	if (!game_handle || game_handle == INVALID_HANDLE_VALUE)
		return dbg::mod_error(L"Injection error 1");

	auto allocated_mem = VirtualAllocEx(game_handle, nullptr, dll_size, MEM_RESERVE | MEM_COMMIT, PAGE_EXECUTE_READWRITE);

	if (!allocated_mem)
		return dbg::mod_error(L"Injection error 2");

	if (!WriteProcessMemory(game_handle, allocated_mem, dll_path.c_str(), dll_size, 0))
		return dbg::mod_error(L"Injection error 3");

	HANDLE thread = CreateRemoteThread(game_handle, nullptr, 0, (LPTHREAD_START_ROUTINE)GetProcAddress(LoadLibrary(L"kernel32.dll"), "LoadLibraryW"), allocated_mem, 0, nullptr);

	if (!thread)
		return dbg::mod_error(L"Injection error 4");

	WaitForSingleObject(thread, INFINITE);

	DWORD ret = 0;

	GetExitCodeThread(thread, &ret);

	CloseHandle(thread);
	CloseHandle(game_handle);

	return true;
}

std::string get_current_path()
{
	char path[_MAX_PATH + 1];
	GetModuleFileNameA(NULL, path, _MAX_PATH);
	auto str = std::string(path);
	return str.substr(0, str.find_last_of("\\/") + 1);
}

int __stdcall browse_game_path_proc(HWND hwnd, UINT msg, LPARAM lparam, LPARAM data)
{
	if (msg == BFFM_INITIALIZED)
		SendMessage(hwnd, BFFM_SETSELECTION, TRUE, data);

	return 0;
}

int main()
{
	console::init_basic();

	console::show_cursor(true);
	console::enable_input(true);

	println(color::yellow, "***** Welcome to TRIIO (Tomb Raider II Multiplayer Mod) *****\n");

	g_registry.reset(new Registry());

	if (!g_registry->init())
		return dbg::mod_error(L"Registry system init failed");

	auto game_path = g_registry->get_string("game_path");

	auto search_game = [&]()
	{
		bool close_dialog = false;

		while (!close_dialog)
		{
			BROWSEINFOW bi { 0 };

			bi.lpszTitle = L"Please specify the directory of Tomb Raider II game";
			bi.ulFlags = BIF_USENEWUI;
			bi.lpfn = browse_game_path_proc;
			bi.lParam = (LPARAM)L"C:\\";

			if (LPITEMIDLIST idl = SHBrowseForFolder(&bi))
			{
				char buffer[MAX_PATH] { 0 };

				SHGetPathFromIDListA(idl, buffer);

				std::string path = buffer;

				if (std::filesystem::is_regular_file(path + "\\Tomb2.exe"))
				{
					g_registry->set_string("game_path", path);

					close_dialog = true;

					game_path = path;

					dbg::mod_ok(L"Tomb2.exe found");
				}
				else dbg::mod_error(L"Tomb2.exe does not exist in this folder", false);

				if (IMalloc* imalloc = 0; SUCCEEDED(SHGetMalloc(&imalloc)))
				{
					imalloc->Free(idl);
					imalloc->Release();
				}
			}
			else close_dialog = true;
		}
	};

	if (game_path.empty())
		search_game();
	else println(color::green, "Game folder: '%s'", game_path.c_str());

	auto load_game = [&](const std::string& ip)
	{
		if (game_path.empty())
			search_game();

		if (game_path.empty())
			return dbg::mod_error(L"Please specify the folder where the game is located");

		game_path += "\\";

		STARTUPINFO si;
		PROCESS_INFORMATION pi;

		memset(&si, 0, sizeof(si));
		memset(&pi, 0, sizeof(pi));

		si.cb = sizeof(si);

		const auto current_path = get_current_path();

		const std::wstring game_path_uni = std::wstring(game_path.begin(), game_path.end()),
						   full_game_path = game_path_uni + L"Tomb2.exe",
						   current_dll_path = std::wstring(current_path.begin(), current_path.end()) + L"triio_c.dll",
						   cmd_line = std::wstring(ip.begin(), ip.end());

		println(color::cyan, "Launching Tomb Raider II...");

		if (!CreateProcessW(full_game_path.c_str(), (LPWSTR)cmd_line.c_str(), nullptr, nullptr, FALSE, 0, nullptr, game_path_uni.c_str(), &si, &pi))
			return dbg::mod_error(L"Could not launch the game");

		println(color::cyan, "Tomb Raider II launched");

		if (!inject_dll(get_pid(L"Tomb2.exe"), current_dll_path))
			return dbg::mod_error(L"Injection failed");

		println(color::cyan, "TRIIO initialized");

		return true;
	};

#if DEBUG
	if (!get_pid(L"Tomb2.exe"))
		load_game("212.114.52.62");
	else
	{
		const auto current_path = get_current_path();

		const std::wstring current_dll_path = std::wstring(current_path.begin(), current_path.end()) + L"triio_c.dll";

		if (!inject_dll(get_pid(L"Tomb2.exe"), current_dll_path))
			return dbg::mod_error(L"Injection failed");
	}
#else
	bool dispatch_commands = true;

	while (dispatch_commands)
	{
		std::vector<std::string> params;
		std::string cmd,
					param;

		printd(white, "\n> ");

		char cmd_line[128] { 0 };
		std::cin.getline(cmd_line, sizeof(cmd_line));

		std::stringstream ss(cmd_line);

		ss >> cmd;

		while (ss >> param)
			params.push_back(param);

		println(white, "\n");

		if (!cmd.compare("connect") && params.size() == 1)
		{
			load_game(params[0]);

			dispatch_commands = false;
		}
		else if (!cmd.compare("exit") || !cmd.compare("quit"))
			dispatch_commands = false;

		_mm_pause();
	}
#endif
	
	g_registry->destroy();

	g_registry.reset();

	println(color::cyan, "Exiting...");

	std::this_thread::sleep_for(std::chrono::seconds(5));

	return 0;
}