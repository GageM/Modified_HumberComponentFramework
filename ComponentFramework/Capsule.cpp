#include "Capsule.h"
#include <MMath.h>
#include "QuadraticSolution.h"

using namespace MATH;

void GEOMETRY::Capsule::generateVerticesAndNormals()
{
	// We need to fill the vertices and normals arrays with the correct data for a capsule
// deltaTheta governs how close each ring will be around our capsule. Try messing with it
	const float deltaTheta = 8.0f;
	// deltaPhi governs how close each point will be per ring. Try messing with it
	const float deltaPhi = 4.0f;

	// The distance between rings when rendering the capsule
	const float ringDistance = 0.1f;

	// Create rings around the capsule for display
	for (float thetaDeg = 0.0f; thetaDeg <= 360.00; thetaDeg += deltaTheta)
	{
		for (Vec3 ringPos = capPosA; ringPos.z <= capPosB.z; ringPos.z += ringDistance)
		{
			Vec3 vertex(r * sin(thetaDeg * DEGREES_TO_RADIANS), r * cos(thetaDeg * DEGREES_TO_RADIANS), 0.0f);
			vertex = MMath::translate(ringPos) * vertex;
			vertex = MMath::rotate(90.0f, Vec3(1.0f, 0.0f, 0.0f)) * vertex;
			// Push the circle point to our vertices array
			vertices.push_back(vertex);
			// No normals on capsule
			normals.push_back(MATH::Vec3(0.0f, 0.0f, 0.0f));// (circle - Vec3(x, y, z));
		}

	}

	
	// Create The Capsule End Caps
	for (float thetaDeg = 0.0f; thetaDeg <= 90.0f; thetaDeg += deltaTheta)
	{
		// Build the top cap
		MATH::Vec3 circle(r * sin(thetaDeg * DEGREES_TO_RADIANS), r * cos(thetaDeg * DEGREES_TO_RADIANS), 0.0f);
		circle = MMath::translate(Vec3(capPosB.x, capPosB.z, capPosB.y)) * circle;
		for (float phiDeg = 0.0f; phiDeg <= 360.0; phiDeg += deltaPhi) {
			// Rotate a point in the ring around the y-axis to build a sphere!
			Matrix3 rotationMatrix = MMath::rotate(deltaPhi, Vec3(0.0f, 1.0f, 0.0f));
			circle = rotationMatrix * circle;

			// Push the circle point to our vertices array
			vertices.push_back(circle);
			// The normal of a sphere points outwards from the center position Vec3(x, y, z)
			normals.push_back(MATH::Vec3(0.0f, 0.0f, 0.0f));// (circle - Vec3(x, y, z));
		}
	}

	for (float thetaDeg = 90.0f; thetaDeg <= 180.0f; thetaDeg += deltaTheta)
	{
		// Build the top cap
		MATH::Vec3 circle(r * sin(thetaDeg * DEGREES_TO_RADIANS), r * cos(thetaDeg * DEGREES_TO_RADIANS), 0.0f);
		circle = MMath::translate(Vec3(capPosA.x, capPosA.z, capPosA.y)) * circle;
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

RayIntersectionInfo GEOMETRY::Capsule::rayIntersectionInfo(const Ray& ray) const
{
	RayIntersectionInfo intersection = checkInfiniteCylinder(ray);
	if (intersection.isIntersected) {
		return intersection;
	}

	intersection = checkEndSpheres(ray);
	return intersection;
}

RayIntersectionInfo GEOMETRY::Capsule::checkInfiniteCylinder(const Ray& ray) const
{
	Vec3 AS = ray.GetStart() - capPosA;
	Vec3 AB = capPosB - capPosA;

	float a = VMath::dot(ray.GetDir(), ray.GetDir()) - pow(VMath::dot(ray.GetDir(), VMath::normalize(AB)), 2);
	float b = 2 * (VMath::dot(ray.GetStart() - capPosA, ray.GetDir()) - VMath::dot(ray.GetDir(), VMath::normalize(AB)) * VMath::dot(AS, VMath::normalize(AB)));
	float c = VMath::dot(AS, AS) - pow(VMath::dot(AS, VMath::normalize(AB)), 2.0f) - pow(r, 2.0f);

	QuadraticSolution qs;
	RayIntersectionInfo intersection;

	qs.SolveQuadratic(a, b, c);

	switch (qs.solutions)
	{
	case NumSolutions::zero:
		intersection.Reset();
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

	Vec3 P = intersection.intersectionPoint;

	// The intersection point is between CapPosA and CapPosB
	// The intersection point is on the cylinder
	if (VMath::dot(capPosB - capPosA, P - capPosA) > 0.0f && VMath::dot(capPosA - capPosB, P - capPosB) > 0.0f)
	{
		return intersection;
	}
	else
	{
		intersection.Reset();
	}
	return intersection;
}

RayIntersectionInfo GEOMETRY::Capsule::checkEndSpheres(const Ray& ray) const
{
	// Check End Sphere A
	Sphere CapSphere = Sphere(capPosA, r);
	RayIntersectionInfo intersection = CapSphere.rayIntersectionInfo(ray);
	if (intersection.isIntersected) {
		return intersection;
	}

	// Check End Sphere B
	CapSphere = Sphere(capPosB, r);
	intersection = CapSphere.rayIntersectionInfo(ray);
	return intersection;
}
