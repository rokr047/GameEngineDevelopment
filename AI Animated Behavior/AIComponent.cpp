#include "AIComponent.h"
// API Abstraction
#include "PrimeEngine/APIAbstraction/APIAbstractionDefines.h"

#include "PrimeEngine/Lua/LuaEnvironment.h"
#include "PrimeEngine/Events/StandardEvents.h"


namespace PE{
namespace Components{

PE_IMPLEMENT_CLASS1(AIComponent, Component);

AIComponent::AIComponent(PE::GameContext &context, PE::MemoryArena arena, Handle hMyself)
: Component(context, arena, hMyself)
{
	
}

AIComponent::AIComponent(PE::GameContext &context, PE::MemoryArena arena, Handle hMyself, Handle hSceneNode)
: Component(context, arena, hMyself)
{
	m_hSceneNode = hSceneNode;
}

AIComponent::AIComponent(PE::GameContext &context, PE::MemoryArena arena, Handle hMyself, Handle hBehaviorSM, Handle hSceneNode)
: Component(context, arena, hMyself)
{
	m_hSceneNode = hSceneNode;
	m_hBehaviorSM = hBehaviorSM;
}

void AIComponent::addDefaultComponents()
{
	Component::addDefaultComponents();
}

}; //namespace components
}; //namespace PE