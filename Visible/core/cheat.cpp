#include "cheat.h"
#include "interfaces.h"
#include "proxy.h"

DWORD initial(HANDLE)
{
	cheat::console::init_logger();
	cheat::memory::get_modules();
	cheat::hooks::init_hooks();

	while (true)
	{
		if (GetKeyState(VK_DELETE) & 1)
		{
			break;
		}
 
		using namespace chrono_literals;
		std::this_thread::sleep_for(1s);
	}

	FreeLibraryAndExitThread(static_cast<HMODULE>(cheat::dll_handle), 1);
}

void cheat::startup(HINSTANCE handle)
{
	dll_handle = handle;
	CreateThread(nullptr, 0, initial, 0, 0, nullptr);
}