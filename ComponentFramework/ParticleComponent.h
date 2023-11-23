#pragma once
#include "Component.h"
#include <Vector.h>
#include <vector>

#include "MeshComponent.h"
#include "MaterialComponent.h"
#include "KDTree.h"

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
	ParticleComponent( Ref<Component> parent_, Ref<Renderer> renderer_, const int& particleCount_ = 1, const float& particleRadius_ = 1.0f, 
		const Vec3& initialVelocity_ = Vec3::right(), Ref<MaterialComponent> material_ = nullptr, Ref<MeshComponent> instance_ = nullptr);

	~ParticleComponent() = default;
	bool OnCreate();
	void OnDestroy();
	void Update(const float deltaTime_);
	void Render()const;

	// Helper functions

	// Getters
	inline Ref<MaterialComponent> GetMaterial() { return material; }
	inline Ref<MeshComponent> GetInstance() { return instance; }

	// Setters
	inline void SetMaterial(const Ref<MaterialComponent> m) { material = m; }
	inline void SetInstance(const Ref<MeshComponent> i) { instance = i; }

	// Simulation bounds
	Vec3 bBPosition;
	Vec3 bBHalfExtents;

	// Simulation Parameters
	const float dampening;
	const int subFrameIterations;

	// Particle info
	int particleCount;
	std::vector<Ref<Particle>> particles;

	const float particleRadius;
	const Vec3 initialVelocity;

	void Simulate(const Vec3& force, const float deltaTime);

	void UpdateParticle(Ref<Particle> p, const Vec3& force, const float deltaTime);

	// Returns the index of the nearest particle to the one given
	int IndexOfNearest(Ref<Particle> p);


	inline void SetHalfExtents(const Vec3& v) { bBHalfExtents = v; }
private:
	Ref<MeshComponent> instance;
	Ref<MaterialComponent> material;

	// Needed for particle collisions
	Ref<KDTree::Node> particleTree;

	float timeSinceLastSpawn;
	float spawnDelay;
};




