#pragma once

#include "sdk/IVPanel.h"
#include "sdk/CGameEntitySystem.h"
#include "sdk/CParticleSystemMgr.h"
#include "sdk/IEngineClient.h"

#define IS_IN_RANGE( value, max, min ) ( value >= max && value <= min )
#define GET_BITS( value ) ( IS_IN_RANGE( value, '0', '9' ) ? ( value - '0' ) : ( ( value & ( ~0x20 ) ) - 'A' + 0xA ) )
#define GET_BYTE( value ) ( GET_BITS( value[0] ) << 4 | GET_BITS( value[1] ) )

inline std::uintptr_t scan(const std::string& image_name, const std::string& signature)
{
	auto image = GetModuleHandleA(image_name.c_str());

	if (!image)
	{
#ifdef DEBUG
		Win32::Error("GetModuleHandleA failed %s", __FUNCTION__);
#endif // DEBUG
		return 0u;
	}

	auto image_base = (std::uintptr_t)(image);
	auto image_dos_hdr = (IMAGE_DOS_HEADER*)(image_base);

	if (image_dos_hdr->e_magic != IMAGE_DOS_SIGNATURE)
	{
#ifdef DEBUG
		Win32::Error("IMAGE_DOS_HEADER::e_magic is invalid %s", __FUNCTION__);
#endif // DEBUG
		return 0u;
	}

	auto image_nt_hdrs = (IMAGE_NT_HEADERS*)(image_base + image_dos_hdr->e_lfanew);

	if (image_nt_hdrs->Signature != IMAGE_NT_SIGNATURE)
	{
#ifdef DEBUG
		Win32::Error("IMAGE_NT_HEADERS::Signature is invalid %s", __FUNCTION__);
#endif // DEBUG
		return 0u;
	}

	auto scan_begin = (std::uint8_t*)(image_base);
	auto scan_end = (std::uint8_t*)(image_base + image_nt_hdrs->OptionalHeader.SizeOfImage);

	std::uint8_t* scan_result = nullptr;
	std::uint8_t* scan_data = (std::uint8_t*)(signature.c_str());

	for (auto current = scan_begin; current < scan_end; current++)
	{
		if (*(std::uint8_t*)scan_data == '\?' || *current == GET_BYTE(scan_data))
		{
			if (!scan_result)
				scan_result = current;

			if (!scan_data[2])
				return (std::uintptr_t)(scan_result);

			scan_data += (*(std::uint16_t*)scan_data == '\?\?' || *(std::uint8_t*)scan_data != '\?') ? 3 : 2;

			if (!*scan_data)
				return (std::uintptr_t)(scan_result);
		}
		else if (scan_result)
		{
			current = scan_result;
			scan_data = (std::uint8_t*)(signature.c_str());
			scan_result = nullptr;
		}
	}

#ifdef DEBUG
	Win32::Warning("Signature '%s' in module '%s' not found %s", signature.c_str(), image_name.c_str(), __FUNCTION__);
#endif // DEBUG

	return 0u;
}

inline uintptr_t get_absolute_address(uintptr_t instruction_ptr, int offset, int size)
{
	return instruction_ptr + *reinterpret_cast<int32_t*>(instruction_ptr + offset) + size;
};

inline void* get_interface(const char* dll_name, const char* interface_name)
{
	const auto module_handle = GetModuleHandle(dll_name);
	if (!module_handle)
		return nullptr;

	const auto address = GetProcAddress(module_handle, "CreateInterface");

	if (!address)
		return nullptr;

	auto return_code = 0;
	const auto create_interface = reinterpret_cast<CreateInterfaceFn>(address);

	return create_interface(interface_name, &return_code);
}

// Old Deprecated Function
inline bool find_game_entity_sytem()
{
	// CPrediction::ReinitPredictables()
	// Look for string "ReinitPredictables".
	// The start of this function contains both a pointer to CGameEntitySystem and a call to CGameEntitySystem::GetHighestEntityIndex();
	// 55                      push    rbp
	// 48 89 E5                mov     rbp, rsp
	// 41 56                   push    r14
	// 4C 8B 35 F3 A8 2A 02    mov     r14, cs:_g_pGameEntitySystem
	// 41 55                   push    r13
	// 49 89 FD                mov     r13, rdi
	// 41 54                   push    r12
	// 53                      push    rbx
	// 31 DB                   xor     ebx, ebx
	// C6 87 D0 00 00 00 01    mov     byte ptr [rdi+0D0h], 1
	// 49 8B 3E                mov     rdi, [r14]
	// E8 EA 4B E2 FF          call    CGameEntitySystem__GetHighestEntityIndex

	uintptr_t reinitPredictables = scan("client.dll", "55 48 89 E5 41 56 4C 8B 35 ?? ?? ?? ?? 41 55 49 89 FD 41 54 53 31");

	if (!reinitPredictables) {
		return false;
	}

	uintptr_t getHighestEntIndexFnAddr = get_absolute_address(reinitPredictables + 33, 1, 5);

	// CGameEntitySystem::GetHighestEntityIndex()
	// We get the class offset for the highest Ent Index
	// 55                      push    rbp
	// 8B 87 60 A0 01 00       mov     eax, dword ptr ds:stru_1A060.r_offset[rdi]
	// 48 89 E5                mov     rbp, rsp
	// 5D                      pop     rbp

	auto highestEntIndexOffset = *reinterpret_cast<uint32_t*>(getHighestEntIndexFnAddr + 3);
	auto entitySystem = **reinterpret_cast<CGameEntitySystem***>(get_absolute_address(reinitPredictables + 6, 3, 7));

	return true;
}

VMT* entity_vmt;
VMT* panel_vmt;
VMT* particle_mgr_vmt;
VMT* particle_collection_vmt;

IVPanel* panel;
CParticleSystemMgr* particle_mgr;
CGameEntitySystem* entity;
IEngineClient* engine;

std::vector<CEntityInstance*> heroes;

namespace cheat::memory
{
	inline void get_modules()
	{
		panel = static_cast<IVPanel*>(get_interface("vgui2.dll", "VGUI_Panel010")); // Set game launch option -console 'cause other way game won't load this interface
		engine = static_cast<IEngineClient*>(get_interface("engine2.dll", "Source2EngineToClient001"));
		particle_mgr = static_cast<CParticleSystemMgr*>(get_interface("particles.dll", "ParticleSystemMgr003"));

		auto* const client = get_interface("client.dll", "Source2Client002");
		auto* vmt_slot = *(uintptr_t**)client + 25; //25th function in Source2Client vtable
		const auto address_start = *vmt_slot + 3;
		entity = *reinterpret_cast<CGameEntitySystem**>((address_start + *(uint32_t*)(address_start)+4));
	}
}

namespace cheat::hooks
{
	inline void init_hooks()
	{
		entity_vmt = new VMT(entity);
		entity_vmt->HookVMT(OnAddEntity, 14);
		entity_vmt->HookVMT(OnRemoveEntity, 15);
		entity_vmt->ApplyVMT(entity);

		panel_vmt = new VMT(panel);
		panel_vmt->HookVMT(PaintTraverse, 55);
		panel_vmt->ApplyVMT(panel);

		particle_mgr_vmt = new VMT(particle_mgr);
		particle_mgr_vmt->HookVMT(CreateParticleCollection, 17);
		particle_mgr_vmt->ApplyVMT(particle_mgr);
	}
}