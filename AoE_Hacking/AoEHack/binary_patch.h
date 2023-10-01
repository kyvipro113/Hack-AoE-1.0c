#pragma once
#include "memory.h"
#include <iostream>
#include <fstream>

using namespace std;

DWORD patch_main_map = 0x005124E6;
DWORD patch_main_objects = 0x00512604;
DWORD patch_main_fog = 0x00540A4C;
DWORD patch_mini_map = 0x0042D228;
DWORD patch_mini_fog = 0x0042D993;

const int total_memory_hacks = 5;
const int patch_size = 2;
bool patched = false;

struct memory_hack
{
    DWORD patch_address;
    BYTE patch_bytes[patch_size];
    BYTE original_bytes[patch_size];
};

struct memory_hack memory_hacks[total_memory_hacks];

void init_binary_hack()
{
    memory_hacks[0] = { patch_main_map, { 0xeb, 0x14 }, { 0 } };
    memory_hacks[1] = { patch_main_objects, { 0x90, 0x90 }, { 0 } };
    memory_hacks[2] = { patch_main_fog, { 0xeb, 0x07 }, { 0 } };
    memory_hacks[3] = { patch_mini_map, { 0xeb, 0x1e }, { 0 } };
    memory_hacks[4] = { patch_mini_fog, { 0x90, 0x90 }, { 0 } };

    for (int i = 0; i < total_memory_hacks; i++)
    {
        readBytes((void*)memory_hacks[i].patch_address, &memory_hacks[i].original_bytes, patch_size);
    }
}

void toggle_map_patch()
{
    void* to_patch;
    for (int i = 0; i < total_memory_hacks; i++)
    {
        if (patched)
        {
            to_patch = &memory_hacks[i].original_bytes;
        }
        else
        {
            to_patch = &memory_hacks[i].patch_bytes;
        }
        writeBytes((void*)memory_hacks[i].patch_address, to_patch, patch_size);
    }
    patched = !patched;
}
