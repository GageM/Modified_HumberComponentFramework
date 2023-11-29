#include "Frustrum.h"
#include <Vector.h>
#include <VMath.h>
#include <MMath.h>
#include <PMath.h>


Frustrum::Frustrum(Matrix4 projection, Matrix4 view)
{
	Set(projection, view);
}

void Frustrum::Set(Matrix4 projection, Matrix4 view)
{
	Matrix4 m = view * projection;

	// Left Plane
	LP = Plane(m[41] + m[11], m[42] + m[12], m[43] + m[13], m[44] + m[14]);
	// Right Plane
	RP = Plane(m[41] - m[11], m[42] - m[12], m[43] - m[13], m[44] - m[14]);
	// Top Plane
	TP = Plane(m[41] - m[21], m[42] - m[21], m[43] - m[23], m[44] - m[24]);
	// Bottom Plane
	BP = Plane(m[41] + m[21], m[42] + m[22], m[43] + m[23], m[44] + m[24]);
	// Near Plane
	NP = Plane(m[41] + m[31], m[42] + m[32], m[43] + m[33], m[44] + m[34]);
	// Far Plane
	FP = Plane(m[41] - m[31], m[42] - m[32], m[43] - m[33], m[44] - m[34]);

	Normalize();
}

void Frustrum::Normalize()
{
	PMath::normalize(LP);
	PMath::normalize(RP);
	PMath::normalize(TP);
	PMath::normalize(BP);
	PMath::normalize(NP);
	PMath::normalize(FP);
}

float Frustrum::DistanceFunction(const Plane& plane, const Vec3& point)
{
	return plane.x * point.x + plane.y * point.y + plane.z * point.z + plane.d;
}

bool Frustrum::InFrustrum(const Vec3& point)
{
	// Check if the point is in the positive normal direction of each frustrum plane
	if ((DistanceFunction(LP, point)) < 0.0f)	return false;
	if ((DistanceFunction(RP, point)) < 0.0f)	return false;
	if ((DistanceFunction(TP, point)) < 0.0f)	return false;
	if ((DistanceFunction(BP, point)) < 0.0f)	return false;
	//if ((DistanceFunction(NP, point)) < 0.0f)	return false;
	//if ((DistanceFunction(FP, point)) < 0.0f)	return false;

	return true;
}
