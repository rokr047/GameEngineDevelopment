#include "Physics.h"
#include "PrimeEngine/Lua/LuaEnvironment.h"
#include "PrimeEngine/Scene/DebugRenderer.h"

namespace PE{
namespace Components{

	PE_IMPLEMENT_CLASS1(Physics, Component);

	Physics::Physics(PE::GameContext &context, PE::MemoryArena arena, Handle hMyself)
	: Component(context, arena, hMyself)
	{
	
	}

	void Physics::addDefaultComponents()
	{
		Component::addDefaultComponents();
	}

	bool Physics::CheckCollision(PhysicsComponent *Collider1, PhysicsComponent *Collider2)
	{
		//Check what type of collider both the Physics Components have

		//SPHERE - SPHERE
		if((strcmp(Collider1->m_colliderType,"sphere") == 0) && (strcmp(Collider2->m_colliderType,"sphere") == 0))
		{
			Vector3 soldierPos1 = Collider1->m_originPos;
			Vector3 soldierPos2 = Collider2->m_originPos;

			Vector3 soldierPos1_2 = Collider1->m_originPos;
			Vector3 soldierPos2_2 = Collider2->m_originPos;

			int colliderRadius1 = Collider1->m_sphereRadius;
			int colliderRadius2 = Collider2->m_sphereRadius;
	
			soldierPos1.m_y += 1.2f;
			soldierPos2.m_y += 1.2f;

			soldierPos1_2.m_y += 0.2f;
			soldierPos2_2.m_y += 0.2f;

			int distanceSquared = (soldierPos1 - soldierPos2).dotProduct(soldierPos1 - soldierPos2);
			int radiusSumSquared = (colliderRadius1 + colliderRadius2) * (colliderRadius1 + colliderRadius2);

			Matrix4x4 worldMatrixInverse = Collider2->m_worldTransform.inverse();
			Vector3 localPosOfBox = worldMatrixInverse * soldierPos2;

					float value = 0.6;
					Vector3 point0 = localPosOfBox + Vector3(value, 0, value);
					Vector3 point1 = localPosOfBox + Vector3(-value, 0, value);
					Vector3 point2 = localPosOfBox + Vector3(-value, 0, -value);
					Vector3 point3 = localPosOfBox + Vector3(value, 0, -value);

					Vector3 point4 = localPosOfBox + Vector3(value, value, value);
					Vector3 point5 = localPosOfBox + Vector3(-value, value, value);
					Vector3 point6 = localPosOfBox + Vector3(-value, value, -value);
					Vector3 point7 = localPosOfBox + Vector3(value, value, -value);

					point0 = Collider2->m_worldTransform * point0;
					point1 = Collider2->m_worldTransform * point1;
					point2 = Collider2->m_worldTransform * point2;
					point3 = Collider2->m_worldTransform * point3;
					point4 = Collider2->m_worldTransform * point4;
					point5 = Collider2->m_worldTransform * point5;
					point6 = Collider2->m_worldTransform * point6;
					point7 = Collider2->m_worldTransform * point7;

					Vector3 color(0.45f, 0.5f, 1.1f);
					
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

					DebugRenderer::Instance()->createLineMesh(false, dummy,  &linepts1[0].m_x, 2, 10);
					DebugRenderer::Instance()->createLineMesh(false, dummy,  &linepts2[0].m_x, 2, 10);
					DebugRenderer::Instance()->createLineMesh(false, dummy,  &linepts3[0].m_x, 2, 10);
					DebugRenderer::Instance()->createLineMesh(false, dummy,  &linepts4[0].m_x, 2, 10);
					DebugRenderer::Instance()->createLineMesh(false, dummy,  &linepts5[0].m_x, 2, 10);
					DebugRenderer::Instance()->createLineMesh(false, dummy,  &linepts6[0].m_x, 2, 10);
					DebugRenderer::Instance()->createLineMesh(false, dummy,  &linepts7[0].m_x, 2, 10);
					DebugRenderer::Instance()->createLineMesh(false, dummy,  &linepts8[0].m_x, 2, 10);
					DebugRenderer::Instance()->createLineMesh(false, dummy,  &linepts9[0].m_x, 2, 10);
					DebugRenderer::Instance()->createLineMesh(false, dummy,  &linepts10[0].m_x, 2, 10);
					DebugRenderer::Instance()->createLineMesh(false, dummy,  &linepts11[0].m_x, 2, 10);
					DebugRenderer::Instance()->createLineMesh(false, dummy,  &linepts12[0].m_x, 2, 10);

			if(distanceSquared < radiusSumSquared)
			{
				return true;
			}
		}
		//SPHERE - BOX
		else if((strcmp(Collider1->m_colliderType,"sphere") == 0) && (strcmp(Collider2->m_colliderType,"box") == 0))
		{
			Matrix4x4 worldMatrixInverse = Collider2->m_worldTransform.inverse();
			Vector3 localPosOfBox = worldMatrixInverse * Collider2->m_originPos;
			Vector3 localSphereCenter = worldMatrixInverse * Collider1->m_originPos;

			localSphereCenter.m_y += 1.2f;

			Vector3 boxMin = localPosOfBox + Vector3(-Collider2->m_boxWidth/2, 0, -Collider2->m_boxLength/2);
			Vector3 boxMax = localPosOfBox + Vector3(Collider2->m_boxWidth/2, Collider2->m_boxHeight, Collider2->m_boxLength/2);
				
					//Drawing for Mesh
					Vector3 point0 = localPosOfBox + Vector3(Collider2->m_boxWidth/2, 0, Collider2->m_boxLength/2);
					Vector3 point1 = localPosOfBox + Vector3(-Collider2->m_boxWidth/2, 0, Collider2->m_boxLength/2);
					Vector3 point2 = localPosOfBox + Vector3(-Collider2->m_boxWidth/2, 0, -Collider2->m_boxLength/2);
					Vector3 point3= localPosOfBox + Vector3(Collider2->m_boxWidth/2, 0, -Collider2->m_boxLength/2);

					Vector3 point4 = localPosOfBox + Vector3(Collider2->m_boxWidth/2, Collider2->m_boxHeight, Collider2->m_boxLength/2);
					Vector3 point5 = localPosOfBox + Vector3(-Collider2->m_boxWidth/2, Collider2->m_boxHeight, Collider2->m_boxLength/2);
					Vector3 point6 = localPosOfBox + Vector3(-Collider2->m_boxWidth/2, Collider2->m_boxHeight, -Collider2->m_boxLength/2);
					Vector3 point7 = localPosOfBox + Vector3(Collider2->m_boxWidth/2, Collider2->m_boxHeight, -Collider2->m_boxLength/2);

					point0 = Collider2->m_worldTransform * point0;
					point1 = Collider2->m_worldTransform * point1;
					point2 = Collider2->m_worldTransform * point2;
					point3 = Collider2->m_worldTransform * point3;
					point4 = Collider2->m_worldTransform * point4;
					point5 = Collider2->m_worldTransform * point5;
					point6 = Collider2->m_worldTransform * point6;
					point7 = Collider2->m_worldTransform * point7;

					Vector3 color(0.25f, 1.75f, 0.10f);
					
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

					DebugRenderer::Instance()->createLineMesh(false, dummy,  &linepts1[0].m_x, 2, 10);
					DebugRenderer::Instance()->createLineMesh(false, dummy,  &linepts2[0].m_x, 2, 10);
					DebugRenderer::Instance()->createLineMesh(false, dummy,  &linepts3[0].m_x, 2, 10);
					DebugRenderer::Instance()->createLineMesh(false, dummy,  &linepts4[0].m_x, 2, 10);
					DebugRenderer::Instance()->createLineMesh(false, dummy,  &linepts5[0].m_x, 2, 10);
					DebugRenderer::Instance()->createLineMesh(false, dummy,  &linepts6[0].m_x, 2, 10);
					DebugRenderer::Instance()->createLineMesh(false, dummy,  &linepts7[0].m_x, 2, 10);
					DebugRenderer::Instance()->createLineMesh(false, dummy,  &linepts8[0].m_x, 2, 10);
					DebugRenderer::Instance()->createLineMesh(false, dummy,  &linepts9[0].m_x, 2, 10);
					DebugRenderer::Instance()->createLineMesh(false, dummy,  &linepts10[0].m_x, 2, 10);
					DebugRenderer::Instance()->createLineMesh(false, dummy,  &linepts11[0].m_x, 2, 10);
					DebugRenderer::Instance()->createLineMesh(false, dummy,  &linepts12[0].m_x, 2, 10);


					//Drawing for soldier
					 float value = 0.6;
					 Vector3 localShereCenter2 = Collider1->m_worldTransform.inverse() * Collider1->m_originPos;
					 localShereCenter2.m_y += 1.2f;
					 point0 = localShereCenter2 + Vector3(value, 0, value);
					 point1 = localShereCenter2 + Vector3(-value, 0, value);
					 point2 = localShereCenter2 + Vector3(-value, 0, -value);
					 point3 = localShereCenter2 + Vector3(value, 0, -value);

					 point4 = localShereCenter2 + Vector3(value, value, value);
					 point5 = localShereCenter2 + Vector3(-value, value, value);
					 point6 = localShereCenter2 + Vector3(-value, value, -value);
					 point7 = localShereCenter2 + Vector3(value, value, -value);

					point0 = Collider1->m_worldTransform * point0;
					point1 = Collider1->m_worldTransform * point1;
					point2 = Collider1->m_worldTransform * point2;
					point3 = Collider1->m_worldTransform * point3;
					point4 = Collider1->m_worldTransform * point4;
					point5 = Collider1->m_worldTransform * point5;
					point6 = Collider1->m_worldTransform * point6;
					point7 = Collider1->m_worldTransform * point7;

					Vector3 color1(0.45f, 0.5f, 1.1f);
					
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

					DebugRenderer::Instance()->createLineMesh(false, dummy,  &linepts1_2[0].m_x, 2, 10);
					DebugRenderer::Instance()->createLineMesh(false, dummy,  &linepts2_2[0].m_x, 2, 10);
					DebugRenderer::Instance()->createLineMesh(false, dummy,  &linepts3_2[0].m_x, 2, 10);
					DebugRenderer::Instance()->createLineMesh(false, dummy,  &linepts4_2[0].m_x, 2, 10);
					DebugRenderer::Instance()->createLineMesh(false, dummy,  &linepts5_2[0].m_x, 2, 10);
					DebugRenderer::Instance()->createLineMesh(false, dummy,  &linepts6_2[0].m_x, 2, 10);
					DebugRenderer::Instance()->createLineMesh(false, dummy,  &linepts7_2[0].m_x, 2, 10);
					DebugRenderer::Instance()->createLineMesh(false, dummy,  &linepts8_2[0].m_x, 2, 10);
					DebugRenderer::Instance()->createLineMesh(false, dummy,  &linepts9_2[0].m_x, 2, 10);
					DebugRenderer::Instance()->createLineMesh(false, dummy,  &linepts10_2[0].m_x, 2, 10);
					DebugRenderer::Instance()->createLineMesh(false, dummy,  &linepts11_2[0].m_x, 2, 10);
					DebugRenderer::Instance()->createLineMesh(false, dummy,  &linepts12_2[0].m_x, 2, 10);

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

			if(distanceSquared <= (Collider1->m_sphereRadius * Collider1->m_sphereRadius))
			{
				//OutputDebugStringA("Box-Sphere collision detected\n");
				return true;
			}
		}

		return false;
	}
};	// namespace PE
};	// namespace Components