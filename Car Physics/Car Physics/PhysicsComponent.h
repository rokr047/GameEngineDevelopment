#ifndef _PE_PHYSICS_COMPONENT_H_
#define _PE_PHYSICS_COMPONENT_H_ 

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
		struct PhysicsComponent : public Component
		{
			PE_DECLARE_CLASS(PhysicsComponent);
	
			//Constructors
			//PhysicsComponent(PE::Handle hMyself);
	
			// Constructor -------------------------------------------------------------
			PhysicsComponent(PE::GameContext &context, PE::MemoryArena arena, Handle hMyself) ;
			PhysicsComponent(PE::GameContext &context, PE::MemoryArena arena, Handle hMyself, Handle hSceneNode) ;

			virtual ~PhysicsComponent(){}

			virtual void addDefaultComponents();

			void computeVelocity(PrimitiveTypes::Float32 frametime);

			char *m_colliderType;
			char *m_isCollidable;
			int m_sphereRadius;
			int m_boxLength;
			int m_boxWidth;
			int m_boxHeight;

			char m_tag[32];
			float m_mass;
			float m_drag;
			bool m_useGravity;

			bool m_isAnimated;

			Vector3 m_velocity;
			Vector3 m_accelaration;
			Vector3 m_rollingResistanceForce;
			Vector3 m_tractionForce;
			Vector3 m_longitudinalForce;
			Vector3 m_dragForce;

			Vector3 m_originPos;
			Matrix4x4 m_worldTransform;

			Vector3 m_boundingBoxPoints[8];

			Handle m_hSceneNode;
		};
	};	// namespace Components
};	// namespace PE



#endif