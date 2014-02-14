#include "DebugLines.h"

namespace PE
{
	namespace Components
	{
		void DebugLines::drawDebugLinesForBox(Vector3 point0,Vector3 point1,Vector3 point2,Vector3 point3,Vector3 point4,Vector3 point5,Vector3 point6,Vector3 point7,Vector3 color)
		{
			Vector3 linepts1[] = {point0, color,point1, color};
			Vector3 linepts2[] = {point1, color,point2, color};
			Vector3 linepts3[] = {point2, color,point3, color};
			Vector3 linepts4[] = {point3, color,point0, color};

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
		}
	};	// namespace PE
};	// namespace Components