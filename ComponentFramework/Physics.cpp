#include "Physics.h"
#include "TransformComponent.h"
#include <MMath.h>

// Physics Equations
void PHYSICS::ApplyForce(Ref<PhysicsComponent> body, const Vec3& force)
{
	body->accel = force / body->mass;
}

void PHYSICS::UpdatePos(Ref<PhysicsComponent> body, const float deltaTime)
{
	body->pos += body->vel * deltaTime;
}

void PHYSICS::UpdateVel(Ref<PhysicsComponent> body, const float deltaTime)
{
	body->vel += body->accel * deltaTime;
}

void PHYSICS::UpdateOrientation(Ref<PhysicsComponent> body, const float deltaTime)
{
	Quaternion rotation;

	// Can you solve all angles at once?
	rotation = QMath::angleAxisRotation(VMath::mag(body->angularVel), VMath::normalize(body->angularVel));
	body->orientation = rotation * body->orientation;

	// Solve X rotation and add to orientation
	//rotation = QMath::angleAxisRotation(body->angularVel.x, Vec3::right());
	//body->orientation = rotation * body->orientation;

	// Solve Y rotation and add to orientation
	//rotation = QMath::angleAxisRotation(body->angularVel.y, Vec3::up());
	//body->orientation = rotation * body->orientation;

	// Solve Z rotation and add to orientation
	//rotation = QMath::angleAxisRotation(body->angularVel.z, Vec3::forward());
	//body->orientation = rotation * body->orientation;
}

void PHYSICS::UpdateTransform(Ref<Actor> actor)
{
	Ref<TransformComponent> transform = actor->GetComponent<TransformComponent>();

	// Do nothing if the actor does not have a transform component
	if (!transform) return;

	Ref<PhysicsComponent> physics = actor->GetComponent<PhysicsComponent>();

	// Do nothing if the actor has no physics component
	if (!physics) return;

	transform->pos = physics->pos;
	transform->orientation = physics->orientation;
}

// Physics Constraints
void PHYSICS::XAxisConstraint()
{
}

void PHYSICS::YAxisConstraint()
{
}

void PHYSICS::ZAxisConstraint()
{
}

void PHYSICS::XYPlaneConstraint()
{
}

void PHYSICS::XZPlaneConstraint()
{
}

void PHYSICS::YZPlaneConstraint()
{
}

void PHYSICS::MouseConstraint(Ref<PhysicsComponent> body, const float deltaTime, const Vec3& mousePos)
{
	// r is the direction vector from the body position to the mouse position
	Vec3 r = mousePos - body->pos;

	Matrix3 Meff = Matrix3(
		1 + (r.z * r.z) + r.y * r.y, -r.x * r.y, -r.x * r.z,
		-r.y * r.x, 1 + (r.x * r.x) + (r.z * r.z), -r.y * r.z,
		-r.x * r.z, -r.y * r.z, 1 + (r.x * r.x) + (r.y * r.y)
	);

	Vec3 JV = body->vel + VMath::cross(body->angularVel, r);

	// calculate deltaV as the lagrangian to save a variable
	Vec3 deltaV = MMath::inverse(Meff) * -JV;

	Vec3 deltaAV = VMath::cross(r, deltaV);

	body->vel += deltaV;

	body->angularVel += deltaAV;
}
