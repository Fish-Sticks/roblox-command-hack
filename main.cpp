// Written completely by Pranksterria
#include <Windows.h>
#include <thread>
#include <vector>
#include <string>
#include <sstream>
#include <iostream>

#define x(x) x - 0x400000 + reinterpret_cast<uintptr_t>(GetModuleHandle(NULL))
#define wrongway(cloud) cloud - reinterpret_cast<uintptr_t>(GetModuleHandle(NULL)) + 0x400000

using r_getdatamodel_typedef = int(__thiscall*)(int* a1, int* a2);
r_getdatamodel_typedef r_getdatamodel = (r_getdatamodel_typedef)(x(0x01077B30));

using r_getdatamodel2_typedef = int* (__cdecl*)();
r_getdatamodel2_typedef r_getdatamodel2 = (r_getdatamodel2_typedef)(x(0x010779E0));

using setgravity_typedef = void(__thiscall*)(int workspace, float newgravity);
setgravity_typedef setgravity = (setgravity_typedef)(x(0x0098B5D0));

using setspeed_typedef = void(__thiscall*)(int humanoid, float newspeed);
setspeed_typedef setspeed = (setspeed_typedef)(x(0xC0F050));

uintptr_t pointerwalk(uintptr_t base, std::vector<uintptr_t> addys)
{
	uintptr_t ret = base;
	for (int i = 0; i < addys.size(); ++i) {
		if (!ret) throw std::exception("Failed to find required data!");
		ret = *reinterpret_cast<uintptr_t*>(ret + addys[i]);
	}
	return ret;
}

std::vector<std::string> split(std::string& str)
{
	std::vector<std::string> myvector;
	std::stringstream mystream(str);
	while (!mystream.eof()) {
		std::string newstr;
		mystream >> newstr;
		myvector.push_back(newstr);
	}

	return myvector;
}

void main() {
	DWORD old;
	AllocConsole();
	VirtualProtect(FreeConsole, 1, PAGE_EXECUTE_READWRITE, &old);
	*reinterpret_cast<byte*>(FreeConsole) = 0xC3;
	VirtualProtect(FreeConsole, 1, old, &old);
	freopen("CONOUT$", "w", stdout);
	freopen("CONIN$", "r", stdin);
	int a[2]{ 0 };
	r_getdatamodel(r_getdatamodel2(), a);
	uintptr_t datamodel = a[0] + 0xC;
	uintptr_t workspace = pointerwalk(datamodel, { 0x30, 0xC, 0 });
	uintptr_t localplayer = pointerwalk(datamodel, { 0x30, 0xC, 0x60, 0x128 }); // character offset = 0x7C, humanoid = 0x30, 0xC, 0x78
	uintptr_t humanoid = pointerwalk(localplayer, { 0x7C, 0x30, 0xC, 0x78 });
	std::cout << "Welcome " << *reinterpret_cast<const char**>(localplayer + 0x28) << std::endl;
	std::cout << *reinterpret_cast<const char**>(humanoid + 0x28) << std::endl;
	printf_s("datamodel: 0x%08X\n", datamodel);
	std::string store;
	while (std::getline(std::cin, store)) {
		std::vector<std::string> args = split(store);
		if (args.size() != 0) {
			if (!strcmp(args[0].c_str(), "gravity")) {
				if (args.size() == 2) {
					try {
						setgravity(workspace, std::stof(args[1]));
					}
					catch (std::exception e) {
						std::cout << e.what() << std::endl;
					}
				}
				else {
					std::cout << "Usage: gravity 100\n";
				}
			}
			else if (!strcmp(args[0].c_str(), "speed")) {
				if (args.size() == 2) {
					try {
						humanoid = pointerwalk(localplayer, { 0x7C, 0x30, 0xC, 0x78 });
						setspeed(humanoid, std::stof(args[1]));
					}
					catch (std::exception e) {
						std::cout << e.what() << std::endl;
					}
				}
				else {
					std::cout << "Usage: speed 100\n";
				}
			}
			else if (!strcmp(args[0].c_str(), "jump")) {

			}
			else {
				std::cout << "Invalid command!\n";
			}
		}
	}
}

/*
	Internals of setgravity (excludes part that updates all existing instances):
	uintptr_t propertyclass = *(uintptr_t*)(workspace + 0x218);
	*(float*)(propertyclass + 0x12C) = num;
	for (int i = 0; i < *(uintptr_t*)(propertyclass + 0x134); ++i) {
		std::cout << "val: " << i << std::endl;
		uintptr_t v7 = *(uintptr_t*)(*(uintptr_t*)(propertyclass + 0x130) + 4 * i);
		uintptr_t v8 = v7 != 0 ? v7 + 0x50 : 0;
		*(uintptr_t*)(v8 + 0xA0) = num;
	}
	**(float**)(workspace + 0x3B8) = *(float*)(workspace + 0x3B8) + num;
*/

BOOL APIENTRY DllMain( HMODULE hModule,
                       DWORD  ul_reason_for_call,
                       LPVOID lpReserved
                     )
{
    switch (ul_reason_for_call)
    {
    case DLL_PROCESS_ATTACH:
		std::thread(main).detach();
    case DLL_THREAD_ATTACH:
    case DLL_THREAD_DETACH:
    case DLL_PROCESS_DETACH:
        break;
    }
    return TRUE;
}
