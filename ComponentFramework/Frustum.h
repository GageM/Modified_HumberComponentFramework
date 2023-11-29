#pragma once
#include <Plane.h>
#include <Matrix.h>
#include <vector>

using namespace MATH;

class Frustum
{
public:
	Frustum() {}

	Frustum(Matrix4 projection, Matrix4 view);
	
	void Set(Matrix4 projection, Matrix4 view);

	void Normalize();

	float DistanceFunction(const Plane& plane, const Vec3& point);

	bool InFrustum(const Vec3& point);

	// 0:Left, 1:Right, 2:Top, 3:Bottom, 4:Near, 5:Far
	Plane LP, RP, TP, BP, NP, FP;

private:
};

