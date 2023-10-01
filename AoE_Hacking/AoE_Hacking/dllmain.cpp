// dllmain.cpp : Defines the entry point for the DLL application.
#include "pch.h"

#include <iostream>
#include <Windows.h>
#include <fstream>
using namespace std;

const int total_memory_hacks = 5;
const int patch_size = 2;
bool patched = false;

struct memory_hack
{
	long patch_address;
	BYTE patch_bytes[patch_size];
	BYTE original_bytes[patch_size];
};


struct memory_hack memory_hacks[total_memory_hacks];

HANDLE aoe_handle = GetCurrentProcess();

void patch()
{
	void* to_patch;
	//ofstream file;
	//file.open("C:\\Users\\marin\\source\\repos\\record.txt");
	for (int i = 0; i < total_memory_hacks; ++i)
	{
		if (patched)
		{
			to_patch = &memory_hacks[i].original_bytes;
			/*file << "Mem_org" << "\n";
			file << memory_hacks[i].original_bytes << "\n";*/
		}
		else
		{
			to_patch = &memory_hacks[i].patch_bytes;
			/*file << "Mem_hack" << "\n";
			file << memory_hacks[i].patch_bytes << "\n";*/
		}

		WriteProcessMemory(aoe_handle, (void*)memory_hacks[i].patch_address, to_patch, patch_size, 0);
		/*cout << "Mem_ORG: " << memory_hacks[i].original_bytes << endl;
		cout << "Mem_Hack: " << memory_hacks[i].patch_bytes << endl;*/
		//file.close();

	}
	patched = !patched;
}

void init_hacks()
{
	memory_hacks[0] = { 0x005124E6, {0xeb, 0x14}, {0} };
	memory_hacks[1] = { 0x00512604, {0x90, 0x90}, {0} };
	memory_hacks[2] = { 0x00540A4C, {0xeb, 0x07}, {0} };
	memory_hacks[3] = { 0x0042D228, {0xeb, 0x1e}, {0} };
	memory_hacks[4] = { 0x0042D993, {0x90, 0x90}, {0} };

	for (int i = 0; i < total_memory_hacks; ++i)
	{
		ReadProcessMemory(aoe_handle, (void*)memory_hacks[i].patch_address, &memory_hacks[i].original_bytes, patch_size, 0); 
	}
}

DWORD WINAPI MainThread(LPVOID param)
{
	init_hacks();
	while (true)
	{
		if (GetAsyncKeyState(VK_F6) & 0x80000)
		{
			//MessageBoxA(NULL, "F6 pressed", "F6 Pressed", MB_OK);
			patch();
		}
		Sleep(100);
	}
	return 0;
}

bool WINAPI DllMain(HINSTANCE hModule, DWORD dwReason, LPVOID lpReverved)
{
	if (dwReason == DLL_PROCESS_ATTACH)
	{
		//MessageBoxA(NULL, "DLL injected!", "DLL injected!", MB_OK);
		CreateThread(0, 0, MainThread, hModule, 0, 0);
	}
	return true;
}