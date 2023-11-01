#include "Box.h"
#include <MMath.h>
#include <QMath.h>

using namespace MATH;

void GEOMETRY::Box::generateVerticesAndNormals()
{
	// Create the box corners
	Vec3 corners[8];

	// Front bottom left corner
	corners[0] = Vec3(x, y, z) + Vec3(-halfExtents.x, -halfExtents.y, -halfExtents.z);
	// Front top left corner
	corners[1] = Vec3(x, y, z) + Vec3(-halfExtents.x, halfExtents.y, -halfExtents.z);
	// Front bottom right corner
	corners[2] = Vec3(x, y, z) + Vec3(halfExtents.x, -halfExtents.y, -halfExtents.z);
	// Front top right corner
	corners[3] = Vec3(x, y, z) + Vec3(halfExtents.x, halfExtents.y, -halfExtents.z);
	// Back bottom left corner
	corners[4] = Vec3(x, y, z) + Vec3(-halfExtents.x, -halfExtents.y, halfExtents.z);
	// Back top left corner
	corners[5] = Vec3(x, y, z) + Vec3(-halfExtents.x, halfExtents.y, halfExtents.z);
	// Back bottom right corner
	corners[6] = Vec3(x, y, z) + Vec3(halfExtents.x, -halfExtents.y, halfExtents.z);
	// Back top right corner
	corners[7] = Vec3(x, y, z) + Vec3(halfExtents.x, halfExtents.y, halfExtents.z);

	// follow box orientation
	for (int i = 0; i < 8; i++)
	{
		corners[i] = QMath::rotate(corners[i], orientation);
	}

	// Create the box faces

	// -Z
	// Face 1
	vertices.push_back(corners[0]); vertices.push_back(corners[3]); vertices.push_back(corners[1]);
	// Face 2
	vertices.push_back(corners[0]); vertices.push_back(corners[2]); vertices.push_back(corners[3]);

	for (int i = 0; i < 6; i++)
	{
		normals.push_back(QMath::rotate(Vec3(0.0f, 0.0f, -1.0f), orientation));
	}

	// +Y
	// Face 3
	vertices.push_back(corners[1]); vertices.push_back(corners[7]); vertices.push_back(corners[5]);
	// Face 4
	vertices.push_back(corners[1]); vertices.push_back(corners[3]); vertices.push_back(corners[7]);

	for (int i = 0; i < 6; i++)
	{
		normals.push_back(QMath::rotate(Vec3(0.0f, 1.0f, 0.0f), orientation));
	}

	// +X
	// Face 5
	vertices.push_back(corners[2]); vertices.push_back(corners[7]); vertices.push_back(corners[3]);
	// Face 6
	vertices.push_back(corners[2]); vertices.push_back(corners[6]); vertices.push_back(corners[7]);

	for (int i = 0; i < 6; i++)
	{
		normals.push_back(QMath::rotate(Vec3(1.0f, 0.0f, 0.0f), orientation));
	}

	// +Z
	// Face 7
	vertices.push_back(corners[6]); vertices.push_back(corners[5]); vertices.push_back(corners[7]);
	// Face 8
	vertices.push_back(corners[6]); vertices.push_back(corners[4]); vertices.push_back(corners[5]);

	for (int i = 0; i < 6; i++)
	{
		normals.push_back(QMath::rotate(Vec3(0.0f, 0.0f, 1.0f), orientation));
	}

	//-X
	// Face 9
	vertices.push_back(corners[4]); vertices.push_back(corners[1]); vertices.push_back(corners[5]);
	// Face 10
	vertices.push_back(corners[4]); vertices.push_back(corners[0]); vertices.push_back(corners[1]);

	for (int i = 0; i < 6; i++)
	{
		normals.push_back(QMath::rotate(Vec3(-1.0f, 0.0f, 0.0f), orientation));
	}

	// -Y
	// Face 11
	vertices.push_back(corners[2]); vertices.push_back(corners[4]); vertices.push_back(corners[6]);
	// Face 12
	vertices.push_back(corners[2]); vertices.push_back(corners[0]); vertices.push_back(corners[4]);

	for (int i = 0; i < 6; i++)
	{
		normals.push_back(QMath::rotate(Vec3(0.0f, -1.0f, 0.0f), orientation));
	}


	StoreMeshData(GL_TRIANGLES);
}

