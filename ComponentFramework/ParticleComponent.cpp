#include "ParticleComponent.h"
#include "ShaderComponent.h"
#include <random>

// Fluid Sim based on fluid simulation with geometry nodes by CGMatter
// https://www.youtube.com/watch?v=-RBbVCphQPw

ParticleComponent::ParticleComponent(
	Ref<Component> parent_,
	Ref<Renderer> renderer_,
	int particleCount_,
	Ref<MaterialComponent> material_,
	Ref<MeshComponent> instance_) :
	// Initialization
	// Base Parameters
	Component(parent_, renderer_),
	particleCount(particleCount_),
	material(material_),
	instance(instance_),


	// Sim parameters
	bBPosition(Vec3()),
	bBHalfExtents(Vec3(10.0f, 10.0f, 10.0f)),
	dampening(0.8f)
{
}

bool ParticleComponent::OnCreate()
{
	if (isCreated) return true;

	// Set up the particle instance mesh
	if (!instance)
	{
		instance = std::make_shared<MeshComponent>(nullptr, renderer, "meshes/Sphere.obj");		
		if (!instance->OnCreate()) return false;
	}

	std::default_random_engine generator;
	std::uniform_real_distribution<float> distribution(-4.0f, 4.0f);

	// Initialize the particles
	for (size_t i = 0; i < particleCount; i++)
	{
		float a = distribution(generator);
		float b = distribution(generator);
		float c = distribution(generator);
		Ref<Particle> p = std::make_shared<Particle>(Vec3(a, b, c), 0.1f);
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
	glUseProgram(material->GetShader()->GetProgram());
	for (Ref<Particle> p : particles)
	{
		glUniformMatrix4fv(
			material->GetShader()->GetUniformID("modelMatrix"), 
			1, GL_FALSE, 
			MMath::scale(Vec3(1.0f, 1.0f, 1.0f) * p->radius) * // Scale instance by radius of particle
			MMath::translate(p->position) * Matrix4()); // Set instance position to particle position

		material->Render();
		instance->Render();
		material->PostRender();
	}
}

void ParticleComponent::Simulate(const Vec3& force, const float deltaTime)
{
	for (Ref<Particle>& p : particles)
	{
		// Apply force to particle
		UpdateParticle(p, force, deltaTime);
	}
}

void ParticleComponent::UpdateParticle(Ref<Particle> p, const Vec3& force, const float deltaTime)
{
	// Apply forces on the particle
	p->acceleration = force / p->mass;
	p->velocity += p->acceleration * deltaTime;
	p->position += p->velocity * deltaTime;

	// Clamp position to be inside bounding box

	// Clamp Y Position
	p->position.y = std::max(p->position.y, bBPosition.y - bBHalfExtents.y);

	if (p->position.y == bBPosition.y - bBHalfExtents.y)
	{
		p->velocity.y *= -dampening;
	}

	// Clamp X & Z Position
	p->position.x = std::clamp(p->position.x, bBPosition.x - bBHalfExtents.x, bBPosition.x + bBHalfExtents.x);
	
	if(p->position.x == bBPosition.x + abs(bBHalfExtents.x))
	{
		p->velocity.x *= -dampening;
	}

	p->position.z = std::clamp(p->position.z, bBPosition.z - bBHalfExtents.z, bBPosition.z + bBHalfExtents.z);

	if (p->position.z == bBPosition.z + abs(bBHalfExtents.z))
	{
		p->velocity.z *= -dampening;
	}



	//p->position.x = std::clamp(p->position.x, boundingBoxPosition.x - boundingBoxHalfExtents.x, boundingBoxPosition.x + boundingBoxHalfExtents.x);
	//p->position.y = std::clamp(p->position.y, boundingBoxPosition.y - boundingBoxHalfExtents.y, boundingBoxPosition.y + boundingBoxHalfExtents.y);
	//p->position.z = std::clamp(p->position.z, boundingBoxPosition.z - boundingBoxHalfExtents.z, boundingBoxPosition.z + boundingBoxHalfExtents.z);


}

Particle::Particle(
	Vec3 position_, 
	float radius_, 
	Vec3 velocity_, 
	Vec3 acceleration_, 
	float mass_) :
	// Initialization
	position(position_), 
	radius(radius_), 
	velocity(velocity_), 
	acceleration(acceleration_), 
	mass(mass_)
{

}

