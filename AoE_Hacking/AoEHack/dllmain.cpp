// dllmain.cpp : Defines the entry point for the DLL application.
#include "pch.h"
#include "binary_patch.h"
#include "pointer_patch.h"

DWORD WINAPI MainThread(LPVOID param)
{
    init_binary_hack();
    while (true)
    {
        if (GetAsyncKeyState(VK_F6) & 0x1)
        {
            toggle_map_pointers();
        }
        else if (GetAsyncKeyState(VK_F7) & 0x1)
        {
            increase_resources();
        }
        else if (GetAsyncKeyState(VK_F8) & 0x1)
        {
            toggle_map_patch();
        }
        Sleep(5);

    }
    return 0;
}



//BOOL APIENTRY DllMain( HMODULE hModule,
//                       DWORD  ul_reason_for_call,
//                       LPVOID lpReserved
//                     )
//{
//    switch (ul_reason_for_call)
//    {
//    case DLL_PROCESS_ATTACH:
//    case DLL_THREAD_ATTACH:
//    case DLL_THREAD_DETACH:
//    case DLL_PROCESS_DETACH:
//        break;
//    }
//    return TRUE;
//}


bool WINAPI DllMain(HINSTANCE hModule, DWORD dwReason, LPVOID lpReverved)
{
	if (dwReason == DLL_PROCESS_ATTACH)
	{
		//MessageBoxA(NULL, "DLL injected!", "DLL injected!", MB_OK);
		CreateThread(0, 0, MainThread, hModule, 0, 0);
	}
	return true;
}
