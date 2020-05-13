#pragma once

#ifndef INVENTORY_H
#define INVENTORY_H

#include <shared/offsets.h>

struct Inventory
{
	static Inventory* get()
	{
		return (Inventory*)offsets::INVENTORY;
	}

	uint16_t get_selected_item()
	{
		return *(uint16_t*)((uintptr_t)this + offsets::SELECTED_ITEM);
	}
};

#endif