#include "Frustrum.h"
#include <Vector.h>

Frustrum::Frustrum(Ref<TransformComponent> transform, const float fovy, const float aspectRatio, const float near, const float far)
{
	Set(transform, fovy, aspectRatio, near, far);
}

void Frustrum::Set(Ref<TransformComponent> transform, const float fovy, const float aspectRatio, const float near, const float far)
{
	const float halfVSide = far * tanf(fovy * 0.5f);
	const float halfHSide = halfVSide * aspectRatio;
	const Vec3 frontMultFar = far * transform->forward();
	
	nearFace = Plane(transform->forward(), VMath::dot(transform->forward(), transform->pos + transform->forward() * near));
	farFace = Plane(-transform->forward(), VMath::dot(-transform->forward(), transform->pos + transform->forward() * far));

	// Get Center Point of the Far Face
	Vec3 fc = transform->pos + transform->forward() * far;

	/// Get The Right & Top Frustrum Faces

	// Top Right Point on the Far Face
	Vec3 ftr = fc + (transform->up() * halfVSide) + (transform->right() * halfHSide);

	// Right Face
	Vec3 faceNormal = VMath::normalize(VMath::cross(frontMultFar - transform->right() * halfHSide, transform->up()));
	rightFace = Plane(faceNormal, VMath::dot(faceNormal, ftr));

	// Top Face
	faceNormal = VMath::normalize(VMath::cross(transform->right(), frontMultFar - transform->up() * halfVSide));
	topFace = Plane(faceNormal, VMath::dot(faceNormal, ftr));

	/// Get The Left & Bottom Frustrum Faces

	// Bottom Left Point on the Far Face
	Vec3 fbl = fc - (transform->up() * halfVSide) - (transform->right() * halfHSide);

	// Left Face
	faceNormal = VMath::normalize(VMath::cross(transform->up(), frontMultFar + transform->right() * halfHSide));
	leftFace = Plane(faceNormal, VMath::dot(faceNormal, fbl));

	// Bottom Face
	faceNormal = VMath::normalize(VMath::cross(frontMultFar + transform->up() * halfVSide, transform->right()));
	bottomFace = Plane(faceNormal, VMath::dot(faceNormal, fbl));

}
