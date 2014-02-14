#ifndef __PE_CAR_PHYSICS_MANAGER_H__
#define __PE_CAR_PHYSICS_MANAGER_H__

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
#include "PrimeEngine/Physics/Physics.h"

#include <vector>

namespace PE{
namespace Components {

struct CarPhysicsManager : public CarPhysicsComponent
{
	PE_DECLARE_CLASS(CarPhysicsManager);

	static void Construct(PE::GameContext &context, PE::MemoryArena arena);

	// Constructor -------------------------------------------------------------
	// same
	CarPhysicsManager(PE::GameContext &context, PE::MemoryArena arena, Handle hMyself) :  CarPhysicsComponent(context, arena, hMyself)
	{
		m_components.reset(512);
	}

	virtual ~CarPhysicsManager(){}

	// component
	virtual void addDefaultComponents();

	static CarPhysicsManager *Instance() {return s_hInstance.getObject<CarPhysicsManager>();}
	static CarPhysicsManager *TitleInstance() {return s_hTitleInstance.getObject<CarPhysicsManager>();}
	static Handle InstanceHandle() {return s_hInstance;}
	static Handle TitleInstanceHandle() {return s_hTitleInstance;}
	static CarPhysicsManager *CurInstance() {return s_hCurInstance.getObject<CarPhysicsManager>();}
	static Handle CurInstanceHandle() {return s_hCurInstance;}
	static void SetTitleAsCurrent(){ s_hCurInstance = s_hTitleInstance; }
	static void SetGameAsCurrent() { s_hCurInstance = s_hInstance; }
	static bool TitleIsCurrent() { return s_hCurInstance == s_hTitleInstance;}

	static void SetInstance(Handle h){s_hInstance = h;}

	void startComputeVelocity(PrimitiveTypes::Float32 frameTime);
	CarPhysicsComponent* getPlayerCar();

	std::vector<PhysicsComponent*> physicsComponentVector;
	PrimitiveTypes::Float32 m_frameTime;

	private:
		static Handle s_hInstance;
		static Handle s_hTitleInstance;
		static Handle s_hCurInstance;

};

}; // namespace Components
}; // namespace PE

#endif // file guard
