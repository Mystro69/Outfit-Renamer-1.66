#define _CRT_SECURE_NO_WARNINGS
#include <iostream>
#include <Windows.h>
#include <TlHelp32.h>
#include <string>
#include <vector>
#include <psapi.h>
#include "color.h"

MODULEENTRY32 GetModule(char* ModuleName, DWORD ProcessId)
{
	MODULEENTRY32 modEntry = { 0 };
	MODULEENTRY32 entry;
	entry.dwSize = sizeof(MODULEENTRY32);
	DWORD baseAddress = 0;

	HANDLE snapshot = CreateToolhelp32Snapshot(TH32CS_SNAPMODULE, ProcessId);

	if (Module32First(snapshot, &entry)) {
		do {
			if (_stricmp(entry.szModule, ModuleName) == 0) {
				modEntry = entry;
				break;
			}
		} while (Module32Next(snapshot, &entry));
	}

	CloseHandle(snapshot);
	return modEntry;
}

std::vector<uintptr_t> AOBScan(HANDLE hProcess, const char* pattern, const char* mask, uintptr_t startAddr, uintptr_t endAddr)
{
	std::vector<uintptr_t> results;
	MEMORY_BASIC_INFORMATION mbi;
	uintptr_t baseAddress = startAddr;
	while (baseAddress < endAddr)
	{
		if (VirtualQueryEx(hProcess, (LPCVOID)baseAddress, &mbi, sizeof(mbi)))
		{
			if ((mbi.State == MEM_COMMIT) && (mbi.Protect != PAGE_NOACCESS))
			{
				char* buffer = new char[mbi.RegionSize];
				SIZE_T bytesRead;
				if (ReadProcessMemory(hProcess, mbi.BaseAddress, buffer, mbi.RegionSize, &bytesRead))
				{
					for (uintptr_t i = 0; i < (bytesRead - strlen(mask)); i++)
					{
						bool found = true;
						for (uintptr_t j = 0; j < strlen(mask); j++)
						{
							if ((mask[j] != '?') && (pattern[j] != buffer[i + j]))
							{
								found = false;
								break;
							}
						}
						if (found)
						{
							results.push_back((uintptr_t)mbi.BaseAddress + i);
						}
					}
				}
				delete[] buffer;
			}
		}
		baseAddress += mbi.RegionSize;
	}
	return results;
}

int main()
{
	SetConsoleTitleA("Outfit Renamer 1.66 | Contact: Mystro#1450 | Platform: Waiting For Game");
	std::cout << "Waiting For Game  ";
	std::cout << '-' << std::flush;
	while (!FindWindowA(NULL, "Grand Theft Auto V")) {
		Sleep(100);
		std::cout << "\b\\" << std::flush;
		Sleep(100);
		std::cout << "\b|" << std::flush;
		Sleep(100);
		std::cout << "\b/" << std::flush;
		Sleep(100);
		std::cout << "\b-" << std::flush;
	}
	system("cls");

	HWND hwnd_AC = FindWindowA(NULL, "Grand Theft Auto V");
	DWORD pid = NULL;
	GetWindowThreadProcessId(hwnd_AC, &pid);
	MODULEENTRY32 module = GetModule((char*)"GTA5.exe", pid);
	HANDLE phandle = OpenProcess(PROCESS_ALL_ACCESS, FALSE, pid);

	if (GetModule((char*)"steam_api64.dll", pid).modBaseAddr != NULL) {
		SetConsoleTitleA("Outfit Renamer 1.66 | Contact: Mystro#1450 | Platform: Steam");
	}
	else if (GetModule((char*)"EOSSDK-Win64-Shipping.dll", pid).modBaseAddr != NULL) {
		SetConsoleTitleA("Outfit Renamer 1.66 | Contact: Mystro#1450 | Platform: EG");
	}
	else {
		SetConsoleTitleA("Outfit Renamer 1.66 | Contact: Mystro#1450 | Platform: SC");
	}

	std::vector<uintptr_t> results = AOBScan(phandle, "\x10\x5D\x00\x00\x00\x00\x00\x00\x60\x5D", "xx????xxxx", (uintptr_t)module.modBaseAddr, (uintptr_t)module.modBaseAddr + module.modBaseSize);
	DWORD Pointer;

	// Print the results
	for (uintptr_t address : results)
	{
		std::cout << "Found pattern at address: " << std::hex << address << std::dec << std::endl;
		Pointer = (DWORD)(address - (DWORD_PTR)module.modBaseAddr);
		std::cout << "Found pointer: " << std::hex << Pointer << std::dec << std::endl;
	}

	DWORD outfitOffets[30] = {
		0x528, // 1
		0x568, // 2
		0x5A8, // 3
		0x5E8, // 4
		0x628, // 5
		0x668, // 6
		0x6A8, // 7
		0x6E8, // 8
		0x728, // 9
		0x768, // 10
		0x7A8, // 11
		0x7E8, // 12
		0x828, // 13
		0x868, // 14
		0x8A8, // 15
		0x8E8, // 16
		0x928, // 17
		0x968, // 18
		0x9A8, // 19
		0x9E8, // 20

	};

	for (int i = 0; i < 20; i++) {
		unsigned long long result;
		char name[] = { 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16, 17, 18, 19, 20 };
		ReadProcessMemory(phandle, (void*)((unsigned long long)module.modBaseAddr + Pointer), &result, sizeof(result), 0);
		ReadProcessMemory(phandle, (void*)((unsigned long long)result + outfitOffets[i]), &name, strlen(name) + 1, 0);

		if (strlen(name) > 0)
			std::cout << i + 1 << " | " << name << std::endl;
		else
		{
			std::cout << colors::red << i + 1 << " | " << name << colors::reset << "\n";
		}

	}

	int element;
	std::cout << "Which one do you want to edit ?: ";
	std::cin >> element;

	std::cout << "What do you want to change it to ?: ";
	std::string s;
	std::cin.ignore();
	std::getline(std::cin, s, '\n');

	unsigned long long result;
	ReadProcessMemory(phandle, (void*)((unsigned long long)module.modBaseAddr + Pointer), &result, sizeof(result), 0);
	WriteProcessMemory(phandle, (void*)(result + outfitOffets[element - 1]), s.c_str(), size(s) + 1, 0);

	system("cls");
	main();
}
