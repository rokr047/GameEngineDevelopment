#include "../Lua/LuaEnvironment.h"
#include "PrimeEngine/Physics/PhysicsManager.h"
#include "PrimeEngine/Physics/DebugLines/DebugLines.h"
#include "PrimeEngine/Math/Vector3.h"
#include "PrimeEngine/Math/Vector4.h"
#include "PrimeEngine/Math/Matrix4x4.h"
#include "PrimeEngine/Scene/DebugRenderer.h"
#include "CharacterControl/Characters/SoldierNPCAnimationSM.h"
#include "CarPhysicsManager.h"
#include <assert.h>

namespace PE {
namespace Components {

PE_IMPLEMENT_CLASS1(PhysicsManager, PhysicsComponent);

Handle PhysicsManager::s_hTitleInstance;
Handle PhysicsManager::s_hInstance;
Handle PhysicsManager::s_hCurInstance;

void PhysicsManager::Construct(PE::GameContext &context, PE::MemoryArena arena)
{
	PE::Handle h("PHYSICS_MANAGER", sizeof(PhysicsManager));
	PhysicsManager *pPhysicsManager = new(h) PhysicsManager(context, arena, h);
	pPhysicsManager->addDefaultComponents();
	SetInstance(h);
	
	s_hTitleInstance = Handle("PHYSICS_MANAGER", sizeof(PhysicsManager));
	PhysicsManager *pTitlePhysicsManager = new(s_hTitleInstance) PhysicsManager(context, arena, h);
	pTitlePhysicsManager->addDefaultComponents();
	
	SetTitleAsCurrent();
}

void PhysicsManager::addDefaultComponents()
{
	PhysicsComponent::addDefaultComponents();
}

void PhysicsManager::StartPhysics(PrimitiveTypes::Float32 frametime)
{
	PE::Handle *pHC = m_components.getFirstPtr();
	physicsComponentVector.clear();

	//For each physics component. Build the collider here.
	for (PrimitiveTypes::UInt32 i = 0; i < m_components.m_size; i++, pHC++) // fast array traversal (increasing ptr)
	{
		Component *pC = (*pHC).getObject<Component>();

		if (pC->isInstanceOf<PhysicsComponent>())
		{
			PhysicsComponent *pPC = (PhysicsComponent *)(pC);
			
			if(strcmp(pPC->m_isCollidable, "false") == 0)
			{
				//This component does not need a collider.
				//OutputDebugStringA("This object does NOT need a collider.\n");
				continue;
			}
			else
			{
				//Need to build a collider for the object.
				//OutputDebugStringA("This object need a collider.\n");

				//Set the position and the world transform of the object here
				PEASSERT(pPC->m_hSceneNode.isValid(), "Each instance must have a scene node parent");

				//multiply a local space vector by this matrix to get the vector in world space..
				pPC->m_worldTransform = pPC->m_hSceneNode.getObject<SceneNode>()->m_worldTransform;
				pPC->m_originPos = pPC->m_hSceneNode.getObject<SceneNode>()->m_base.getPos(); 
				
				physicsComponentVector.push_back(pPC);
				if(strcmp(pPC->m_tag,"soldier") == 0)
				{
					pPC->m_accelaration = Vector3(0.0,0.0,1.0 * frametime);
					pPC->computeVelocity(frametime);
				}
			}
			
		}
	}
		
	if(physicsComponentVector.size() > 0)
	{
		int vSize = physicsComponentVector.size();
		
		//Check for collision with all the other objects
		for(int i=0;i< vSize-1;i++)
		{
			if(strcmp(physicsComponentVector[i]->m_colliderType,"plane") == 0)
			{
				//OutputDebugStringA("Encountered a plane with common objects not checking for collision.\n");
				continue;
			}

			for(int j=i+1;j<vSize;j++)
			{
				bool collisionResult;
				
				if(strcmp(physicsComponentVector[j]->m_colliderType,"plane") == 0)
				{
					//OutputDebugStringA("Encountered a plane with common objects not checking for collision.\n");
					continue;
				}

				collisionResult = Physics::CheckCollision(physicsComponentVector[i], physicsComponentVector[j]);

				//NEED TO FIX THIS
				// COLLISION DETECTION AND MOMENTUM TRANSFER IS BAD
				if(collisionResult)
				{
					Vector3 pos1 = physicsComponentVector[i]->m_hSceneNode.getObject<SceneNode>()->m_base.getPos();
					Vector3 pos2 = physicsComponentVector[j]->m_hSceneNode.getObject<SceneNode>()->m_base.getPos(); 

					Vector3 dir = pos2 - pos1;

					if(physicsComponentVector[i]->m_isAnimated && physicsComponentVector[j]->m_isAnimated)
					{
						physicsComponentVector[i]->m_hSceneNode.getObject<SceneNode>()->m_base.setPos(pos1-(dir*0.03)+Vector3(0.07,0,0));
					}
					else if(physicsComponentVector[i]->m_isAnimated)
					{
						//physicsComponentVector[i]->m_hSceneNode.getObject<SceneNode>()->m_base.setPos(pos1-(dir*0.03));
					}
					else if(physicsComponentVector[j]->m_isAnimated)
					{
						//physicsComponentVector[j]->m_hSceneNode.getObject<SceneNode>()->m_base.setPos(pos2-(dir*0.03));
					}

					if(strcmp(physicsComponentVector[i]->m_colliderType,"playercar") == 0 && strcmp(physicsComponentVector[j]->m_tag,"soldier") == 0 )
					{
 						CarPhysicsComponent *playerCar = CarPhysicsManager::Instance()->getPlayerCar();
						Vector3 c_acceleration = playerCar->m_acceleration;
						float c_mass = playerCar->m_mass;

						Vector3 c_force = c_mass * c_acceleration;
						physicsComponentVector[j]->m_tractionForce = c_force ;
					}
					else if(strcmp(physicsComponentVector[j]->m_colliderType,"playercar")== 0 && strcmp(physicsComponentVector[i]->m_tag,"soldier") == 0)
					{
						CarPhysicsComponent *playerCar = CarPhysicsManager::Instance()->getPlayerCar();
						Vector3 c_acceleration = playerCar->m_acceleration;
						float c_mass = playerCar->m_mass;

						Vector3 c_force = c_mass * c_acceleration;
						physicsComponentVector[i]->m_tractionForce = c_force ;
					}
				}
			}
		}

		//Check for gravity with the soldier and cobbler plane

		for(int i=0;i < vSize-1;i++)
		{
			bool gravityResult = false;

			if(strcmp(physicsComponentVector[i]->m_colliderType,"playercar") == 0 || strcmp(physicsComponentVector[i]->m_colliderType,"sphere") == 0)
			{
				for(int j=0;j<vSize;j++)
				{
					if(strcmp(physicsComponentVector[j]->m_colliderType,"plane") != 0)
					{
						//OutputDebugStringA("Encountered a common objects not checking for collision.\n");
						continue;
					}

					gravityResult = GravityCheck(physicsComponentVector[i], physicsComponentVector[j],true);
					if(gravityResult)
					{
						break;
					}
				}
			}
			else
			{
				//OutputDebugStringA("Encountered a common objects not checking for collision.\n");
				continue;
			}

			if(!gravityResult)
			{
				if(physicsComponentVector[i]->m_isAnimated || strcmp(physicsComponentVector[i]->m_colliderType,"playercar") == 0 )
				{
					//OutputDebugStringA("Soldier not on a plane");
					physicsComponentVector[i]->m_hSceneNode.getObject<SceneNode>()->m_base.setPos(physicsComponentVector[i]->m_hSceneNode.getObject<SceneNode>()->m_base.getPos() + Vector3(0,-0.2,0));
				}
			}
		}
	}
}

bool PhysicsManager::GravityCheck(PhysicsComponent *sphereCollider1, PhysicsComponent *planeCollider2, bool gravity)
	{

		//Specific gravity check for tank.
		if((strcmp(sphereCollider1->m_colliderType,"playercar") == 0) && (strcmp(planeCollider2->m_colliderType,"plane") == 0))
		{
			//If we encounter the tank.
			//we are going to generate 4 spheres for wheels (easier and faster than box or cylinders)
			Matrix4x4 worldMatrixInverse = planeCollider2->m_worldTransform.inverse();
			
			Vector3 localPosOfBox = worldMatrixInverse * planeCollider2->m_originPos;
			Vector3 color1(1.5f, 0.5f, 0.1f);

			Vector3 boxMin = localPosOfBox + Vector3(-planeCollider2->m_boxWidth/2, 0, -planeCollider2->m_boxLength/2);
			Vector3 boxMax = localPosOfBox + Vector3(planeCollider2->m_boxWidth/2, planeCollider2->m_boxHeight/4, planeCollider2->m_boxLength/2);

			float radius = 0.5 ,value = 0.5;
			Vector3 localTankBackLeft = sphereCollider1->m_worldTransform.inverse() * sphereCollider1->m_originPos;

					localTankBackLeft.m_z += 1.6f;
					localTankBackLeft.m_y += 0.2f;
					localTankBackLeft.m_x += 0.7f;

					//DEBUG DRAW -- START
					Vector3 BL_point0 = sphereCollider1->m_worldTransform * (localTankBackLeft + Vector3(value, 0, value));
					Vector3 BL_point1 = sphereCollider1->m_worldTransform * (localTankBackLeft + Vector3(-value, 0, value));
					Vector3 BL_point2 = sphereCollider1->m_worldTransform * (localTankBackLeft + Vector3(-value, 0, -value));
					Vector3 BL_point3 = sphereCollider1->m_worldTransform * (localTankBackLeft + Vector3(value, 0, -value));

					Vector3 BL_point4 = sphereCollider1->m_worldTransform * (localTankBackLeft + Vector3(value, value, value));
					Vector3 BL_point5 = sphereCollider1->m_worldTransform * (localTankBackLeft + Vector3(-value, value, value));
					Vector3 BL_point6 = sphereCollider1->m_worldTransform * (localTankBackLeft + Vector3(-value, value, -value));
					Vector3 BL_point7 = sphereCollider1->m_worldTransform * (localTankBackLeft + Vector3(value, value, -value));

					DebugLines::drawDebugLinesForBox(BL_point0,BL_point1,BL_point2,BL_point3,BL_point4,BL_point5,BL_point6,BL_point7,color1);
					//DEBUG DRAW -- END

				Vector3 localTankBackRight = sphereCollider1->m_worldTransform.inverse() * sphereCollider1->m_originPos;

				localTankBackRight.m_z += 1.6f;
				localTankBackRight.m_y += 0.2f;
				localTankBackRight.m_x -= 0.7f;

				//DEBUG DRAW -- START
				Vector3 BR_point0 = sphereCollider1->m_worldTransform * (localTankBackRight + Vector3(value, 0, value));
				Vector3 BR_point1 = sphereCollider1->m_worldTransform * (localTankBackRight + Vector3(-value, 0, value));
				Vector3 BR_point2 = sphereCollider1->m_worldTransform * (localTankBackRight + Vector3(-value, 0, -value));
				Vector3 BR_point3 = sphereCollider1->m_worldTransform * (localTankBackRight + Vector3(value, 0, -value));

				Vector3 BR_point4 = sphereCollider1->m_worldTransform * (localTankBackRight + Vector3(value, value, value));
				Vector3 BR_point5 = sphereCollider1->m_worldTransform * (localTankBackRight + Vector3(-value, value, value));
				Vector3 BR_point6 = sphereCollider1->m_worldTransform * (localTankBackRight + Vector3(-value, value, -value));
				Vector3 BR_point7 = sphereCollider1->m_worldTransform * (localTankBackRight + Vector3(value, value, -value));

				DebugLines::drawDebugLinesForBox(BR_point0,BR_point1,BR_point2,BR_point3,BR_point4,BR_point5,BR_point6,BR_point7,color1);
				//DEBUG DRAW -- END

					Vector3 localTankFrontLeft = sphereCollider1->m_worldTransform.inverse() * sphereCollider1->m_originPos;

					localTankFrontLeft.m_z -= 2.2f;
					localTankFrontLeft.m_y += 0.2f;
					localTankFrontLeft.m_x += 0.7f;

					//DEBUG DRAW -- START
					Vector3 FL_point0 = sphereCollider1->m_worldTransform * (localTankFrontLeft + Vector3(value, 0, value));
					Vector3 FL_point1 = sphereCollider1->m_worldTransform * (localTankFrontLeft + Vector3(-value, 0, value));
					Vector3 FL_point2 = sphereCollider1->m_worldTransform * (localTankFrontLeft + Vector3(-value, 0, -value));
					Vector3 FL_point3 = sphereCollider1->m_worldTransform * (localTankFrontLeft + Vector3(value, 0, -value));

					Vector3 FL_point4 = sphereCollider1->m_worldTransform * (localTankFrontLeft + Vector3(value, value, value));
					Vector3 FL_point5 = sphereCollider1->m_worldTransform * (localTankFrontLeft + Vector3(-value, value, value));
					Vector3 FL_point6 = sphereCollider1->m_worldTransform * (localTankFrontLeft + Vector3(-value, value, -value));
					Vector3 FL_point7 = sphereCollider1->m_worldTransform * (localTankFrontLeft + Vector3(value, value, -value));

					DebugLines::drawDebugLinesForBox(FL_point0,FL_point1,FL_point2,FL_point3,FL_point4,FL_point5,FL_point6,FL_point7,color1);
					//DEBUG DRAW -- END

				Vector3 localTankFrontRight = sphereCollider1->m_worldTransform.inverse() * sphereCollider1->m_originPos;

				localTankFrontRight.m_z -= 2.2f;
				localTankFrontRight.m_y += 0.2f;
				localTankFrontRight.m_x -= 0.7f;

				//DEBUG DRAW -- START
				Vector3 FR_point0 = sphereCollider1->m_worldTransform * (localTankFrontRight + Vector3(value, 0, value));
				Vector3 FR_point1 = sphereCollider1->m_worldTransform * (localTankFrontRight + Vector3(-value, 0, value));
				Vector3 FR_point2 = sphereCollider1->m_worldTransform * (localTankFrontRight + Vector3(-value, 0, -value));
				Vector3 FR_point3 = sphereCollider1->m_worldTransform * (localTankFrontRight + Vector3(value, 0, -value));

				Vector3 FR_point4 = sphereCollider1->m_worldTransform * (localTankFrontRight + Vector3(value, value, value));
				Vector3 FR_point5 = sphereCollider1->m_worldTransform * (localTankFrontRight + Vector3(-value, value, value));
				Vector3 FR_point6 = sphereCollider1->m_worldTransform * (localTankFrontRight + Vector3(-value, value, -value));
				Vector3 FR_point7 = sphereCollider1->m_worldTransform * (localTankFrontRight + Vector3(value, value, -value));

				DebugLines::drawDebugLinesForBox(FR_point0,FR_point1,FR_point2,FR_point3,FR_point4,FR_point5,FR_point6,FR_point7,color1);
				//DEBUG DRAW -- END

				Vector3 localTankRightSphereCenter = planeCollider2->m_worldTransform.inverse() * sphereCollider1->m_originPos;

				localTankRightSphereCenter.m_z -= 2.2f;
				localTankRightSphereCenter.m_y += 0.2f;
				localTankRightSphereCenter.m_x -= 0.7f;

				Vector3 closestPointToSphereCenter = localTankRightSphereCenter;

				//Now compute the closet point in BB to sphere center
				if(closestPointToSphereCenter.m_x < boxMin.m_x)
				{
					closestPointToSphereCenter.m_x = boxMin.m_x;
				}
				else if(closestPointToSphereCenter.m_x > boxMax.m_x)
				{
					closestPointToSphereCenter.m_x = boxMax.m_x;
				}

				if(closestPointToSphereCenter.m_y < boxMin.m_y)
				{
					closestPointToSphereCenter.m_y = boxMin.m_y;
				}
				else if(closestPointToSphereCenter.m_y > boxMax.m_y)
				{
					closestPointToSphereCenter.m_y = boxMax.m_y;
				}

				if(closestPointToSphereCenter.m_z < boxMin.m_z)
				{
					closestPointToSphereCenter.m_z = boxMin.m_z;
				}
				else if(closestPointToSphereCenter.m_z > boxMax.m_z)
				{
					closestPointToSphereCenter.m_z = boxMax.m_z;
				}

				//Now compute distance between closestPointToSphereCenter to sphereCenter
				float distanceSquared = (localTankRightSphereCenter - closestPointToSphereCenter).dotProduct(localTankRightSphereCenter - closestPointToSphereCenter);

				if(distanceSquared <= (radius * radius))
				{
					//OutputDebugStringA("Plane-Sphere collision detected\n");
					return true;
				}

			//return false;
			return false;
		}
		//SPHERE - PLANE
		if((strcmp(sphereCollider1->m_colliderType,"sphere") == 0) && (strcmp(planeCollider2->m_colliderType,"plane") == 0))
		{
			Matrix4x4 worldMatrixInverse = planeCollider2->m_worldTransform.inverse();
			Vector3 localPosOfBox = worldMatrixInverse * planeCollider2->m_originPos;
			Vector3 localSphereCenter = worldMatrixInverse * sphereCollider1->m_originPos;

			localSphereCenter.m_y += 0.6f;

			Vector3 boxMin = localPosOfBox + Vector3(-planeCollider2->m_boxWidth/2, 0, -planeCollider2->m_boxLength/2);
			Vector3 boxMax = localPosOfBox + Vector3(planeCollider2->m_boxWidth/2, planeCollider2->m_boxHeight/4, planeCollider2->m_boxLength/2);

					Vector3 point0 = localPosOfBox + Vector3(planeCollider2->m_boxWidth/2, 0, planeCollider2->m_boxLength/2);
					Vector3 point1 = localPosOfBox + Vector3(-planeCollider2->m_boxWidth/2, 0, planeCollider2->m_boxLength/2);
					Vector3 point2 = localPosOfBox + Vector3(-planeCollider2->m_boxWidth/2, 0, -planeCollider2->m_boxLength/2);
					Vector3 point3= localPosOfBox + Vector3(planeCollider2->m_boxWidth/2, 0, -planeCollider2->m_boxLength/2);

					Vector3 point4 = localPosOfBox + Vector3(planeCollider2->m_boxWidth/2, planeCollider2->m_boxHeight/4, planeCollider2->m_boxLength/2);
					Vector3 point5 = localPosOfBox + Vector3(-planeCollider2->m_boxWidth/2, planeCollider2->m_boxHeight/4, planeCollider2->m_boxLength/2);
					Vector3 point6 = localPosOfBox + Vector3(-planeCollider2->m_boxWidth/2, planeCollider2->m_boxHeight/4, -planeCollider2->m_boxLength/2);
					Vector3 point7 = localPosOfBox + Vector3(planeCollider2->m_boxWidth/2, planeCollider2->m_boxHeight/4, -planeCollider2->m_boxLength/2);

					point0 = planeCollider2->m_worldTransform * point0;
					point1 = planeCollider2->m_worldTransform * point1;
					point2 = planeCollider2->m_worldTransform * point2;
					point3 = planeCollider2->m_worldTransform * point3;
					point4 = planeCollider2->m_worldTransform * point4;
					point5 = planeCollider2->m_worldTransform * point5;
					point6 = planeCollider2->m_worldTransform * point6;
					point7 = planeCollider2->m_worldTransform * point7;

					Vector3 color(1.0f, 2.5f, 0.75f);
					
					Vector3 linepts1[] = {point0, color, point1, color};
					Vector3 linepts2[] = {point1, color, point2, color};
					Vector3 linepts3[] = {point2, color, point3, color};
					Vector3 linepts4[] = {point3, color, point0, color};

					Vector3 linepts5[] = {point4, color, point5, color};
					Vector3 linepts6[] = {point5, color, point6, color};
					Vector3 linepts7[] = {point6, color, point7, color};
					Vector3 linepts8[] = {point7, color, point4, color};

					Vector3 linepts9[] = {point0, color, point4, color};
					Vector3 linepts10[] = {point1, color, point5, color};
					Vector3 linepts11[] = {point2, color, point6, color};
					Vector3 linepts12[] = {point3, color, point7, color};

					Matrix4x4 dummy;

					DebugRenderer::Instance()->createLineMesh(false, dummy,  &linepts1[0].m_x, 2, DEBUG_DRAW_TIME);
					DebugRenderer::Instance()->createLineMesh(false, dummy,  &linepts2[0].m_x, 2, DEBUG_DRAW_TIME);
					DebugRenderer::Instance()->createLineMesh(false, dummy,  &linepts3[0].m_x, 2, DEBUG_DRAW_TIME);
					DebugRenderer::Instance()->createLineMesh(false, dummy,  &linepts4[0].m_x, 2, DEBUG_DRAW_TIME);
					DebugRenderer::Instance()->createLineMesh(false, dummy,  &linepts5[0].m_x, 2, DEBUG_DRAW_TIME);
					DebugRenderer::Instance()->createLineMesh(false, dummy,  &linepts6[0].m_x, 2, DEBUG_DRAW_TIME);
					DebugRenderer::Instance()->createLineMesh(false, dummy,  &linepts7[0].m_x, 2, DEBUG_DRAW_TIME);
					DebugRenderer::Instance()->createLineMesh(false, dummy,  &linepts8[0].m_x, 2, DEBUG_DRAW_TIME);
					DebugRenderer::Instance()->createLineMesh(false, dummy,  &linepts9[0].m_x, 2, DEBUG_DRAW_TIME);
					DebugRenderer::Instance()->createLineMesh(false, dummy,  &linepts10[0].m_x, 2, DEBUG_DRAW_TIME);
					DebugRenderer::Instance()->createLineMesh(false, dummy,  &linepts11[0].m_x, 2, DEBUG_DRAW_TIME);
					DebugRenderer::Instance()->createLineMesh(false, dummy,  &linepts12[0].m_x, 2, DEBUG_DRAW_TIME);

					//Drawing for Soldier
					float value = 0.6;
					Vector3 localSphereCenter2 = sphereCollider1->m_worldTransform.inverse() * sphereCollider1->m_originPos;
					 point0 = localSphereCenter2 + Vector3(value, 0, value);
					 point1 = localSphereCenter2 + Vector3(-value, 0, value);
					 point2 = localSphereCenter2 + Vector3(-value, 0, -value);
					 point3 = localSphereCenter2 + Vector3(value, 0, -value);

					 point4 = localSphereCenter2 + Vector3(value, value, value);
					 point5 = localSphereCenter2 + Vector3(-value, value, value);
					 point6 = localSphereCenter2 + Vector3(-value, value, -value);
					 point7 = localSphereCenter2 + Vector3(value, value, -value);

					point0 = sphereCollider1->m_worldTransform * point0;
					point1 = sphereCollider1->m_worldTransform * point1;
					point2 = sphereCollider1->m_worldTransform * point2;
					point3 = sphereCollider1->m_worldTransform * point3;
					point4 = sphereCollider1->m_worldTransform * point4;
					point5 = sphereCollider1->m_worldTransform * point5;
					point6 = sphereCollider1->m_worldTransform * point6;
					point7 = sphereCollider1->m_worldTransform * point7;

					Vector3 color1(1.5f, 0.5f, 0.1f);
					
					Vector3 linepts1_2[] = {point0, color1, point1, color1};
					Vector3 linepts2_2[] = {point1, color1, point2, color1};
					Vector3 linepts3_2[] = {point2, color1, point3, color1};
					Vector3 linepts4_2[] = {point3, color1, point0, color1};

					Vector3 linepts5_2[] = {point4, color1, point5, color1};
					Vector3 linepts6_2[] = {point5, color1, point6, color1};
					Vector3 linepts7_2[] = {point6, color1, point7, color1};
					Vector3 linepts8_2[] = {point7, color1, point4, color1};

					Vector3 linepts9_2[] = {point0, color1, point4, color1};
					Vector3 linepts10_2[] = {point1, color1, point5, color1};
					Vector3 linepts11_2[] = {point2, color1, point6, color1};
					Vector3 linepts12_2[] = {point3, color1, point7, color1};

					//Matrix4x4 dummy;

					DebugRenderer::Instance()->createLineMesh(false, dummy,  &linepts1_2[0].m_x, 2, DEBUG_DRAW_TIME);
					DebugRenderer::Instance()->createLineMesh(false, dummy,  &linepts2_2[0].m_x, 2, DEBUG_DRAW_TIME);
					DebugRenderer::Instance()->createLineMesh(false, dummy,  &linepts3_2[0].m_x, 2, DEBUG_DRAW_TIME);
					DebugRenderer::Instance()->createLineMesh(false, dummy,  &linepts4_2[0].m_x, 2, DEBUG_DRAW_TIME);
					DebugRenderer::Instance()->createLineMesh(false, dummy,  &linepts5_2[0].m_x, 2, DEBUG_DRAW_TIME);
					DebugRenderer::Instance()->createLineMesh(false, dummy,  &linepts6_2[0].m_x, 2, DEBUG_DRAW_TIME);
					DebugRenderer::Instance()->createLineMesh(false, dummy,  &linepts7_2[0].m_x, 2, DEBUG_DRAW_TIME);
					DebugRenderer::Instance()->createLineMesh(false, dummy,  &linepts8_2[0].m_x, 2, DEBUG_DRAW_TIME);
					DebugRenderer::Instance()->createLineMesh(false, dummy,  &linepts9_2[0].m_x, 2, DEBUG_DRAW_TIME);
					DebugRenderer::Instance()->createLineMesh(false, dummy,  &linepts10_2[0].m_x, 2, DEBUG_DRAW_TIME);
					DebugRenderer::Instance()->createLineMesh(false, dummy,  &linepts11_2[0].m_x, 2, DEBUG_DRAW_TIME);
					DebugRenderer::Instance()->createLineMesh(false, dummy,  &linepts12_2[0].m_x, 2, DEBUG_DRAW_TIME);

			Vector3 closestPointToSphereCenter = localSphereCenter;

			//Now compute the closet point in BB to sphere center
			if(closestPointToSphereCenter.m_x < boxMin.m_x)
			{
				closestPointToSphereCenter.m_x = boxMin.m_x;
			}
			else if(closestPointToSphereCenter.m_x > boxMax.m_x)
			{
				closestPointToSphereCenter.m_x = boxMax.m_x;
			}

			if(closestPointToSphereCenter.m_y < boxMin.m_y)
			{
				closestPointToSphereCenter.m_y = boxMin.m_y;
			}
			else if(closestPointToSphereCenter.m_y > boxMax.m_y)
			{
				closestPointToSphereCenter.m_y = boxMax.m_y;
			}

			if(closestPointToSphereCenter.m_z < boxMin.m_z)
			{
				closestPointToSphereCenter.m_z = boxMin.m_z;
			}
			else if(closestPointToSphereCenter.m_z > boxMax.m_z)
			{
				closestPointToSphereCenter.m_z = boxMax.m_z;
			}

			//Now compute distance between closestPointToSphereCenter to sphereCenter
			float distanceSquared = (localSphereCenter - closestPointToSphereCenter).dotProduct(localSphereCenter - closestPointToSphereCenter);
			//printf("soldierDistanceSquared : %f soldierRadius %d\n",distanceSquared,sphereCollider1->m_sphereRadius * sphereCollider1->m_sphereRadius);
			if(distanceSquared <= (sphereCollider1->m_sphereRadius * sphereCollider1->m_sphereRadius))
			{
				//OutputDebugStringA("Plane-Sphere collision detected\n");
				return true;
			}
		}

		return false;
	}

}; // namespace Components
}; // namespace PE
