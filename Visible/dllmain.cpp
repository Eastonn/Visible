#pragma once

#include "dllmain.h"

#include "core/interfaces.h"
#include "core/proxy.h"

void AddLog(const char* format, ...)
{
	char buffer[1024] = {0};
	va_list args;

	va_start(args, format);
	vsprintf_s(buffer, format, args);
	printf(buffer);
	CSX::Log::AddLog(buffer);
	va_end(args);
}

constexpr auto kMaxEntitiesInList = 512;
constexpr auto kMaxEntityLists = 64;
constexpr auto kMaxTotalEntities = kMaxEntitiesInList * kMaxEntityLists;

DWORD WINAPI OnDllAttach(const PVOID base)
{
#ifdef _DEBUG
	AllocConsole();

	freopen_s(reinterpret_cast<FILE**>(stdin), "CONIN$", "r", stdin);
	freopen_s(reinterpret_cast<FILE**>(stdout), "CONOUT$", "w", stdout);

	SetConsoleTitleA("Visible - Debug console");
#endif

	AddLog("[+] Cheat Injected\n");
	
	/*GetModules();*/

	AddLog("[+] Got Modules \n");

	//entity_vmt = new VMT(entity);
	//entity_vmt->HookVMT(OnAddEntity, 14);
	//entity_vmt->HookVMT(OnRemoveEntity, 15);
	//entity_vmt->ApplyVMT(entity);

	//panel_vmt = new VMT(panel);
	//panel_vmt->HookVMT(PaintTraverse, 55);
	//panel_vmt->ApplyVMT(panel);

	//particle_mgr_vmt = new VMT(particle_mgr);
	//particle_mgr_vmt->HookVMT(CreateParticleCollection, 17);
	//particle_mgr_vmt->ApplyVMT(particle_mgr);

	AddLog("[+] Initialized Hooks \n");

	while (true)
	{
		if (GetKeyState(VK_DELETE) & 1)
		{
			break;
		}
		using namespace chrono_literals;
		std::this_thread::sleep_for(1s);
	}

	FreeLibraryAndExitThread(static_cast<HMODULE>(base), 1);
}

VOID WINAPI OnDllDetach()
{
#ifdef _DEBUG
	fclose(static_cast<FILE*>(stdin));
	fclose(static_cast<FILE*>(stdout));

	FreeConsole();

	delete entity_vmt;
	delete panel_vmt;
	delete particle_mgr_vmt;
#endif
}

BOOL WINAPI DllMain(const HMODULE h_module, const DWORD dw_reason, const LPVOID lp_reserved)
{
	if (dw_reason == DLL_PROCESS_ATTACH)
	{
		DisableThreadLibraryCalls(h_module);

		char lp_filename[MAX_PATH] = {0};
		GetModuleFileNameA(h_module, lp_filename, MAX_PATH);

		string base_dir = lp_filename;
		base_dir = base_dir.substr(0, base_dir.find_last_of("\\/"));

		memset(lp_filename, 0, MAX_PATH);

		base_dir += '\\';

		const auto log_file = base_dir + "visible.log";

		CSX::Log::SetLogFile(log_file.c_str());

		auto* const h = CreateThread(nullptr, NULL, OnDllAttach, h_module, NULL, nullptr);

		if (!h)
			throw std::exception("Error while creating thread.");

		CloseHandle(h);
	}
	else if (dw_reason == DLL_PROCESS_DETACH)
	{
		if (!lp_reserved)
		{
			using namespace chrono_literals;
			std::this_thread::sleep_for(1s);
		}

		OnDllDetach();
	}
	return TRUE;
}
