#pragma once
#include "memory.h"
#include <iostream>
#include <fstream>

using namespace std;

bool toggle = false;
float resources_boost = 100.0f;

struct map
{
	BYTE reveal_map;
	BYTE no_fog;
};

struct resources
{
	float food;
	float wood;
	float stone;
	float gold;
};

struct memory_ptr
{
	DWORD base_address;
	int total_offsets;
	int offsets[];
};

memory_ptr map_hack_ptr = {
	0x002A1804,
	1,
	{ 0x8DB8 }
};

memory_ptr resources_hack_ptr = {
	0x001830F4,
	5,
	{ 0x524, 0x40, 0x100, 0x30, 0x1f0 }
};

map* map_hack;
resources* resources_hack;

HMODULE getBaseAddress()
{
	return GetModuleHandle(NULL);
}

DWORD* trace_pointer(memory_ptr* hack)
{
	DWORD* location = (DWORD*)((DWORD)getBaseAddress() + (DWORD)hack->base_address);
	for (int i = 0; i < hack->total_offsets; i++)
	{
		location = (DWORD*)(*location + (int)hack->offsets[i]);
	}
	return location;
}

void init_pointer()
{
	if (map_hack == NULL && resources_hack == NULL)
	{
		map_hack = (map*)(trace_pointer(&map_hack_ptr));
		resources_hack = (resources*)(trace_pointer(&resources_hack_ptr));
	}
}

void toggle_map_pointers()
{
	init_pointer();
	if (toggle)
	{
		map_hack->reveal_map = 0;
		map_hack->no_fog = 1;
	}
	else
	{
		map_hack->reveal_map = 1;
		map_hack->no_fog = 0;
	}
	toggle = !toggle;
}

void increase_resources()
{
	init_pointer();
	resources_hack->food += (float)resources_boost;
	resources_hack->wood += (float)resources_boost;
	resources_hack->stone += (float)resources_boost;
	resources_hack->gold += (float)resources_boost;
}








