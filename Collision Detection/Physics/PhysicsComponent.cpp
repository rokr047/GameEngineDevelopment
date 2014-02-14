#include "PhysicsComponent.h"

// API Abstraction
#include "PrimeEngine/APIAbstraction/APIAbstractionDefines.h"

#include "PrimeEngine/Lua/LuaEnvironment.h"
#include "PrimeEngine/Events/StandardEvents.h"


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
}

void PhysicsComponent::addDefaultComponents()
{
	Component::addDefaultComponents();
}

}; //namespace components
}; //namespace PE