#pragma once

#include "Component.h"
#include <Vector.h>
#include <Quaternion.h>
#include <Matrix.h>
#include "TransformComponent.h"

using namespace MATH;
class PhysicsComponent : public Component
{
public:
	float mass = 1.0f;
	Matrix4 rotationalinertia;
	Vec3 pos;
	Vec3 vel;
	Vec3 accel;
	Quaternion orientation;
	Vec3 angularVel;
	Vec3 angularAccel;

	PhysicsComponent(Ref<Component> parent_, RendererType renderer_);
	PhysicsComponent(Ref<Component> parent_, RendererType renderer_, Ref<TransformComponent> transform, float mass_ = 1.0f);
	~PhysicsComponent();


	bool OnCreate();
	void OnDestroy();
	void Update(const float deltaTime);
	void Render() const;


private:

};

