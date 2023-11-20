#include "ParticleComponent.h"


ParticleComponent::ParticleComponent(Ref<Component> parent_, Ref<Renderer> renderer_, Ref<MeshComponent> instance_, int particleCount_) : Component(parent_, renderer_),
instance(instance_), particleCount(particleCount_)
{
}

bool ParticleComponent::OnCreate()
{
	if (isCreated) return true;

	// Set up the particle instance
	if (!instance)
	{
		instance = std::make_shared<MeshComponent>(nullptr, renderer, "meshes/Sphere.obj");
		
		if (!instance->OnCreate()) return false;
	}

	// Set up the shader
	// TODO::Link this to assets
	shader = std::make_shared<ShaderComponent>(nullptr, renderer, "shaders/debugVert.glsl", "shaders/debugFrag.glsl");
	if (!shader->OnCreate()) return false;

	// Initialize the particles
	for (size_t i = 0; i < particleCount; i++)
	{
		Ref<Particle> p = std::make_shared<Particle>();
		particles.push_back(p);
	}

	isCreated = true;
	return isCreated;
}

void ParticleComponent::OnDestroy()
{
	instance = nullptr;
}

void ParticleComponent::Update(const float deltaTime_)
{
}

void ParticleComponent::Render() const
{
	instance->Render();
}

void ParticleComponent::Simulate(const Vec3& force, const float deltaTime)
{
	for (Ref<Particle>& p : particles)
	{
		// Calculate the force on each particle



		UpdateParticle(p, force, deltaTime);
	}
}

void ParticleComponent::UpdateParticle(Ref<Particle> point, const Vec3& force, const float deltaTime)
{
	// Apply forces on the particle
	point->acceleration = force / point->mass;
	point->velocity += point->acceleration * deltaTime;
	point->position += point->velocity * deltaTime;

	// Clamp position to be inside bounding box
	point->position.x = std::clamp(point->position.x, boundingBoxPosition.x - boundingBoxHalfExtents.x, boundingBoxPosition.x + boundingBoxHalfExtents.x);
	point->position.y = std::clamp(point->position.y, boundingBoxPosition.y - boundingBoxHalfExtents.y, boundingBoxPosition.y + boundingBoxHalfExtents.y);
	point->position.z = std::clamp(point->position.z, boundingBoxPosition.z - boundingBoxHalfExtents.z, boundingBoxPosition.z + boundingBoxHalfExtents.z);


}

Particle::Particle(Vec3 position_, float radius_, Vec3 velocity_, Vec3 acceleration_, float mass_) :
	position(position_), radius(radius_), velocity(velocity_), acceleration(acceleration_), mass(mass_)
{

}