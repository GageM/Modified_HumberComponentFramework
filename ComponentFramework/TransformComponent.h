// Umer Noor 2022
// Original code from/taught by Dr. Scott Fielder. Lucky for me I was in that class (Game Engine 4)

#pragma once
#include "Component.h"
#include <Matrix.h>
#include <Quaternion.h>
#include <Euler.h>
#include "Actor.h"
#include <VMath.h>
#include <QMath.h>
#include <EMath.h>

using namespace MATH;
struct TransformComponent : public Component {
	Vec3 pos;
	Quaternion orientation;
	Euler rotation;
	Vec3 scale;
	TransformComponent(Ref<Component> parent_);
	TransformComponent(Ref<Component> parent_, Vec3 pos_, Quaternion orientation_, Vec3 scale_ = Vec3(1.0f, 1.0f, 1.0f)); // that last argument (has to be the last set) allows to initialize without a 4th argument
	~TransformComponent();
	bool OnCreate();
	void OnDestroy();
	void Update(const float deltaTime_);
	void Render() const;
	Matrix4 GetTransformMatrix() const;
	Quaternion GetOrientation() { return orientation; }

	inline void SetTransform(Vec3 pos_, Quaternion orientation_, Vec3 scale_ = Vec3(1.0f, 1.0f, 1.0f)) {
		SetPosition(pos_);
		SetOrientation(orientation_);
		SetScale(scale_);
	}

	inline void SetTransform(Vec3 pos_, Euler rotation_, Vec3 scale_ = Vec3(1.0f, 1.0f, 1.0f)) {
		SetPosition(pos_);
		SetRotation(rotation_);
		SetScale(scale_);
	}

	inline void SetPosition(Vec3 pos_) {
		pos = pos_;
	}

	inline void SetOrientation(Quaternion orientation_) {
		orientation = orientation_;
	}

	// Sets the orientation from the current rotation
	inline void UpdateOrientation()	{
		orientation = QMath::toQuaternion(rotation);
	}

	inline void SetRotation(Euler rotation_) {
		rotation = rotation_;
	}

	// Sets the rotation from the current orientation
	inline void UpdateRotation() {
		rotation = EMath::toEuler(orientation);
	}

	inline void SetScale(Vec3 scale_) {
		scale = scale_;
	}

	// Getters for forward, right and up vectors from current orientation
	Vec3 forward() { return VMath::normalize(orientation * Vec3(0.0f, 0.0f, 1.0f)); }
	Vec3 right() { return VMath::normalize(orientation * Vec3(1.0f, 0.0f, 0.0f)); }
	Vec3 up() { return VMath::normalize(orientation * Vec3(0.0f, 1.0f, 0.0f)); }
};