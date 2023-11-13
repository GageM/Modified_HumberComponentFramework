#include "PhysicsComponent.h"

PhysicsComponent::PhysicsComponent(Ref<Component> parent_, Ref<Renderer> renderer_) : Component(parent_, renderer_),
mass(0.0f), rotationalinertia(Matrix4()), pos(Vec3()), vel(Vec3()), orientation(Quaternion()), angularVel(Vec3()), angularAccel(Vec3())
{
}

PhysicsComponent::PhysicsComponent(Ref<Component> parent_, Ref<Renderer> renderer_, Ref<TransformComponent> transform, float mass_) : Component(parent_, renderer_),
mass(mass_), rotationalinertia(Matrix4()), pos(transform->pos), vel(Vec3()), orientation(transform->orientation), angularVel(Vec3()), angularAccel(Vec3())
{
}

PhysicsComponent::~PhysicsComponent()
{
}

bool PhysicsComponent::OnCreate()
{
	if (isCreated) return true;

	isCreated = true;
	return isCreated;
}

void PhysicsComponent::OnDestroy()
{
}

void PhysicsComponent::Update(const float deltaTime)
{
}

void PhysicsComponent::Render() const
{
}
