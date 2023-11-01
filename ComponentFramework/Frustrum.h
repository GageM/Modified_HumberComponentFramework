#pragma once
#include <Plane.h>
#include "TransformComponent.h"

using namespace MATH;

class Frustrum
{
public:
	Frustrum() {}

	Frustrum(Ref<TransformComponent> transform, const float fovy, const float aspectRatio, const float near, const float far);
	
	void Set(Ref<TransformComponent> transform, const float fovy, const float aspectRatio, const float near, const float far);

	Plane topFace;
	Plane bottomFace;

	Plane rightFace;
	Plane leftFace;

	Plane nearFace;
	Plane farFace;

private:
};

