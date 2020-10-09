#pragma once

#include "sdk/IVPanel.h"
#include "sdk/CGameEntitySystem.h"
#include "sdk/CParticleSystemMgr.h"
#include "../vmt.h"

inline void* get_interface(const char* dllname, const char* interfacename)
{
	const auto module_handle = GetModuleHandle(dllname);
	const auto adress = GetProcAddress(module_handle, "CreateInterface");
	CreateInterfaceFn create_interface = reinterpret_cast<CreateInterfaceFn>(adress);

	auto return_code = 0;
	auto* module = create_interface(interfacename, &return_code);

	return module;
}

IVPanel* panel;
CParticleSystemMgr* particle_mgr;
CGameEntitySystem* entity;
std::vector<CEntityInstance*> heroes;

VMT* entity_vmt;
VMT* panel_vmt;
VMT* particle_mgr_vmt;
VMT* particle_collection_vmt;

void GetModules()
{
	// SET DOTA 2 LAUNCH OPTION -console
	panel = static_cast<IVPanel*>(get_interface("vgui2.dll", "VGUI_Panel010"));
	particle_mgr = static_cast<CParticleSystemMgr*>(get_interface("particles.dll", "ParticleSystemMgr003"));

	auto* const client = get_interface("client.dll", "Source2Client002");
	auto* vmt_slot = *(uintptr_t**)client + 25; //25th function in Source2Client vtable
	const auto address_start = *vmt_slot + 3;
	entity = *reinterpret_cast<CGameEntitySystem**>((address_start + *(uint32_t*)(address_start) + 4));
}
