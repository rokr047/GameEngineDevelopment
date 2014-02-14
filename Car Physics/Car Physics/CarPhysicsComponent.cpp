#include "CarPhysicsComponent.h"

// API Abstraction
#include "PrimeEngine/APIAbstraction/APIAbstractionDefines.h"

#include "PrimeEngine/PrimeEngineIncludes.h"

#include "PrimeEngine/Lua/LuaEnvironment.h"
#include "PrimeEngine/Events/StandardEvents.h"
#include "PrimeEngine/Render/IRenderer.h"

#include "CharacterControl/Events/Events.h"

#define DEBUG_DRAW_TIME 10

using namespace PE::Components;
using namespace PE::Events;

namespace PE{
namespace Components{

PE_IMPLEMENT_CLASS1(CarPhysicsComponent, Component);

CarPhysicsComponent::CarPhysicsComponent(PE::GameContext &context, PE::MemoryArena arena, Handle hMyself)
: Component(context, arena, hMyself)
{
	
}

CarPhysicsComponent::CarPhysicsComponent(PE::GameContext &context, PE::MemoryArena arena, Handle hMyself, Handle hSceneNode)
: Component(context, arena, hMyself)
{
	m_hSceneNode = hSceneNode;
	m_isAnimated = false;
	m_Velocity.m_x = m_Velocity.m_y = m_Velocity.m_z = 0.0;
	m_driveDirection.m_x = m_driveDirection.m_y = m_driveDirection.m_z = 0.0;

}

void CarPhysicsComponent::addDefaultComponents()
{
	Component::addDefaultComponents();

	PE_REGISTER_EVENT_HANDLER(Event_PRE_RENDER_needsRC, CarPhysicsComponent::do_PRE_RENDER_needsRC);
	PE_REGISTER_EVENT_HANDLER(CharacterControl::Events::Event_Tank_Throttle, CarPhysicsComponent::do_Tank_Throttle);
	PE_REGISTER_EVENT_HANDLER(CharacterControl::Events::Event_Tank_Turn, CarPhysicsComponent::do_Tank_Turn);
}


void CarPhysicsComponent::do_PRE_RENDER_needsRC(PE::Events::Event *pEvt)
{
	Event_PRE_RENDER_needsRC *pRealEvent = (Event_PRE_RENDER_needsRC *)(pEvt);

	char buf[80];
	sprintf(buf, "Velocity: %f",m_Velocity.length());

	Vector3 printPos(m_hSceneNode.getObject<SceneNode>()->m_base.getPos());
	printPos.m_y += 2.0f;

	DebugRenderer::Instance()->createTextMesh(
			buf, false, false, true, false, 0,
			printPos, 0.01f, pRealEvent->m_threadOwnershipMask);
		

}

void CarPhysicsComponent::do_Tank_Throttle(PE::Events::Event *pEvt)
{
	CharacterControl::Events::Event_Tank_Throttle *pRealEvent = (CharacterControl::Events::Event_Tank_Throttle *)(pEvt);

	//printf("I'm called\n");
	if (!m_hSceneNode.isValid())
	{
		assert(!"wrong setup. must have scene node referenced");
		return;
	}

	SceneNode *pFirstSN = m_hSceneNode.getObject<SceneNode>();

	//Vector3 newPosition = pFirstSN->m_base.getPos() + (m_Velocity * m_frametime);

	//pFirstSN->m_base.setPos(newPosition);
	m_driveDirection = pRealEvent->m_relativeMove;
	Vector3 relativePosVelocity;
	if(m_driveDirection.m_z > 0)
	{
		relativePosVelocity =pRealEvent->m_relativeMove * 2.0 * m_frametime;
		printf("Accel\n");
	}
	else if(m_driveDirection.m_z < 0)
	{
		relativePosVelocity =pRealEvent->m_relativeMove*0.5 * m_frametime;
		printf("braking\n");
	}

	//pFirstSN->m_base.moveForward(relativePosVelocity.getZ());
	pFirstSN->m_base.moveRight(relativePosVelocity.getX());
	pFirstSN->m_base.moveUp(relativePosVelocity.getY());
}

void CarPhysicsComponent::do_Tank_Turn(PE::Events::Event *pEvt)
{
	CharacterControl::Events::Event_Tank_Turn *pRealEvent = (CharacterControl::Events::Event_Tank_Turn *)(pEvt);

	if (!m_hSceneNode.isValid())
	{
		assert(!"wrong setup. must have scene node referenced");
		return;
	}

	SceneNode *pFirstSN = m_hSceneNode.getObject<SceneNode>();

	//pcam->m_base.turnUp(pRealEvent->m_relativeRotate.getY());
	if(m_Velocity.lengthSqr() >= 2.25)
	{
		pFirstSN->m_base.turnLeft(-pRealEvent->m_relativeRotate.getX());
	}
	pFirstSN->m_base.turnUp(pRealEvent->m_relativeRotate.getY());
}

void CarPhysicsComponent::computeVelocity(PrimitiveTypes::Float32 frameTime)
{
	m_frametime = frameTime ;

	// we will compute the final velocity of the car here.

	//TRACTION FORCE
	m_engineTorque = m_throttlePosition * m_maxTorque;
	m_driveTorque = m_engineTorque * m_gearRatio * m_differentialRatio * m_transmissionEfficiency ;

	m_tractionForce = m_driveDirection * (m_driveTorque/m_wheelRadiusConstant);

	//DRAG FORCE
	m_dragForce = (- m_dragConstant * m_Velocity.length()) * m_Velocity;

	//ROLLING RESISTANCE
	m_rollingResistanceForce = - m_rollingResistanceConstant * m_Velocity;

	m_longitudinalForce = m_tractionForce + m_dragForce + m_rollingResistanceForce;

	//ACCELARATION
	m_acceleration = m_longitudinalForce /1500;

	//Current Velocity
	m_Velocity = m_Velocity + m_acceleration * frameTime;

	SceneNode *pFirstSN = m_hSceneNode.getObject<SceneNode>();

	Vector3 relativePosVelocity = m_Velocity * m_frametime;

	pFirstSN->m_base.moveForward(relativePosVelocity.getZ());
}

}; //namespace components
}; //namespace PE