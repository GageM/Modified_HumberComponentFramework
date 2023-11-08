#pragma once

#include <glew.h>
#include <vector>
#include <Vector.h>
#include <Quaternion.h>

using namespace MATH;

class DrawDebug
{
public:
	// 1D Primitives

	static void DrawLine(const Vec3& start, const Vec3& end);

	static void DrawRay(const Vec3& start, const Vec3& dir, const float& length = 1000.0f);

	// 2D Primitives

	static void DrawCircle(const Vec3& pos, const Quaternion& orientation, const float& r, const int& segments);

	static void DrawArc(const Vec3& pos, const Quaternion& orientation, const float& startAngle, const float& endAngle, const float& radius, const int& segments);

	static void DrawTriangle(const Vec3& v1, const Vec3& v2, const Vec3& v3);

	static void DrawRectangle(const Vec3& pos, const Quaternion& orientation, const Vec2& halfExtents);

	// 3D Primitives
	
	static void DrawSphere(const Vec3& pos, const Quaternion& orientation, const float& r, const int& segments);

	static void DrawBox(const Vec3& pos, const Quaternion& orientation, const Vec3& halfExtents);

	static void DrawCylinder(const Vec3& capA, const Vec3& capB, const Quaternion& orientation, const float& r, const int& segments);

	static void DrawCapsule(const Vec3& capA, const Vec3& capB, const Quaternion& orientation, const float& r, const int& segments);

private:

	static void StoreMeshData(GLenum drawmode, std::vector<Vec3>& vertices, std::vector<Vec3>& normals, GLuint& vao, GLuint& vbo, size_t& dataLength);
};

