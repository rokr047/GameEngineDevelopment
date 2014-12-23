#ifndef __PE_AI_MANAGER_H__
#define __PE_AI_MANAGER_H__

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

#include "CharacterControl/WayPoint.h"

// Sibling/Children includes
#include "PrimeEngine/AI/AIComponent.h"

#include <vector>

namespace PE{

namespace Components {

struct AIManager : public AIComponent
{
	PE_DECLARE_CLASS(AIManager);

	static void Construct(PE::GameContext &context, PE::MemoryArena arena);

	// Constructor -------------------------------------------------------------
	// same
	AIManager(PE::GameContext &context, PE::MemoryArena arena, Handle hMyself) :  AIComponent(context, arena, hMyself)
	{
		m_components.reset(512);
	}

	virtual ~AIManager(){}

	// component
	virtual void addDefaultComponents();

	static AIManager *Instance() {return s_hInstance.getObject<AIManager>();}
	static AIManager *TitleInstance() {return s_hTitleInstance.getObject<AIManager>();}
	static Handle InstanceHandle() {return s_hInstance;}
	static Handle TitleInstanceHandle() {return s_hTitleInstance;}
	static AIManager *CurInstance() {return s_hCurInstance.getObject<AIManager>();}
	static Handle CurInstanceHandle() {return s_hCurInstance;}
	static void SetTitleAsCurrent(){ s_hCurInstance = s_hTitleInstance; }
	static void SetGameAsCurrent() { s_hCurInstance = s_hInstance; }
	static bool TitleIsCurrent() { return s_hCurInstance == s_hTitleInstance;}

	static void SetInstance(Handle h){s_hInstance = h;}

	void StartAI();

	bool checkIfInFOV(AIComponent *Enemy, AIComponent *Soldier);
	float getAngleBetweenVectors(Vector3 v1, Vector3 v2);

	void drawFOVDebugLines(AIComponent *debugComponent);

	void setWaypointForSoldiers(AIComponent *Soldier);

	void sortWaypointForSoldier(AIComponent *Soldier, std::vector<CharacterControl::Components::WayPoint*> *coverWaypointVector);
	bool AIManager::setWaypointAsAssigned(std::vector<CharacterControl::Components::WayPoint*> *coverWaypointVector, int index);

	//we are going to add the components to this vector.
	std::vector<AIComponent*> AIComponentVector;

	private:
		static Handle s_hInstance;
		static Handle s_hTitleInstance;
		static Handle s_hCurInstance;
};
}; // namespace Components
}; // namespace PE

#endif // file guard
