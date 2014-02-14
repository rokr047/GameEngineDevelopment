#include "Physics.h"
#include "PrimeEngine/Lua/LuaEnvironment.h"
#include "PrimeEngine/Scene/DebugRenderer.h"
#include "PrimeEngine/Physics/DebugLines/DebugLines.h"
#define DEBUG_DRAW_TIME 10

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
			return SphereSphereCollision(Collider1, Collider2);			
		}
		//SPHERE - BOX
		else if((strcmp(Collider1->m_colliderType,"sphere") == 0) && (strcmp(Collider2->m_colliderType,"box") == 0))
		{
			return SphereBoxCollision(Collider1, Collider2);			
		}
		else if((strcmp(Collider1->m_colliderType,"box") == 0) && (strcmp(Collider2->m_colliderType,"sphere") == 0))
		{
			return SphereBoxCollision(Collider2, Collider1);			
		}
		// BOX - BOX
		else if((strcmp(Collider1->m_colliderType,"box") == 0) && (strcmp(Collider2->m_colliderType,"box") == 0))
		{
			return BoxBoxCollision(Collider1, Collider2); 			
		}
		// PlayerCar - BOX
		else if((strcmp(Collider1->m_colliderType,"playercar") == 0) && (strcmp(Collider2->m_colliderType,"box") == 0))
		{
			return BoxBoxCollision(Collider1, Collider2);			
		}
		// BOX - PlayerCar
		else if((strcmp(Collider1->m_colliderType,"box") == 0) && (strcmp(Collider2->m_colliderType,"playercar") == 0))
		{
			return BoxBoxCollision(Collider2, Collider1);			
		}
		//SPHERE - PlayerCar
		else if((strcmp(Collider1->m_colliderType,"sphere") == 0) && (strcmp(Collider2->m_colliderType,"playercar") == 0))
		{
			return SphereCarCollision(Collider1, Collider2);			
		}
		else if((strcmp(Collider1->m_colliderType,"playercar") == 0) && (strcmp(Collider2->m_colliderType,"sphere") == 0))
		{
			return SphereCarCollision(Collider2, Collider1);
		}
	
		return false;
	}

	bool Physics::SphereSphereCollision(PhysicsComponent *Collider1, PhysicsComponent *Collider2)
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
			Vector3 point0 = Collider2->m_worldTransform * (localPosOfBox + Vector3(value, 0, value));
			Vector3 point1 = Collider2->m_worldTransform * (localPosOfBox + Vector3(-value, 0, value));
			Vector3 point2 = Collider2->m_worldTransform * (localPosOfBox + Vector3(-value, 0, -value));
			Vector3 point3 = Collider2->m_worldTransform * (localPosOfBox + Vector3(value, 0, -value));

			Vector3 point4 = Collider2->m_worldTransform * (localPosOfBox + Vector3(value, value, value));
			Vector3 point5 = Collider2->m_worldTransform * (localPosOfBox + Vector3(-value, value, value));
			Vector3 point6 = Collider2->m_worldTransform * (localPosOfBox + Vector3(-value, value, -value));
			Vector3 point7 = Collider2->m_worldTransform * (localPosOfBox + Vector3(value, value, -value));

			Vector3 color(0.45f, 0.5f, 1.1f);

			DebugLines::drawDebugLinesForBox(point0,point1,point2,point3,point4,point5,point6,point7,color);
										

			if(distanceSquared < radiusSumSquared)
			{
				return true;
			}

			return false;

	}

	bool Physics::BoxBoxCollision(PhysicsComponent *Collider1, PhysicsComponent *Collider2)
	{
		Matrix4x4 worldMatrixInverse = Collider2->m_worldTransform.inverse();

			Vector3 localPosOfBox1 = worldMatrixInverse *  Collider1->m_originPos;
			Vector3 localPosOfBox2 = worldMatrixInverse * Collider2->m_originPos;

			Vector3 box1_Min = localPosOfBox1 + Vector3(-Collider1->m_boxWidth/2, 0, -Collider1->m_boxLength/2);
			Vector3 box1_Max = localPosOfBox1 + Vector3(Collider1->m_boxWidth/2, Collider1->m_boxHeight, Collider1->m_boxLength/2);

			Matrix4x4 worldMatrixInverseOfCollider1 = Collider1->m_worldTransform.inverse();
			Vector3 localPosOfBox1Collider1 = worldMatrixInverseOfCollider1 * Collider1->m_originPos;

			//Drawing for Box1
			Vector3 point0 = Collider1->m_worldTransform * (localPosOfBox1Collider1 + Vector3(Collider1->m_boxWidth/2, 0, Collider1->m_boxLength/2));
			Vector3 point1 = Collider1->m_worldTransform * (localPosOfBox1Collider1 + Vector3(-Collider1->m_boxWidth/2, 0, Collider1->m_boxLength/2));
			Vector3 point2 = Collider1->m_worldTransform * (localPosOfBox1Collider1 + Vector3(-Collider1->m_boxWidth/2, 0, -Collider1->m_boxLength/2));
			Vector3 point3 = Collider1->m_worldTransform * (localPosOfBox1Collider1 + Vector3(Collider1->m_boxWidth/2, 0, -Collider1->m_boxLength/2));

			Vector3 point4 = Collider1->m_worldTransform * (localPosOfBox1Collider1 + Vector3(Collider1->m_boxWidth/2, Collider1->m_boxHeight, Collider1->m_boxLength/2));
			Vector3 point5 = Collider1->m_worldTransform * (localPosOfBox1Collider1 + Vector3(-Collider1->m_boxWidth/2, Collider1->m_boxHeight, Collider1->m_boxLength/2));
			Vector3 point6 = Collider1->m_worldTransform * (localPosOfBox1Collider1 + Vector3(-Collider1->m_boxWidth/2, Collider1->m_boxHeight, -Collider1->m_boxLength/2));
			Vector3 point7 = Collider1->m_worldTransform * (localPosOfBox1Collider1 + Vector3(Collider1->m_boxWidth/2, Collider1->m_boxHeight, -Collider1->m_boxLength/2));

			Vector3 color(0.25f, 1.75f, 0.10f);

			DebugLines::drawDebugLinesForBox(point0,point1,point2,point3,point4,point5,point6,point7,color);			

			//Drawing for Box2

			Vector3 box2_Min = localPosOfBox2 + Vector3(-Collider2->m_boxWidth/2, 0, -Collider2->m_boxLength/2);
			Vector3 box2_Max = localPosOfBox2 + Vector3(Collider2->m_boxWidth/2, Collider2->m_boxHeight, Collider2->m_boxLength/2);

			Vector3 _point0 = Collider2->m_worldTransform * (localPosOfBox2 + Vector3(Collider2->m_boxWidth/2, 0, Collider2->m_boxLength/2));
			Vector3 _point1 = Collider2->m_worldTransform * (localPosOfBox2 + Vector3(-Collider2->m_boxWidth/2, 0, Collider2->m_boxLength/2));
			Vector3 _point2 = Collider2->m_worldTransform * (localPosOfBox2 + Vector3(-Collider2->m_boxWidth/2, 0, -Collider2->m_boxLength/2));
			Vector3 _point3 = Collider2->m_worldTransform * (localPosOfBox2 + Vector3(Collider2->m_boxWidth/2, 0, -Collider2->m_boxLength/2));

			Vector3 _point4 = Collider2->m_worldTransform * (localPosOfBox2 + Vector3(Collider2->m_boxWidth/2, Collider2->m_boxHeight, Collider2->m_boxLength/2));
			Vector3 _point5 = Collider2->m_worldTransform * (localPosOfBox2 + Vector3(-Collider2->m_boxWidth/2, Collider2->m_boxHeight, Collider2->m_boxLength/2));
			Vector3 _point6 = Collider2->m_worldTransform * (localPosOfBox2 + Vector3(-Collider2->m_boxWidth/2, Collider2->m_boxHeight, -Collider2->m_boxLength/2));
			Vector3 _point7 = Collider2->m_worldTransform * (localPosOfBox2 + Vector3(Collider2->m_boxWidth/2, Collider2->m_boxHeight, -Collider2->m_boxLength/2));
		
			Vector3 _color(0.25f, 1.75f, 0.10f);

			DebugLines::drawDebugLinesForBox(_point0,_point1,_point2,_point3,_point4,_point5,_point6,_point7,_color);
			
			bool collisionCheck1, collisionCheck2, collisionCheck3, collisionCheck4, collisionCheck5, collisionCheck6;

			collisionCheck1 = collisionCheck2 = collisionCheck3 = collisionCheck4 = collisionCheck5 = collisionCheck6 = true;
			
			if(box1_Min.m_x > box2_Max.m_x)
			{
				collisionCheck1 = false;
			}
			if(box1_Max.m_x < box2_Min.m_x)
			{
				collisionCheck2 = false;
			}
			if(box1_Min.m_y > box2_Max.m_y)
			{
				collisionCheck3 = false;
			}
			if(box1_Max.m_y < box2_Min.m_y)
			{
				collisionCheck4 = false;
			}
			if(box1_Min.m_z > box2_Max.m_z)
			{
				collisionCheck3 = false;
			}
			if(box1_Max.m_z < box2_Min.m_z)
			{
				collisionCheck4 = false;
			}

			if(collisionCheck1 && collisionCheck2 && collisionCheck3 && collisionCheck4 && collisionCheck5 && collisionCheck6)
			{
				//OutputDebugStringA("Box-Box collision detected\n");
				return true;
			}

			return false;

	}

	bool Physics::SphereBoxCollision(PhysicsComponent *Collider1, PhysicsComponent *Collider2)
	{
			Matrix4x4 worldMatrixInverse = Collider2->m_worldTransform.inverse();
			Vector3 localPosOfBox = worldMatrixInverse * Collider2->m_originPos;
			Vector3 localSphereCenter = worldMatrixInverse * Collider1->m_originPos;

			localSphereCenter.m_y += 1.2f;

			Vector3 boxMin = localPosOfBox + Vector3(-Collider2->m_boxWidth/2, 0, -Collider2->m_boxLength/2);
			Vector3 boxMax = localPosOfBox + Vector3(Collider2->m_boxWidth/2, Collider2->m_boxHeight, Collider2->m_boxLength/2);
				
			//Drawing for Mesh
			Vector3 point0 = Collider2->m_worldTransform * (localPosOfBox + Vector3(Collider2->m_boxWidth/2, 0, Collider2->m_boxLength/2));
			Vector3 point1 = Collider2->m_worldTransform * (localPosOfBox + Vector3(-Collider2->m_boxWidth/2, 0, Collider2->m_boxLength/2));
			Vector3 point2 = Collider2->m_worldTransform * (localPosOfBox + Vector3(-Collider2->m_boxWidth/2, 0, -Collider2->m_boxLength/2));
			Vector3 point3 = Collider2->m_worldTransform * (localPosOfBox + Vector3(Collider2->m_boxWidth/2, 0, -Collider2->m_boxLength/2));

			Vector3 point4 = Collider2->m_worldTransform * (localPosOfBox + Vector3(Collider2->m_boxWidth/2, Collider2->m_boxHeight, Collider2->m_boxLength/2));
			Vector3 point5 = Collider2->m_worldTransform * (localPosOfBox + Vector3(-Collider2->m_boxWidth/2, Collider2->m_boxHeight, Collider2->m_boxLength/2));
			Vector3 point6 = Collider2->m_worldTransform * (localPosOfBox + Vector3(-Collider2->m_boxWidth/2, Collider2->m_boxHeight, -Collider2->m_boxLength/2));
			Vector3 point7 = Collider2->m_worldTransform * (localPosOfBox + Vector3(Collider2->m_boxWidth/2, Collider2->m_boxHeight, -Collider2->m_boxLength/2));

			Vector3 color(0.25f, 1.75f, 0.10f);

			DebugLines::drawDebugLinesForBox(point0,point1,point2,point3,point4,point5,point6,point7,color);
				
			//Drawing for soldier
			float value = 0.6;
			Vector3 localShereCenter2 = Collider1->m_worldTransform.inverse() * Collider1->m_originPos;
			localShereCenter2.m_y += 1.2f;
			point0 = Collider1->m_worldTransform * (localShereCenter2 + Vector3(value, 0, value));
			point1 = Collider1->m_worldTransform * (localShereCenter2 + Vector3(-value, 0, value));
			point2 = Collider1->m_worldTransform * (localShereCenter2 + Vector3(-value, 0, -value));
			point3 = Collider1->m_worldTransform * (localShereCenter2 + Vector3(value, 0, -value));

			point4 = Collider1->m_worldTransform * (localShereCenter2 + Vector3(value, value, value));
			point5 = Collider1->m_worldTransform * (localShereCenter2 + Vector3(-value, value, value));
			point6 = Collider1->m_worldTransform * (localShereCenter2 + Vector3(-value, value, -value));
			point7 = Collider1->m_worldTransform * (localShereCenter2 + Vector3(value, value, -value));

			Vector3 color1(0.45f, 0.5f, 1.1f);
					
			DebugLines::drawDebugLinesForBox(point0,point1,point2,point3,point4,point5,point6,point7,color1);

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

			return false;
	}

	bool Physics::SphereCarCollision(PhysicsComponent *Collider1, PhysicsComponent *Collider2)
	{
			Matrix4x4 worldMatrixInverse = Collider2->m_worldTransform.inverse();
			Vector3 localPosOfBox = worldMatrixInverse * Collider2->m_originPos;
			Vector3 localSphereCenter = worldMatrixInverse * Collider1->m_originPos;

			localSphereCenter.m_y += 1.2f;

			Vector3 boxMin = localPosOfBox + Vector3(-Collider2->m_boxWidth/2, 0, -Collider2->m_boxLength/2);
			Vector3 boxMax = localPosOfBox + Vector3(Collider2->m_boxWidth/2, Collider2->m_boxHeight, Collider2->m_boxLength/2);
				
			//Drawing for Car
			Vector3 point0 = Collider2->m_worldTransform * (localPosOfBox + Vector3(Collider2->m_boxWidth/2, 0, Collider2->m_boxLength/2));
			Vector3 point1 = Collider2->m_worldTransform * (localPosOfBox + Vector3(-Collider2->m_boxWidth/2, 0, Collider2->m_boxLength/2));
			Vector3 point2 = Collider2->m_worldTransform * (localPosOfBox + Vector3(-Collider2->m_boxWidth/2, 0, -Collider2->m_boxLength/2));
			Vector3 point3 = Collider2->m_worldTransform * (localPosOfBox + Vector3(Collider2->m_boxWidth/2, 0, -Collider2->m_boxLength/2));

			Vector3 point4 = Collider2->m_worldTransform * (localPosOfBox + Vector3(Collider2->m_boxWidth/2, Collider2->m_boxHeight, Collider2->m_boxLength/2));
			Vector3 point5 = Collider2->m_worldTransform * (localPosOfBox + Vector3(-Collider2->m_boxWidth/2, Collider2->m_boxHeight, Collider2->m_boxLength/2));
			Vector3 point6 = Collider2->m_worldTransform * (localPosOfBox + Vector3(-Collider2->m_boxWidth/2, Collider2->m_boxHeight, -Collider2->m_boxLength/2));
			Vector3 point7 = Collider2->m_worldTransform * (localPosOfBox + Vector3(Collider2->m_boxWidth/2, Collider2->m_boxHeight, -Collider2->m_boxLength/2));

			Vector3 color(0.25f, 1.75f, 0.10f);

			DebugLines::drawDebugLinesForBox(point0,point1,point2,point3,point4,point5,point6,point7,color);
				
			//Drawing for sphere colliders
			float value = 0.6;
			Vector3 localShereCenter2 = Collider1->m_worldTransform.inverse() * Collider1->m_originPos;
			localShereCenter2.m_y += 1.2f;
			point0 = Collider1->m_worldTransform * (localShereCenter2 + Vector3(value, 0, value));
			point1 = Collider1->m_worldTransform * (localShereCenter2 + Vector3(-value, 0, value));
			point2 = Collider1->m_worldTransform * (localShereCenter2 + Vector3(-value, 0, -value));
			point3 = Collider1->m_worldTransform * (localShereCenter2 + Vector3(value, 0, -value));

			point4 = Collider1->m_worldTransform * (localShereCenter2 + Vector3(value, value, value));
			point5 = Collider1->m_worldTransform * (localShereCenter2 + Vector3(-value, value, value));
			point6 = Collider1->m_worldTransform * (localShereCenter2 + Vector3(-value, value, -value));
			point7 = Collider1->m_worldTransform * (localShereCenter2 + Vector3(value, value, -value));

			Vector3 color1(0.45f, 0.5f, 1.1f);
					
			DebugLines::drawDebugLinesForBox(point0,point1,point2,point3,point4,point5,point6,point7,color1);

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
				OutputDebugStringA("PlayerCar-Sphere collision detected\n");
				return true;
			}

		return false;
	}


};	// namespace PE
};	// namespace Components