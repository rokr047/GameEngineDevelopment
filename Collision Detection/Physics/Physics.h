#ifndef _PE_PHYSICS_H_
#define _PE_PHYSICS_H_

#include "PrimeEngine/APIAbstraction/APIAbstractionDefines.h"

// Outer Engine Includes
#include <assert.h>

// Inter-Engine includes
#include "PrimeEngine/MemoryManagement/Handle.h"
#include "PrimeEngine/PrimitiveTypes/PrimitiveTypes.h"
#include "PrimeEngine/Events/Component.h"
#include "PrimeEngine/Events/Event.h"
#include "PrimeEngine/Utils/Array/Array.h"
#include "PrimeEngine/PhysX/Physics.h"
#include "PrimeEngine/Math/Vector3.h"
#include "PrimeEngine/Scene/SceneNode.h"
#include "PrimeEngine/Events/StandardEvents.h"

#include "PhysicsComponent.h"

namespace PE
{
	namespace Components
	{
		struct Physics: public Component
		{
			PE_DECLARE_CLASS(Physics);

			//Constructor
			Physics(PE::GameContext &context, PE::MemoryArena arena, Handle myHandle);
	
			virtual ~Physics(){}

			virtual void addDefaultComponents();

			// Sphere-Sphere Collision
			static bool CheckCollision(PhysicsComponent *sphereCollider1, PhysicsComponent *sphereCollider2);

			// Individual events -------------------------------------------------------
			static Physics *Instance() {return s_hInstance.getObject<Physics>();}
			static Physics *TitleInstance() {return s_hTitleInstance.getObject<Physics>();}
			static Handle InstanceHandle() {return s_hInstance;}
			static Handle TitleInstanceHandle() {return s_hTitleInstance;}
			static Physics *CurInstance() {return s_hCurInstance.getObject<Physics>();}
			static Handle CurInstanceHandle() {return s_hCurInstance;}
			static void SetTitleAsCurrent(){ s_hCurInstance = s_hTitleInstance; }
			static void SetGameAsCurrent() { s_hCurInstance = s_hInstance; }
			static bool TitleIsCurrent() { return s_hCurInstance == s_hTitleInstance;}

			static void SetInstance(Handle h){s_hInstance = h;}
			private:
				static Handle s_hInstance;
				static Handle s_hTitleInstance;
				static Handle s_hCurInstance;
		};
	};
};

#endif