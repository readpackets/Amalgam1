#include "../SDK/SDK.h"

#include "../Features/Spectate/Spectate.h"
#include <algorithm>

MAKE_SIGNATURE(CClientState_ProcessFixAngle, "engine.dll", "40 53 48 83 EC ? F3 0F 10 42", 0x0);

MAKE_HOOK(CClientState_ProcessFixAngle, S::CClientState_ProcessFixAngle(), bool,
	CClientState* rcx, SVC_FixAngle* msg)
{
#ifdef DEBUG_HOOKS
	if (!Vars::Hooks::CClientState_ProcessFixAngle[DEFAULT_BIND])
		return CALL_ORIGINAL(rcx, msg);
#endif

	if (Vars::Visuals::Removals::AngleForcing.Value)
		return false;

	if (F::Spectate.m_iTarget != -1 && msg)
	{
		Vec3 safeAngle = msg->m_Angle;
		safeAngle.x = std::clamp(safeAngle.x, -89.0f, 89.0f);
		safeAngle.y = std::fmod(safeAngle.y + 180.0f, 360.0f) - 180.0f;
		safeAngle.z = 0.0f; 
		
		F::Spectate.m_vOldView = safeAngle;
	}
	return CALL_ORIGINAL(rcx, msg);
}