#pragma once

#include "dllmain.h"
#include "core/cheat.h"

bool __stdcall DllMain(HINSTANCE hinstDLL, DWORD fdwReason, LPVOID lpReserved) {
	if (fdwReason != DLL_PROCESS_ATTACH) 
		return TRUE;

	return cheat::startup(hinstDLL);
}
