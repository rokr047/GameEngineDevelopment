#ifndef _PE_AI_COMPONENT_H_
#define _PE_AI_COMPONENT_H_ 

//API Abstraction

#include "PrimeEngine/APIAbstraction/APIAbstractionDefines.h"

// Outer Engine Includes
#include <assert.h>

// Inter-Engine includes
#include "PrimeEngine/MemoryManagement/Handle.h"
#include "PrimeEngine/PrimitiveTypes/PrimitiveTypes.h"
#include "PrimeEngine/Events/Component.h"
#include "PrimeEngine/Events/Event.h"
#include "PrimeEngine/Utils/Array/Array.h"
#include "PrimeEngine/Math/Vector3.h"
#include "PrimeEngine/Math/Vector4.h"
#include "PrimeEngine/Math/Matrix4x4.h"
#include "PrimeEngine/Scene/SceneNode.h"

namespace PE
{
	namespace Components
	{
		struct AIComponent : public Component
		{
			PE_DECLARE_CLASS(AIComponent);
	
			//Constructors
	
			// Constructor -------------------------------------------------------------
			AIComponent(PE::GameContext &context, PE::MemoryArena arena, Handle hMyself) ;
			AIComponent(PE::GameContext &context, PE::MemoryArena arena, Handle hMyself, Handle hSceneNode) ;
			AIComponent(PE::GameContext &context, PE::MemoryArena arena, Handle hMyself, Handle hBehaviorSM, Handle hSceneNode) ;

			virtual ~AIComponent(){}

			virtual void addDefaultComponents();

			Vector3 m_originPos;
			Matrix4x4 m_worldTransform;

			int m_faction;
			int m_fovdistance;

			Handle m_hSceneNode;
			Handle m_hBehaviorSM;
			char m_assignedWaypoint[32];
		};
	};	// namespace Components
};	// namespace PE



#endif