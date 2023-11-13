#include <iostream>
#include "TransformComponent.h"
#include "MeshComponent.h"
#include <MMath.h>

//dont have to say it again, but why not
using namespace MATH;

TransformComponent::TransformComponent(Ref<Component> parent_, Ref<Renderer> renderer_):Component(parent_, renderer_)
{
}
TransformComponent::TransformComponent(Ref<Component> parent_, Ref<Renderer> renderer_, Vec3 pos_, Quaternion orientation_, Vec3 scale_)
	: Component(parent_, renderer_), pos(pos_), orientation(orientation_), scale(scale_)
{
	UpdateRotation();
}
TransformComponent::~TransformComponent() {
// let's set the scale to 1 so we don't foul ourselves up
	scale = Vec3(1.0f, 1.0f, 1.0f);
	pos = Vec3(0.0f, 0.0f, 0.0f);
	orientation = Quaternion(1.0, Vec3(0.0f, 0.0f, 0.0f));
	rotation = Euler(0.0f, 0.0f, 0.0f);
}
// TO DO: Do we need to check for isCreated here? Not much going on in OnCreate or OnDestroy
bool TransformComponent::OnCreate() {
	return true;
}
void TransformComponent::OnDestroy() {}

void TransformComponent::Update(const float deltaTime) {
}
void TransformComponent::Render()const {}

Matrix4 TransformComponent::GetTransformMatrix() const
{
	// we're gonna get translation, rotation, and scale
	// scale first so that scale is applied on local axes instead of global axes
	// so let's do rotation second so that origin is at the centre before we rotate
	Matrix4 m;
	m = MMath::translate(pos) * MMath::toMatrix4(orientation) * MMath::scale(scale);
	return m;
}
