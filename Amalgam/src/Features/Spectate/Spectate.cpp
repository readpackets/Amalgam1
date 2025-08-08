#include "Spectate.h"
#include <algorithm>

void CSpectate::NetUpdateEnd(CTFPlayer* pLocal)
{
	if (!pLocal)
		return;

	m_iTarget = m_iIntendedTarget;
	CTFPlayer* pEntity = nullptr;
	if (m_iTarget != -1)
	{
		int entityIndex = I::EngineClient->GetPlayerForUserID(m_iTarget);
		if (entityIndex <= 0)
		{
			m_iTarget = m_iIntendedTarget = -1;
			return;
		}
		
		IClientEntity* clientEntity = I::ClientEntityList->GetClientEntity(entityIndex);
		if (!clientEntity)
		{
			m_iTarget = m_iIntendedTarget = -1;
			return;
		}
		
		pEntity = clientEntity->As<CTFPlayer>();
		if (!pEntity || pEntity == pLocal)
		{
			m_iTarget = m_iIntendedTarget = -1;
		}
	}
	
	if (m_iTarget == -1)
	{
		if (pLocal->IsAlive() && pLocal->m_hObserverTarget())
		{
			pLocal->m_vecViewOffset() = pLocal->GetViewOffset();
			pLocal->m_iObserverMode() = OBS_MODE_NONE;
			pLocal->m_hObserverTarget().Set(nullptr);
		}
		return;
	}

	m_pOriginalTarget = pLocal->m_hObserverTarget().Get();
	m_iOriginalMode = pLocal->m_iObserverMode();
	if (!pEntity)
		return;

	int observerMode = OBS_MODE_NONE;
	if (pEntity->m_hObserverTarget())
	{
		observerMode = pEntity->m_iObserverMode();
	}
	
	switch (observerMode)
	{
	case OBS_MODE_FIRSTPERSON:
	case OBS_MODE_THIRDPERSON:
		if (pEntity->m_hObserverTarget())
		{
			pLocal->m_hObserverTarget().Set(pEntity->m_hObserverTarget());
		}
		else
		{
			pLocal->m_hObserverTarget().Set(pEntity);
		}
		break;
	default:
		pLocal->m_hObserverTarget().Set(pEntity);
	}
	
	pLocal->m_iObserverMode() = Vars::Visuals::Thirdperson::Enabled.Value ? OBS_MODE_THIRDPERSON : OBS_MODE_FIRSTPERSON;
	pLocal->m_vecViewOffset() = pEntity->GetViewOffset();
	Vars::Visuals::Thirdperson::Enabled.Value ? I::Input->CAM_ToThirdPerson() : I::Input->CAM_ToFirstPerson();

	m_pTargetTarget = pLocal->m_hObserverTarget().Get();
	m_iTargetMode = pLocal->m_iObserverMode();
}

void CSpectate::NetUpdateStart(CTFPlayer* pLocal)
{
	if (!pLocal || m_iTarget == -1)
		return;

	if (m_pOriginalTarget)
	{
		pLocal->m_hObserverTarget().Set(m_pOriginalTarget);
		pLocal->m_iObserverMode() = m_iOriginalMode;
	}
	else
	{
		pLocal->m_hObserverTarget().Set(nullptr);
		pLocal->m_iObserverMode() = OBS_MODE_NONE;
	}
}

void CSpectate::CreateMove(CTFPlayer* pLocal, CUserCmd* pCmd)
{
	if (!pLocal || !pCmd)
		return;

	int iButtons = pCmd->buttons & ~IN_SCORE;
	if (iButtons)
		m_iIntendedTarget = -1;

	static bool bStaticView = false;
	const bool bLastView = bStaticView;
	const bool bCurrView = bStaticView = m_iTarget != -1;
	if (!bCurrView)
	{
		if (bLastView && I::EngineClient)
		{
			Vec3 safeAngles = m_vOldView;
			safeAngles.x = std::clamp(safeAngles.x, -89.0f, 89.0f);
			safeAngles.y = std::fmod(safeAngles.y + 180.0f, 360.0f) - 180.0f;
			safeAngles.z = 0.0f; 
			
			I::EngineClient->SetViewAngles(safeAngles);
		}
		m_vOldView = pCmd->viewangles;
	}
	else
		pCmd->viewangles = m_vOldView;
}

void CSpectate::SetTarget(int iTarget)
{
	if (iTarget != -1)
	{
		int entityIndex = I::EngineClient->GetPlayerForUserID(iTarget);
		if (entityIndex <= 0)
		{
			m_iIntendedTarget = -1;
			return;
		}
		
		IClientEntity* clientEntity = I::ClientEntityList->GetClientEntity(entityIndex);
		if (!clientEntity || !clientEntity->As<CTFPlayer>())
		{
			m_iIntendedTarget = -1;
			return;
		}
	}
	
	m_iIntendedTarget = m_iIntendedTarget == iTarget ? -1 : iTarget;
}