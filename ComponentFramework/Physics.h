#pragma once
#include <Vector.h>
#include <Matrix.h>
#include <Quaternion.h>
#include "PhysicsComponent.h"
#include "Actor.h"


using namespace MATH;

namespace PHYSICS
{
	// Base physics equations
	void ApplyForce(Ref<PhysicsComponent> body, const Vec3& force);

	void UpdatePos(Ref<PhysicsComponent> body, const float deltaTime);

	void UpdateVel(Ref<PhysicsComponent> body, const float deltaTime);

	void UpdateOrientation(Ref<PhysicsComponent> body, const float deltaTime);

	void UpdateTransform(Ref<Actor> actor);

	// Physics constraints
	void XAxisConstraint();

	void YAxisConstraint();

	void ZAxisConstraint();

	void XYPlaneConstraint();

	void XZPlaneConstraint();

	void YZPlaneConstraint();

	void MouseConstraint(Ref<PhysicsComponent> body, const float deltaTime, const Vec3& mousePos);
}