RayIntersectionInfo GEOMETRY::Box::rayIntersectionInfo(const Ray& ray) const
{
	RayIntersectionInfo intersection;

	intersection = checkFaceIntersection(ray, FaceDirection::PosX);
	if (intersection.isIntersected) { return intersection; }

	intersection = checkFaceIntersection(ray, FaceDirection::NegX);
	if (intersection.isIntersected) { return intersection; }

	intersection = checkFaceIntersection(ray, FaceDirection::PosY);
	if (intersection.isIntersected) { return intersection; }

	intersection = checkFaceIntersection(ray, FaceDirection::NegY);
	if (intersection.isIntersected) { return intersection; }

	intersection = checkFaceIntersection(ray, FaceDirection::PosZ);
	if (intersection.isIntersected) { return intersection; }

	intersection = checkFaceIntersection(ray, FaceDirection::NegZ);
	return intersection;

}

RayIntersectionInfo GEOMETRY::Box::checkFaceIntersection(const Ray& ray, const FaceDirection dir) const
{
	RayIntersectionInfo intersection;
	Plane face;

	// Set face based on the direction given 
	switch (dir)
	{
	case FaceDirection::PosX:
		face = Plane(Vec3::right(), VMath::dot(Vec3::right(), Vec3(x + halfExtents.x, y, z)));

		break;
	case FaceDirection::NegX:
		face = Plane(-Vec3::right(), VMath::dot(-Vec3::right(), Vec3(x - halfExtents.x, y, z)));

		break;
	case FaceDirection::PosY:
		face = Plane(Vec3::up(), VMath::dot(Vec3::up(), Vec3(x, y + halfExtents.y, z)));
		break;
	case FaceDirection::NegY:
		face = Plane(-Vec3::up(), VMath::dot(-Vec3::up(), Vec3(x, y - halfExtents.y, z)));
		break;
	case FaceDirection::PosZ:
		face = Plane(Vec3::forward(), VMath::dot(Vec3::forward(), Vec3(x, y, z + halfExtents.z)));
		break;
	case FaceDirection::NegZ:
		face = Plane(-Vec3::forward(), VMath::dot(-Vec3::forward(), Vec3(x, y, z - halfExtents.z)));
		break;
	default:
		break;
	}

	Vec3 faceNormal = Vec3(face.x, face.y, face.z);

	// Check if the ray is pointed away from the face
	if (VMath::dot(faceNormal, ray.GetDir()) < 0) {
		intersection.Reset();
		return intersection;
	}

	float t = -(VMath::dot(ray.GetStart(), faceNormal) + face.d) / (VMath::dot(faceNormal, ray.GetDir()));

	// Check if the intersection is behind the ray start
	if (t < 0) {
		intersection.Reset();
		return intersection;
	}

	Vec3 planeIntersection = ray.currentPos(t);

	intersection.t = t;
	intersection.intersectionPoint = planeIntersection;


	switch (dir)
	{
	case FaceDirection::PosX:
	case FaceDirection::NegX:
	{
		float zMin = z - halfExtents.z;
		float zMax = z + halfExtents.z;
		float yMin = y - halfExtents.y;
		float yMax = y + halfExtents.y;
		if (planeIntersection.z > zMin && planeIntersection.z < zMax &&
			planeIntersection.y > yMin && planeIntersection.y < yMax) {
			intersection.isIntersected = true;
			return intersection;
		}
		break;
	}

	case FaceDirection::PosY:
	case FaceDirection::NegY:
	{
		float xMin = x - halfExtents.x;
		float xMax = x + halfExtents.x;
		float zMin = x - halfExtents.x;
		float zMax = x + halfExtents.x;
		if (planeIntersection.x > xMin && planeIntersection.x < xMax &&
			planeIntersection.z > zMin && planeIntersection.z < zMax) {
			intersection.isIntersected = true;
			return intersection;
		}
		break;
	}


	case FaceDirection::PosZ:
	case FaceDirection::NegZ:
	{
		float xMin = x - halfExtents.x;
		float xMax = x + halfExtents.x;
		float yMin = y - halfExtents.y;
		float yMax = y + halfExtents.y;
		if (planeIntersection.x > xMin && planeIntersection.x < xMax &&
			planeIntersection.y > yMin && planeIntersection.y < yMax) {
			intersection.isIntersected = true;
			return intersection;
		}
		break;
	}

	default:
		break;
	}

	intersection.Reset();
	return intersection;
}
