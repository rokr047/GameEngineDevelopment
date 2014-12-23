#include "../Lua/LuaEnvironment.h"
#include "PrimeEngine/AI/AIManager.h"
#include "PrimeEngine/Math/Vector3.h"
#include "PrimeEngine/Math/Vector4.h"
#include "PrimeEngine/Math/Matrix4x4.h"
#include "PrimeEngine/Scene/DebugRenderer.h"
#include "PrimeEngine/Events/Component.h"
#include "CharacterControl/Characters/SoldierNPCAnimationSM.h"
#include "CharacterControl/Characters/SoldierNPCBehaviorSM.h"
#include "CharacterControl/Characters/SoldierNPCMovementSM.h"
#include "CharacterControl/Characters/SoldierNPC.h"
#include "CharacterControl/Events/Events.h"
#include "CharacterControl/ClientGameObjectManagerAddon.h"
#include <assert.h>
#include <math.h>
#include <vector>
#include "CharacterControl/CharacterControlContext.h"

#define FOVANGLE 20
#define PI 3.14159265
#define FOVDISTANCESQRD 900
#define DEBUGVECTORLENGTH 30
#define DEBUGLINETIME 2


namespace PE {
namespace Components {

PE_IMPLEMENT_CLASS1(AIManager, AIComponent);

Handle AIManager::s_hTitleInstance;
Handle AIManager::s_hInstance;
Handle AIManager::s_hCurInstance;

void AIManager::Construct(PE::GameContext &context, PE::MemoryArena arena)
{
	PE::Handle h("AI_MANAGER", sizeof(AIManager));
	AIManager *pAIManager = new(h) AIManager(context, arena, h);
	pAIManager->addDefaultComponents();
	SetInstance(h);
	
	s_hTitleInstance = Handle("AI_MANAGER", sizeof(AIManager));
	AIManager *pTitleAIManager = new(s_hTitleInstance) AIManager(context, arena, h);
	pTitleAIManager->addDefaultComponents();
	
	SetTitleAsCurrent();
}

void AIManager::addDefaultComponents()
{
	AIComponent::addDefaultComponents();
}

void AIManager::StartAI()
{
	//Do AI Stuff.

	//get all the AI Components and add it to the vector.
	PE::Handle *pHC = m_components.getFirstPtr();
	AIComponentVector.clear();

	//For each AI component, add it to the vector, so that we can cycle through later.
	for (PrimitiveTypes::UInt32 i = 0; i < m_components.m_size; i++, pHC++) // fast array traversal (increasing ptr)
	{
		Component *pC = (*pHC).getObject<Component>();

		if (pC->isInstanceOf<AIComponent>())
		{
			AIComponent *pAIC = (AIComponent *)(pC);

			//Set the position and the world transform of the object here
			PEASSERT(pAIC->m_hSceneNode.isValid(), "Each instance must have a scene node parent");

			//multiply a local space vector by this matrix to get the vector in world space..
			pAIC->m_worldTransform = pAIC->m_hSceneNode.getObject<SceneNode>()->m_worldTransform;
			
			//This position is actually in world space.
			pAIC->m_originPos = pAIC->m_hSceneNode.getObject<SceneNode>()->m_base.getPos(); 
				
			AIComponentVector.push_back(pAIC);
		}
	}

	if(AIComponentVector.size() > 0)
	{
		int vSize = AIComponentVector.size();

		//If true, AIComponentVector[j] is in FOV of AIComponentVector[i]
		bool inFOVResult = false;
		
		//Compute viewResult with all other soldiers
		//!inFOVResult in the for loop, will make sure that even if 1 person sees the opposite faction,
		//the whole team will run for cover.
		for(int i=0;i<vSize && !inFOVResult ;i++)
		{
			drawFOVDebugLines(AIComponentVector[i]);

			//Check which other soldiers are in FOV of AIComponentVector[i]...
			for(int j=0;j<vSize && !inFOVResult ;j++)
			{
				drawFOVDebugLines(AIComponentVector[j]);
				
				//	Compute Field of view result only if the soldiers belong to different factions
				if (AIComponentVector[i]->m_faction != AIComponentVector[j]->m_faction && AIComponentVector[i]->m_faction != 0 && AIComponentVector[j] != 0)
				{
					inFOVResult = checkIfInFOV(AIComponentVector[i], AIComponentVector[j]);
				}
				else
				{
					//Both the soldiers are in the same faction,
					//do not compute FOV.
					continue;
				}
			}
		}

		if(inFOVResult)
		{
			//	Call the event target reached of the SoldierBehaviorSM 
			PE::Handle h("SoldierNPCMovementSM_Event_TARGET_REACHED", sizeof(CharacterControl::Events::SoldierNPCMovementSM_Event_TARGET_REACHED));
			CharacterControl::Events::SoldierNPCMovementSM_Event_TARGET_REACHED *pOutEvt = new(h) CharacterControl::Events::SoldierNPCMovementSM_Event_TARGET_REACHED();

			//Atleast 1 person has seen the enemy faction
			//so let everybody in the team know.
			for(int i=0;i<vSize;i++)
			{
				

				//	Change state of Soldiers i and j to RUN_TO_COVER
				if(AIComponentVector[i]->m_hBehaviorSM.getObject<CharacterControl::Components::SoldierNPCBehaviorSM>()->m_state != CharacterControl::Components::SoldierNPCBehaviorSM::States::RUN_TO_COVER &&
					AIComponentVector[i]->m_hBehaviorSM.getObject<CharacterControl::Components::SoldierNPCBehaviorSM>()->m_state != CharacterControl::Components::SoldierNPCBehaviorSM::States::TAKING_COVER)
				{
					if (AIComponentVector[i]->m_faction != 0)
					{
						//Need to set the waypoint to run to.
						setWaypointForSoldiers(AIComponentVector[i]);
					
						AIComponentVector[i]->m_hBehaviorSM.getObject<CharacterControl::Components::SoldierNPCBehaviorSM>()->m_state = CharacterControl::Components::SoldierNPCBehaviorSM::States::RUN_TO_COVER;
						AIComponentVector[i]->m_hBehaviorSM.getObject<CharacterControl::Components::SoldierNPCBehaviorSM>()->handleEvent(pOutEvt);
					}
				}		
			}

			h.release();
		}
	}
}

void AIManager::sortWaypointForSoldier(AIComponent *Soldier, std::vector<CharacterControl::Components::WayPoint*> *coverWaypointVector)
{
	//Compute the distance from soldier to waypoint for each waypoint
	for(int i=0;i< (coverWaypointVector->size() - 1);i++)
	{
		for(int j=0; j < (coverWaypointVector->size() - i - 1); j++)
		{
			Vector3 dirVecFromSoldierToWaypoint1 = (*coverWaypointVector)[j]->m_base.getPos() - Soldier->m_originPos;
			Vector3 dirVecFromSoldierToWaypoint2 = (*coverWaypointVector)[j+1]->m_base.getPos() - Soldier->m_originPos;

			(*coverWaypointVector)[j]->distanceFromSoldier = dirVecFromSoldierToWaypoint1.dotProduct(dirVecFromSoldierToWaypoint1);
			(*coverWaypointVector)[j+1]->distanceFromSoldier = dirVecFromSoldierToWaypoint2.dotProduct(dirVecFromSoldierToWaypoint2);

			if((*coverWaypointVector)[j]->distanceFromSoldier  > (*coverWaypointVector)[j+1]->distanceFromSoldier)
			{
				CharacterControl::Components::WayPoint* tempWayPoint = (*coverWaypointVector)[j];
				(*coverWaypointVector)[j] = (*coverWaypointVector)[j+1];
				(*coverWaypointVector)[j+1] = tempWayPoint;
			}
		}
	}
}

void AIManager::setWaypointForSoldiers(AIComponent *Soldier)
{
	int vSize = AIComponentVector.size();
	Vector3 soldierPos = Soldier->m_originPos;

	for(int i=0;i<vSize;i++)
	{
		if(Soldier->m_faction != AIComponentVector[i]->m_faction)
		{
			//We have found the enemy for this soldier.
			//Get the position of the enemy.
			
			Vector3 enemyPos = AIComponentVector[i]->m_originPos;

			//now compute the direction vector.
			Vector3 dirVecFromSoldierToEnemy = enemyPos - soldierPos;
			dirVecFromSoldierToEnemy.normalize();

			//Now we need to make sure that the soldier run, in the opposite
			//direction of the enemy, and get to the cover waypoint.
			std::vector<CharacterControl::Components::WayPoint*> coverWaypointVector;
			coverWaypointVector.clear();


			CharacterControl::Components::ClientGameObjectManagerAddon *pGameObjectManagerAddon = (CharacterControl::Components::ClientGameObjectManagerAddon  *)(m_pContext->get<CharacterControl::CharacterControlContext>()->getGameObjectManagerAddon());
			if (pGameObjectManagerAddon)
			{
				coverWaypointVector = pGameObjectManagerAddon->getAllCoverWaypoints();

				sortWaypointForSoldier(Soldier,&coverWaypointVector);

				//Now we have all the cover points.
				//For each waypoint get the direction vector.
				CharacterControl::Components::WayPoint* tempWayPoint;

				for(int i=0;i<coverWaypointVector.size();i++)
				{				
					bool hasAlreadyBeenAssigned = false;
					
					for(int j = 0; j < AIComponentVector.size();j++)
					{
						
						if(strcmpi(coverWaypointVector[i]->m_name, AIComponentVector[j]->m_hBehaviorSM.getObject<CharacterControl::Components::SoldierNPCBehaviorSM>()->m_coverWaypoint) == 0)
						{
							hasAlreadyBeenAssigned = true;
							break;
						}
						
					}

					if(!hasAlreadyBeenAssigned)
					{
						StringOps::writeToString(coverWaypointVector[i]->m_name, Soldier->m_hBehaviorSM.getObject<CharacterControl::Components::SoldierNPCBehaviorSM>()->m_coverWaypoint, 32);
						break;
					}
				}
			}

			Soldier->m_hBehaviorSM.getObject<CharacterControl::Components::SoldierNPCBehaviorSM>()->m_hEnemy = AIComponentVector[i]->m_hSceneNode;
		}
	}
}

bool AIManager::setWaypointAsAssigned(std::vector<CharacterControl::Components::WayPoint*> *coverWaypointVector, int index)
{
	(*coverWaypointVector)[index]->m_hasBeenAssignedToSoldier = 1;

	return true;
}

bool AIManager::checkIfInFOV(AIComponent *Enemy, AIComponent *Soldier)
{
	//How to calculate if the soldier is in FOV of the enemy fast:

	// Step 1 : Get the direction vector from soldier1 to soldier2 [dirSoldier1].
	// Step 2 : get the forward vector of soldier1 [fvSoldier1]
	// Step 3 : compute angle between dirSoldier1 and fvSoldier1. [sol1Alpha]
	// Step 4 : if sol1Alpha < FOVAngle, soldier2 is in FOV of soldier1.

	//Getting the forward vector from the transformation matrix.
	Vector3 fVecEnemy = Enemy->m_hSceneNode.getObject<SceneNode>()->m_base.getN();

	//Getting the direction vector.
	Vector3 dirEnemyToSoldier = Soldier->m_originPos - Enemy->m_originPos;

	float distEnemyToSoldierSqrd = dirEnemyToSoldier.dotProduct(dirEnemyToSoldier);

	//Now we have to compute the angle between the direction vector and the forward vector.
	//And if the soldier is within 20 metres from the enemy. 
	if((getAngleBetweenVectors(fVecEnemy,dirEnemyToSoldier) < FOVANGLE) && distEnemyToSoldierSqrd <= (Soldier->m_fovdistance * Soldier->m_fovdistance))
	{
		//Soldier is in FOV of enemy.
		return true;
	}

	return false;
}

float AIManager::getAngleBetweenVectors(Vector3 v1, Vector3 v2)
{
	float degrees = 0.0f;

	//Compute the dot product of the 2 vectors
	float dotProduct = v1.dotProduct(v2);

	//Cos0 = (v1 . v2 ) / ||a|| . ||b||
	//0 = acos(Value)

	float value = dotProduct / (v1.length() * v2.length());

	degrees = (acos(value) * 180.0) / PI;

	return degrees;
}

void AIManager::drawFOVDebugLines(AIComponent *debugComponent)
{
	Vector3 forward = debugComponent->m_hSceneNode.getObject<SceneNode>()->m_base.getN();

	//Now build the rotation Matrix with the FOV value
	Matrix4x4 rotMatrix(std::cosf(FOVANGLE *  PI / 180.0), 0, -std::sinf(FOVANGLE * PI / 180.0),0,
		      0, 1, 0, 0,
			  std::sinf(FOVANGLE * PI / 180.0), 0,std::cosf(FOVANGLE * PI / 180.0), 0,
			  0, 0, 0, 1);

	Matrix4x4 rotMatrixInverse(std::cosf(FOVANGLE * PI / 180.0), 0, std::sinf(FOVANGLE * PI / 180.0),	0,
								0,					1,					0,		0,
							   -std::sinf(FOVANGLE * PI / 180.0), 0,	std::cosf(FOVANGLE * PI / 180.0),	0,
								0,					0,					  0,	1);

	Vector3 oneSideRotation = rotMatrix * forward;
	Vector3 otherSideRotation = rotMatrixInverse * forward;

	Vector3 position =  debugComponent->m_hSceneNode.getObject<SceneNode>()->m_base.getPos();

	Vector3 leftPosition = debugComponent->m_hSceneNode.getObject<SceneNode>()->m_base.getPos() + (debugComponent->m_fovdistance * oneSideRotation);

	Vector3 rightPosition = debugComponent->m_hSceneNode.getObject<SceneNode>()->m_base.getPos() + (debugComponent->m_fovdistance * otherSideRotation);

	Vector3 color;
	if(debugComponent->m_faction == 1)
	{
		//	Team Red
		color.m_x = 1.40f;	color.m_y = 0.25f;	color.m_z = 0.25f;
	}
	else if(debugComponent->m_faction == 2)
	{
		//	Team Blue
		color.m_x = 0.25f;	color.m_y = 0.25f;	color.m_z = 1.40f;
	}
	else if(debugComponent->m_faction == 0)
	{
		//	Civilian
		color.m_x = 1.40f;	color.m_y = 1.40f;	color.m_z = 1.40f;
	}
	
	Vector3 linepts1[] = {position, color, rightPosition, color};
	Vector3 linepts2[] = {position, color, leftPosition, color};

	Matrix4x4 dummy;

	DebugRenderer::Instance()->createLineMesh(false, dummy,  &linepts1[0].m_x, 2, DEBUGLINETIME);
	DebugRenderer::Instance()->createLineMesh(false, dummy,  &linepts2[0].m_x, 2, DEBUGLINETIME);

}

}; // namespace Components
}; // namespace PE
