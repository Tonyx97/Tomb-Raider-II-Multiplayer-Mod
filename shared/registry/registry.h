#pragma once

#ifndef REGISTRY_H
#define REGISTRY_H

#include <Windows.h>

#include <memory>
#include <string>

class Registry
{
private:

	HKEY key = nullptr;

public:

	bool init();

	void destroy();

	std::string get_string(const char* value_name);
	
	bool set_string(const char* value_name, const std::string& value);
};

inline std::unique_ptr<Registry> g_registry;

#endif