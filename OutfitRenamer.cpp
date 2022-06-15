#include <iostream>
#include <Windows.h>
#include <TlHelp32.h>
#include <string>
#include "color.hpp"

MODULEENTRY32 GetModule(const wchar_t* ModuleName, DWORD ProcessId)
{
	MODULEENTRY32 modEntry = { 0 };
	
	HANDLE SnapShot = CreateToolhelp32Snapshot(TH32CS_SNAPMODULE, ProcessId);
	if (SnapShot)
	{
		MODULEENTRY32W ModuleEntry = { 0 };
		ModuleEntry.dwSize = sizeof(ModuleEntry);

		if (Module32FirstW(SnapShot, &ModuleEntry))
		{
			do
			{
				if (wcscmp(ModuleEntry.szModule, ModuleName) == 0)
				{
					modEntry = ModuleEntry;
					break;
				}
			} while (Module32NextW(SnapShot, &ModuleEntry));
		}

		CloseHandle(SnapShot);
	}

	return modEntry;
}

int main()
{
	SetConsoleTitleA("Outfit Renamer 1.60 | Contact: Mystro#1450 | Platform: Waiting For Game");	
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
	MODULEENTRY32 module = GetModule(L"GTA5.exe", pid);
	HANDLE phandle = OpenProcess(PROCESS_ALL_ACCESS, FALSE, pid);

	DWORD Pointer;
	if (GetModule(L"steam_api64.dll", pid).modBaseAddr != NULL) {
		SetConsoleTitleA("Outfit Renamer 1.60 | Contact: Mystro#1450 | Platform: Steam");
		Pointer = 0x0254CEA0;
	}
	else if (GetModule(L"EOSSDK-Win64-Shipping.dll", pid).modBaseAddr != NULL) {
		SetConsoleTitleA("Outfit Renamer 1.60 | Contact: Mystro#1450 | Platform: Epic Games");
		Pointer = 0x02548D30;
	}
	else {
		SetConsoleTitleA("Outfit Renamer 1.60 | Contact: Mystro#1450 | Platform: Not Supported (Using Steam Pointers)");
		Pointer = 0x0254CEA0;
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
			std::cout << i+1 << " | "  << name  << std::endl;
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