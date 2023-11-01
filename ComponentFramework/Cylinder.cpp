#include "Cylinder.h"
#include <MMath.h>
#include "QuadraticSolution.h"

using namespace MATH;
using namespace GEOMETRY;

void GEOMETRY::Cylinder::generateVerticesAndNormals()
{
	// We need to fill the vertices and normals arrays with the correct data for a cylinder
// deltaTheta governs how close each ring will be around our cylinder. Try messing with it
	const float deltaTheta = 2.0f;
	// deltaPhi governs how close each point will be per ring. Try messing with it
	const float deltaPhi = 2.0f;

	// The distance between rings when rendering the cylinder
	const float ringDistance = 0.1f;

	// Create rings around the cylinder for display
	for (float thetaDeg = 0.0f; thetaDeg <= 360.00; thetaDeg += deltaTheta)
	{
		for (MATH::Vec3 ringPos = capPosA; ringPos.z <= capPosB.z; ringPos.z += ringDistance)
		{		
			Vec3 vertex(r * sin(thetaDeg * DEGREES_TO_RADIANS), r * cos(thetaDeg * DEGREES_TO_RADIANS), 0.0f);
			vertex = MMath::translate(ringPos) * vertex;
			// Push the circle point to our vertices array
			vertices.push_back(vertex);
			// No normals on cylinder
			normals.push_back(MATH::Vec3(0.0f, 0.0f, 0.0f));// (circle - Vec3(x, y, z));
		}

	}

	
	// Once we are all done filling the vertices and normals, use the base class method to store the data in the GPU
	
	StoreMeshData(GL_POINTS);
}

RayIntersectionInfo GEOMETRY::Cylinder::rayIntersectionInfo(const Ray& ray) const
{
	RayIntersectionInfo intersection = checkInfiniteCylinder(ray);	
	if (intersection.isIntersected) {
		return intersection;
	}

	intersection = checkEndCaps(ray);
	return intersection;
}

RayIntersectionInfo GEOMETRY::Cylinder::checkInfiniteCylinder(const Ray& ray) const
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

RayIntersectionInfo GEOMETRY::Cylinder::checkEndCaps(const Ray& ray) const
{
	RayIntersectionInfo intersection;
	Vec3 AB = capPosB - capPosA;

	// Check Cap A
	if (VMath::dot(-AB, ray.GetDir()) > 0)
	{
		// Pointed to endcap A
		Vec3 capANormal = VMath::normalize(AB);
		Plane capAPlane(capANormal, VMath::dot(capANormal, capPosA));

		// Check if the ray intersects with a plane on cap A
		float t = -(VMath::dot(capANormal, ray.GetStart()) + capAPlane.d) / VMath::dot(capANormal, ray.GetDir());

		Vec3 Q = ray.currentPos(t);

		// x^2 + y^2 + z^2 = r^2

		Vec3 radiusCheck = Q - capPosA;

		if (VMath::mag(radiusCheck) > r)
		{
			// Intersection is not within end cap
			intersection.Reset();
			return intersection;
		}
		intersection.isIntersected = true;
		intersection.intersectionPoint = Q;
		intersection.t = t;
		return intersection;
	}
	// Check Cap B
	else
	{
		
		Vec3 capBNormal = VMath::normalize(-AB);
		Plane capBPlane(capBNormal, VMath::dot(capBNormal, capPosB));

		// Check if the ray intersects with a plane on cap A
		float t = -(VMath::dot(capBNormal, ray.GetStart()) + capBPlane.d)/ VMath::dot(capBNormal, ray.GetDir());

		Vec3 Q = ray.currentPos(t);

		// x^2 + y^2 + z^2 = r^2

		Vec3 radiusCheck = Q - capPosA;

		if (VMath::mag(radiusCheck) > r)
		{
			// Intersection is not within end cap
			intersection.Reset();
			return intersection;
		}
		intersection.isIntersected = true;
		intersection.intersectionPoint = Q;
		intersection.t = t;
		return intersection;
	}

}
