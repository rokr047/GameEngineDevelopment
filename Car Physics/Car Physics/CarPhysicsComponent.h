#ifndef _PE_CAR_PHYSICS_COMPONENT_H_
#define _PE_CAR_PHYSICS_COMPONENT_H_ 

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
		struct CarPhysicsComponent : public Component
		{
			PE_DECLARE_CLASS(CarPhysicsComponent);
	
			// Constructor -------------------------------------------------------------
			CarPhysicsComponent(PE::GameContext &context, PE::MemoryArena arena, Handle hMyself) ;
			CarPhysicsComponent(PE::GameContext &context, PE::MemoryArena arena, Handle hMyself, Handle hSceneNode) ;

			virtual ~CarPhysicsComponent(){}

			virtual void addDefaultComponents();

			PE_DECLARE_IMPLEMENT_EVENT_HANDLER_WRAPPER(do_PRE_RENDER_needsRC)
			void do_PRE_RENDER_needsRC(PE::Events::Event *pEvt);

			PE_DECLARE_IMPLEMENT_EVENT_HANDLER_WRAPPER(do_Tank_Throttle);
			void do_Tank_Throttle(PE::Events::Event *pEvt);

			PE_DECLARE_IMPLEMENT_EVENT_HANDLER_WRAPPER(do_Tank_Turn);
			void do_Tank_Turn(PE::Events::Event *pEvt);

			void computeVelocity(PrimitiveTypes::Float32 frameTime);

			/*char *m_colliderType;
			char *m_isCollidable;
			int m_sphereRadius;
			int m_boxLength;
			int m_boxWidth;
			int m_boxHeight;*/

			char *m_tag;
			float m_mass;
			float m_drag;
			bool m_useGravity;
			float m_frametime;
			bool m_isAnimated;

			Vector3 m_originPos;
			Matrix4x4 m_worldTransform;

			Vector3 m_boundingBoxPoints[8];


			// Car Data
			float m_throttlePosition;
			float m_gearRatio;
			float m_differentialRatio;
			float m_transmissionEfficiency;			
			float m_b,m_c,m_L;
			float m_weightFront, m_weightRear;
			
			
			//Constants
			float m_dragConstant;
			float m_rollingResistanceConstant;
			float m_breakingConstant;
			float m_wheelRadiusConstant;			
			float m_gForceConstant; 

			//Force
			Vector3 m_tractionForce;
			Vector3 m_dragForce;
			Vector3 m_rollingResistanceForce;
			Vector3 m_longitudinalForce;
			Vector3 m_breakingForce;
			Vector3 m_Velocity;
			Vector3 m_newPosition, m_currentPosition;
			Vector3 m_acceleration;
			Vector3 m_driveDirection;

			//Torque
			float m_maxTorque;
			float m_engineTorque;
			float m_driveTorque;


			Handle m_hSceneNode;
		};
	};	// namespace Components
};	// namespace PE


#endif