#pragma once
#include "Component.h"
#include <Vector.h>
#include <vector>

#include "MeshComponent.h"
#include "ShaderComponent.h"

using namespace MATH;

// Particle class is separate from sphere so it can contain less information
class Particle
{
public:
	Particle(
		Vec3 position_ = Vec3(0.0f, 0.0f, 0.0f), 
		float radius_ = 1.0f, 
		Vec3 velocity_ = Vec3(0.0f, 0.0f, 0.0f), 
		Vec3 acceleration_ = Vec3(0.0f, 0.0f, 0.0f), 
		float mass_ = 1.0f
	);

	Vec3 position;
	float radius;

	Vec3 velocity;
	Vec3 acceleration;

	float mass;
};

class ParticleComponent : Component
{
public:
	ParticleComponent(Ref<Component> parent_, Ref<Renderer> renderer_, Ref<MeshComponent> instance_, int particleCount_ = 1);
	// using = default; is the same as {};
	~ParticleComponent() = default;
	bool OnCreate();
	void OnDestroy();
	void Update(const float deltaTime_);
	void Render()const;

	// Simulation bounds
	Vec3 boundingBoxPosition;
	Vec3 boundingBoxHalfExtents;

	// Particle info
	int particleCount;
	std::vector<Ref<Particle>> particles;

	void Simulate(const Vec3& force, const float deltaTime);

	void UpdateParticle(Ref<Particle> point, const Vec3& force, const float deltaTime);

private:
	Ref<MeshComponent> instance;
	Ref<ShaderComponent> shader;
};


