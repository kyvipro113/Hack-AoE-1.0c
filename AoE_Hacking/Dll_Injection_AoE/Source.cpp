#include <Windows.h>
#include <TlHelp32.h>
#include <iostream>

using namespace std;

#define CHUNK_SIZE 0x1
#define MAX_ADDRESS 0x7ffffff

int getProcID(const wchar_t* target)
{
	DWORD pID = 0;
	PROCESSENTRY32 pe32;
	pe32.dwSize = sizeof(PROCESSENTRY32);
	HANDLE hSnapshot = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
	do
	{
		if (wcscmp(pe32.szExeFile, target) == 0)
		{
			CloseHandle(hSnapshot);
			pID = pe32.th32ProcessID;
			break;
		}
	} while (Process32Next(hSnapshot, &pe32));
	CloseHandle(hSnapshot);
	return pID;
}

DWORD getProcessId(LPCTSTR ProcessName) // non-conflicting function name
{
	PROCESSENTRY32 pt;
	HANDLE hsnap = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
	pt.dwSize = sizeof(PROCESSENTRY32);
	if (Process32First(hsnap, &pt)) { // must call this first
		do {
			if (!lstrcmpi(pt.szExeFile, ProcessName)) {
				CloseHandle(hsnap);
				return pt.th32ProcessID;
			}
		} while (Process32Next(hsnap, &pt));
	}
	CloseHandle(hsnap); // close handle on failure
	return 0;
}

uintptr_t getModuleBaseAddress(DWORD procId, const wchar_t* modName)
{
	uintptr_t modBaseAddr = 0;
	HANDLE hSnap = CreateToolhelp32Snapshot(TH32CS_SNAPMODULE | TH32CS_SNAPMODULE32, procId);
	if (hSnap != INVALID_HANDLE_VALUE)
	{
		MODULEENTRY32 modEntry;
		modEntry.dwSize = sizeof(modEntry);
		if (Module32First(hSnap, &modEntry))
		{
			do
			{
				if (!_wcsicmp(modEntry.szModule, modName))
				{
					modBaseAddr = (uintptr_t)modEntry.modBaseAddr;
					break;
				}
			} while (Module32Next(hSnap, &modEntry));
		}
	}
	CloseHandle(hSnap);
	return modBaseAddr;
}

int main(int argc, char* argv[])
{
	//const wchar_t* process = L"Empiresx.exe";
	//int pID = getProcID(process);
	DWORD pID = getProcessId(TEXT("EMPIRESX.EXE"));
	//DWORD pID = getProcessId(TEXT("Empiresx.exe"));
	cout << pID << endl;

	uintptr_t baseAddr = getModuleBaseAddress(pID, L"EMPIRESX.EXE");
	cout << "Base Address: " << baseAddr << endl;

	// char dll[] = "AoE_Hacking.dll";
	char dll[] = "AoEHack.dll";
	cout << dll << endl;
	char dllPath[MAX_PATH] = { 0 };
	GetFullPathNameA(dll, MAX_PATH, dllPath, NULL);
	cout << dllPath << endl;

	HANDLE hProcess = OpenProcess(PROCESS_CREATE_THREAD | PROCESS_QUERY_INFORMATION | PROCESS_VM_OPERATION | PROCESS_VM_WRITE | PROCESS_VM_READ, FALSE, pID);
	LPVOID pszLibFileRemote = VirtualAllocEx(hProcess, NULL, strlen(dllPath) + 1, MEM_COMMIT | MEM_RESERVE, PAGE_READWRITE);
	WriteProcessMemory(hProcess, pszLibFileRemote, dllPath, strlen(dllPath) + 1, NULL);
	HANDLE handleThread = CreateRemoteThread(hProcess, NULL, NULL, (LPTHREAD_START_ROUTINE)LoadLibraryA, pszLibFileRemote, NULL, NULL);

	WaitForSingleObject(handleThread, INFINITE);
	CloseHandle(handleThread);
	VirtualFreeEx(hProcess, dllPath, 0, MEM_RELEASE);
	CloseHandle(hProcess);
	cout << "Dll Injection Success" << endl;
	try
	{
		char buffer[CHUNK_SIZE];
		float value = 200;
		HANDLE hProcess = OpenProcess(PROCESS_CREATE_THREAD | PROCESS_QUERY_INFORMATION | PROCESS_VM_OPERATION | PROCESS_VM_WRITE | PROCESS_VM_READ, FALSE, pID);
		for (DWORD i = 0x000000; i < MAX_ADDRESS; i += CHUNK_SIZE)
		{
			ReadProcessMemory(hProcess, (void*)i, &buffer, sizeof(CHUNK_SIZE), 0);
			for (int j = 0; j < CHUNK_SIZE; j++)
			{
				if ((float)buffer[j] == value)
				{
					cout << buffer[j] << endl;
				}
			}
		}

	}
	catch (const std::exception&)
	{
		cout << "Error" << endl;
	}
	system("pause");
	return 0;
}