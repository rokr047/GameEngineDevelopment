#ifndef _PE_PHYSICS_DEBUGLINES_H_
#define _PE_PHYSICS_DEBUGLINES_H_

#include "PrimeEngine/Math/Vector3.h"
#include "PrimeEngine/Math/Vector4.h"
#include "PrimeEngine/Math/Matrix4x4.h"
#include "PrimeEngine/Scene/DebugRenderer.h"
#include <assert.h>
#define DEBUG_DRAW_TIME 2

namespace PE
{
	namespace Components
	{
		class DebugLines
		{
			// Individual events ------------------------------------------------------
		public:
			static void drawDebugLinesForBox(Vector3 point0,Vector3 point1,Vector3 point2,Vector3 point3,Vector3 point4,Vector3 point5,Vector3 point6,Vector3 point7,Vector3 color);
		};
	};
};

#endif