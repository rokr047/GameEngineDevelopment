#include "PhysicsComponent.h"

// API Abstraction
#include "PrimeEngine/APIAbstraction/APIAbstractionDefines.h"

#include "PrimeEngine/Lua/LuaEnvironment.h"
#include "PrimeEngine/Events/StandardEvents.h"
#define DEBUG_DRAW_TIME 10

namespace PE{
namespace Components{

PE_IMPLEMENT_CLASS1(PhysicsComponent, Component);

PhysicsComponent::PhysicsComponent(PE::GameContext &context, PE::MemoryArena arena, Handle hMyself)
: Component(context, arena, hMyself)
{
	
}

PhysicsComponent::PhysicsComponent(PE::GameContext &context, PE::MemoryArena arena, Handle hMyself, Handle hSceneNode)
: Component(context, arena, hMyself)
{
	m_hSceneNode = hSceneNode;
	m_isAnimated = false;
	m_velocity = Vector3(0.0,0.0,0.0);
	m_accelaration = Vector3(0.0,0.0,0.0);
}

void PhysicsComponent::addDefaultComponents()
{
	Component::addDefaultComponents();
}

void PhysicsComponent::computeVelocity(PrimitiveTypes::Float32 frametime)
{
	//DRAG FORCE
	m_dragForce = (- 2 * m_velocity.length()) * m_velocity;

	//ROLLING RESISTANCE
	m_rollingResistanceForce = - 2.5 * m_velocity;

	m_longitudinalForce = m_tractionForce + m_dragForce + m_rollingResistanceForce;

	//ACCELARATION
	m_accelaration = m_longitudinalForce /m_mass;

	m_velocity = m_velocity + m_accelaration * frametime;

	SceneNode *pFirstSN = m_hSceneNode.getObject<SceneNode>();

	Vector3 relativePosVelocity = m_velocity * frametime;

	pFirstSN->m_base.moveForward(relativePosVelocity.getZ());
	pFirstSN->m_base.moveRight(relativePosVelocity.getX());
	pFirstSN->m_base.moveUp(relativePosVelocity.getY());
}

}; //namespace components
}; //namespace PE