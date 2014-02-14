#ifndef __PE_PHYSICS_MANAGER_H__
#define __PE_PHYSICS_MANAGER_H__

// API Abstraction
#include "PrimeEngine/APIAbstraction/APIAbstractionDefines.h"

// Outer-Engine includes
#include <assert.h>

// Inter-Engine includes
#include "PrimeEngine/MemoryManagement/Handle.h"
#include "PrimeEngine/PrimitiveTypes/PrimitiveTypes.h"
#include "PrimeEngine/Events/Component.h"
#include "PrimeEngine/Utils/Array/Array.h"
#include "PrimeEngine/APIAbstraction/Effect/Effect.h"

// Sibling/Children includes
#include "PrimeEngine/Physics/CarPhysicsComponent.h"
#include "PrimeEngine/Physics/PhysicsComponent.h"
#include "PrimeEngine/Physics/Physics.h"

#include <vector>

namespace PE{
namespace Components {

struct PhysicsManager : public PhysicsComponent
{
	PE_DECLARE_CLASS(PhysicsManager);

	static void Construct(PE::GameContext &context, PE::MemoryArena arena);

	// Constructor -------------------------------------------------------------
	// same
	PhysicsManager(PE::GameContext &context, PE::MemoryArena arena, Handle hMyself) :  PhysicsComponent(context, arena, hMyself)
	{
		m_components.reset(512);
	}

	virtual ~PhysicsManager(){}

	// component
	virtual void addDefaultComponents();

	static PhysicsManager *Instance() {return s_hInstance.getObject<PhysicsManager>();}
	static PhysicsManager *TitleInstance() {return s_hTitleInstance.getObject<PhysicsManager>();}
	static Handle InstanceHandle() {return s_hInstance;}
	static Handle TitleInstanceHandle() {return s_hTitleInstance;}
	static PhysicsManager *CurInstance() {return s_hCurInstance.getObject<PhysicsManager>();}
	static Handle CurInstanceHandle() {return s_hCurInstance;}
	static void SetTitleAsCurrent(){ s_hCurInstance = s_hTitleInstance; }
	static void SetGameAsCurrent() { s_hCurInstance = s_hInstance; }
	static bool TitleIsCurrent() { return s_hCurInstance == s_hTitleInstance;}

	bool GravityCheck(PhysicsComponent *sphereCollider1, PhysicsComponent *planeCollider2, bool gravity);

	static void SetInstance(Handle h){s_hInstance = h;}

	void StartPhysics(PrimitiveTypes::Float32 frametime);

	PrimitiveTypes::Float32 m_frameTime;

	std::vector<PhysicsComponent*> physicsComponentVector;

	std::vector<CarPhysicsComponent*> carPhysicsComponentVector;

	private:
		static Handle s_hInstance;
		static Handle s_hTitleInstance;
		static Handle s_hCurInstance;

};

}; // namespace Components
}; // namespace PE

#endif // file guard
