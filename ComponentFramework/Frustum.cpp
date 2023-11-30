#include "Frustum.h"
#include <Vector.h>
#include <VMath.h>
#include <MMath.h>
#include <PMath.h>


Frustum::Frustum(Matrix4 projection, Matrix4 view)
{
	Set(projection, view);
}

void Frustum::Set(Matrix4 projection, Matrix4 view)
{
	Matrix4 m = view * projection;

	// Left Plane
	LP = Plane(m[12] + m[0], m[13] + m[1], m[14] + m[2], m[15] + m[3]);
	// Right Plane
	RP = Plane(m[12] - m[0], m[13] - m[1], m[14] - m[2], m[15] - m[3]);
	// Top Plane
	TP = Plane(m[12] - m[4], m[13] - m[5], m[14] - m[6], m[15] - m[7]);
	// Bottom Plane
	BP = Plane(m[12] + m[4], m[13] + m[5], m[14] + m[6], m[15] + m[7]);
	// Near Plane
	NP = Plane(m[12] + m[8], m[13] + m[9], m[14] + m[10], m[15] + m[11]);
	// Far Plane
	FP = Plane(m[12] - m[8], m[13] - m[9], m[14] - m[10], m[15] - m[11]);

	Normalize();
}

void Frustum::Normalize()
{
	PMath::normalize(LP);
	PMath::normalize(RP);
	PMath::normalize(TP);
	PMath::normalize(BP);
	PMath::normalize(NP);
	PMath::normalize(FP);
}

float Frustum::DistanceFunction(const Plane& plane, const Vec3& point)
{
	return plane.x * point.x + plane.y * point.y + plane.z * point.z + plane.d;
}

bool Frustum::InFrustum(const Vec3& point)
{
	// Check if the point is in the positive normal direction of each Frustum plane
	if ((DistanceFunction(LP, point)) < 0.0f)	return false;
	if ((DistanceFunction(RP, point)) < 0.0f)	return false;
	if ((DistanceFunction(TP, point)) < 0.0f)	return false;
	if ((DistanceFunction(BP, point)) < 0.0f)	return false;
	if ((DistanceFunction(NP, point)) < 0.0f)	return false;
	if ((DistanceFunction(FP, point)) < 0.0f)	return false;

	return true;
}
