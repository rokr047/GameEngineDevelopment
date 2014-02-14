#include "../Lua/LuaEnvironment.h"
#include "PrimeEngine/Physics/CarPhysicsManager.h"
#include "PrimeEngine/Physics/DebugLines/DebugLines.h"
#include "PrimeEngine/Math/Vector3.h"
#include "PrimeEngine/Math/Vector4.h"
#include "PrimeEngine/Math/Matrix4x4.h"
#include "PrimeEngine/Scene/DebugRenderer.h"
#include "CharacterControl/Characters/SoldierNPCAnimationSM.h"
#include "PrimeEngine/Physics/CarPhysicsComponent.h"
#include <assert.h>

namespace PE {
namespace Components {

PE_IMPLEMENT_CLASS1(CarPhysicsManager, CarPhysicsComponent);

Handle CarPhysicsManager::s_hTitleInstance;
Handle CarPhysicsManager::s_hInstance;
Handle CarPhysicsManager::s_hCurInstance;

void CarPhysicsManager::Construct(PE::GameContext &context, PE::MemoryArena arena)
{
	PE::Handle h("CAR_PHYSICS_MANAGER", sizeof(CarPhysicsManager));
	CarPhysicsManager *pCarPhysicsManager = new(h) CarPhysicsManager(context, arena, h);
	pCarPhysicsManager->addDefaultComponents();
	SetInstance(h);
	
	s_hTitleInstance = Handle("CAR_PHYSICS_MANAGER", sizeof(CarPhysicsManager));
	CarPhysicsManager *pTitlePhysicsManager = new(s_hTitleInstance) CarPhysicsManager(context, arena, h);
	pTitlePhysicsManager->addDefaultComponents();
	
	SetTitleAsCurrent();
}

void CarPhysicsManager::addDefaultComponents()
{
	CarPhysicsComponent::addDefaultComponents();
}

void CarPhysicsManager::startComputeVelocity(PrimitiveTypes::Float32 frameTime)
{
	//We will get all the instance of CarPhysicsComponent and will compute velocity for each of them.
	PE::Handle *pHC = m_components.getFirstPtr();

	for (PrimitiveTypes::UInt32 i = 0; i < m_components.m_size; i++, pHC++) // fast array traversal (increasing ptr)
	{
		Component *pC = (*pHC).getObject<Component>();

		if (pC->isInstanceOf<CarPhysicsComponent>())
		{
			CarPhysicsComponent *pPC = (CarPhysicsComponent *)(pC);

			pPC->computeVelocity(frameTime);
		}
	}
}

CarPhysicsComponent* CarPhysicsManager::getPlayerCar()
{
	PE::Handle *pHC = m_components.getFirstPtr();

	for (PrimitiveTypes::UInt32 i = 0; i < m_components.m_size; i++, pHC++) // fast array traversal (increasing ptr)
	{
		Component *pC = (*pHC).getObject<Component>();

		if (pC->isInstanceOf<CarPhysicsComponent>())
		{
			CarPhysicsComponent *pPC = (CarPhysicsComponent *)(pC);

			if(pPC->m_tag == "player")
			{
				return pPC;
			}
		}
	}

}

}; // namespace Components
}; // namespace PE
