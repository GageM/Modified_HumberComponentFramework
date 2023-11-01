#include "Sphere.h"
#include <MMath.h>
#include "QuadraticSolution.h"

using namespace MATH;
using namespace GEOMETRY;


RayIntersectionInfo Sphere::rayIntersectionInfo(const Ray& ray) const
{
	QuadraticSolution qs;
	RayIntersectionInfo intersection;

	float a = MATH::VMath::dot(ray.GetDir(), ray.GetDir());
	float b = 2 * VMath::dot(ray.GetDir(), (ray.GetStart() - Vec3(x, y, z)));
	float c = VMath::dot((ray.GetStart() - Vec3(x, y, z)), ray.GetStart() - Vec3(x, y, z)) - pow(r, 2.0f);

	qs.SolveQuadratic(a, b, c);

	switch (qs.solutions)
	{
	case NumSolutions::zero:
		intersection.isIntersected = false;
		break;
	case NumSolutions::one:
		intersection.isIntersected = true;
		intersection.t = qs.firstSolution;
		intersection.intersectionPoint = ray.currentPos(intersection.t);
		break;
	case NumSolutions::two:
		intersection.isIntersected = true;
		intersection.t = qs.firstSolution;
		intersection.intersectionPoint = ray.currentPos(intersection.t);
		break;
	default:
		break;
	}

	return intersection;
}


void Sphere::generateVerticesAndNormals()
{
	// We need to fill the vertices and normals arrays with the correct data for a sphere
	// deltaTheta governs how close each ring will be around our sphere. Try messing with it
	const float deltaTheta = 2.0f;
	// deltaPhi governs how close each point will be per ring. Try messing with it
	const float deltaPhi = 2.0f;
	for (float thetaDeg = 0.0f; thetaDeg <= 360.0f; thetaDeg += deltaTheta)
	{
		// Build a ring
		Vec3 circle(r * sin(thetaDeg * DEGREES_TO_RADIANS), r * cos(thetaDeg * DEGREES_TO_RADIANS), 0.0f);
		for (float phiDeg = 0.0f; phiDeg <= 360.0f; phiDeg += deltaPhi) {
			// Rotate a point in the ring around the y-axis to build a sphere!
			Matrix3 rotationMatrix = MMath::rotate(deltaPhi, Vec3(0.0f, 1.0f, 0.0f));
			circle = rotationMatrix * circle;
			// Push the circle point to our vertices array
			vertices.push_back(circle);
			// The normal of a sphere points outwards from the center position Vec3(x, y, z)
			normals.push_back(MATH::Vec3(0.0f, 0.0f, 0.0f));// (circle - Vec3(x, y, z));
		}
	}
	// Once we are all done filling the vertices and normals, use the base class method to store the data in the GPU
	StoreMeshData(GL_POINTS);
}