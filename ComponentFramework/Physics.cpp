#include "Physics.h"
#include "TransformComponent.h"

// Physics Equations
void Physics::ApplyForce(Ref<PhysicsComponent> body, const Vec3& force)
{
	body->accel = force / body->mass;
}

void Physics::UpdatePos(Ref<PhysicsComponent> body, float deltaTime)
{
	body->pos += body->vel * deltaTime;
}

void Physics::UpdateVel(Ref<PhysicsComponent> body, float deltaTime)
{
	body->vel += body->accel * deltaTime;
}

void Physics::UpdateOrientation(Ref<PhysicsComponent> body, float deltaTime)
{
	Quaternion rotation;

	// Can you solve all angles at once?
	//rotation = QMath::angleAxisRotation(VMath::mag(body->angularVel), VMath::normalize(body->angularVel));
	//body->orientation = rotation * body->orientation;

	// Solve X rotation and add to orientation
	rotation = QMath::angleAxisRotation(body->angularVel.x, Vec3::right());
	body->orientation = rotation * body->orientation;

	// Solve Y rotation and add to orientation
	rotation = QMath::angleAxisRotation(body->angularVel.y, Vec3::up());
	body->orientation = rotation * body->orientation;

	// Solve Z rotation and add to orientation
	rotation = QMath::angleAxisRotation(body->angularVel.z, Vec3::forward());
	body->orientation = rotation * body->orientation;
}

void Physics::UpdateTransform(Ref<Actor> actor)
{
	Ref<TransformComponent> transform = actor->GetComponent<TransformComponent>();
	Ref<PhysicsComponent> physics = actor->GetComponent<PhysicsComponent>();

	// Do nothing if the actor does not have a transform component
	if (!transform) return;

	// Do nothing if the actor has no physics component
	if (!physics) return;

	transform->pos = physics->pos;
	transform->orientation = physics->orientation;
}

// Physics Constraints
void Physics::XAxisConstraint()
{
}

void Physics::YAxisConstraint()
{
}

void Physics::ZAxisConstraint()
{
}

void Physics::XYPlaneConstraint()
{
}

void Physics::XZPlaneConstraint()
{
}

void Physics::YZPlaneConstraint()
{
}

void Physics::CameraPlaneConstraint()
{
}
