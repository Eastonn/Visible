#pragma once
#include "interfaces.h"

// CGameEntitySystem
CEntityInstance* OnRemoveEntity(CGameEntitySystem*, CEntityInstance*, EntityHandle);
CEntityInstance* OnAddEntity(CGameEntitySystem*, CEntityInstance*, EntityHandle);

// panel
void PaintTraverse(IVPanel*, IVGuiPaintSurface*, VPANEL, bool, bool);

// ParticleSystemMgr
void* CreateParticleCollection(CParticleSystemMgr* thisptr,
                               CWeakHandle_InfoForResourceTypeIParticleSystemDefinition* info, void* unk,
                               IParticleSystemQuery* query, bool bUnk, float fUnk, int iUnk);

// CParticleCollection
void SetRenderingEnabled(void*, bool);

CEntityInstance* OnAddEntity(CGameEntitySystem* ecx, CEntityInstance* ptr, EntityHandle index)
{
	auto ret = entity_vmt->GetOriginalMethod(OnAddEntity)(ecx, ptr, index);
	const char* typeName = ptr->SchemaDynamicBinding()->bindingName;

	if (strstr(typeName, "DOTA_Unit_Hero"))
	{
		auto alreadyExists = false;
		for (auto hero : heroes)
		{
			if (typeName == hero->SchemaDynamicBinding()->bindingName) //prevent adding illusions by checking for duplicates
			{
				alreadyExists = true;
				break;
			}
		}

		if (!alreadyExists)
		{
			heroes.push_back(ptr);
		}
	}

	return ret;
}

CEntityInstance* OnRemoveEntity(CGameEntitySystem* ecx, CEntityInstance* ptr, EntityHandle index)
{
	const char* typeName = ptr->SchemaDynamicBinding()->bindingName;

	if (strstr(typeName, "DOTA_Unit_Hero"))
	{
		for (size_t i = heroes.size(); i-- > 0;)
		{
			if (heroes[i] == ptr)
			{
				heroes.erase(heroes.begin() + i);
				break;
			}
		}
	}

	return entity_vmt->GetOriginalMethod(OnRemoveEntity)(ecx, ptr, index);
}

void* CreateParticleCollection(CParticleSystemMgr* thisptr,
                               CWeakHandle_InfoForResourceTypeIParticleSystemDefinition* info, void* unk,
                               IParticleSystemQuery* query, bool bUnk, float fUnk, int iUnk)
{
	void* particleCollection = particle_mgr_vmt->GetOriginalMethod(CreateParticleCollection)(
		thisptr, info, unk, query, bUnk, fUnk, iUnk);

	//check if we have a vtable
	if (particle_collection_vmt) //we've already created a vtable
	{
		//change pointer in particles to point to our updated vtable
		particle_collection_vmt->ApplyVMT(particleCollection);
	}
	else
	{
		//no vtable, so create one
		particle_collection_vmt = new VMT(particleCollection);
		particle_collection_vmt->HookVMT(SetRenderingEnabled, 88);
		particle_collection_vmt->ApplyVMT(particleCollection);
	}

	return particleCollection;
}

void PaintTraverse(IVPanel* ecx, IVGuiPaintSurface* surface, VPANEL vpanel, bool force_repaint, bool allow_force)
{
	panel_vmt->GetOriginalMethod(PaintTraverse)(ecx, surface, vpanel, force_repaint, allow_force);
	
	/*
	surface->PushMakeCurrent(vpanel, false
	surface->DrawSetColor(0, 255, 0, 255);
	surface->DrawLine(0, 1080 / 2, 1920, 1080 / 2
	surface->PopMakeCurrent(vpanel);
	*/
	
	/*for (const auto hero : heroes)
	{
		if (hero->CanBeSeenByTeam(DOTA_TEAM_RADIANT) && hero->CanBeSeenByTeam(DOTA_TEAM_DIRE))
			hero->DrawEntityDebugOverlays(ABSBOX);
	}*/
}

void SetRenderingEnabled(void* thisptr, bool state)
{
	particle_collection_vmt->GetOriginalMethod(SetRenderingEnabled)(thisptr, true);
}
