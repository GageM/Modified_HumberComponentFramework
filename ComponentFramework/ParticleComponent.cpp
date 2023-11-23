#include "ParticleComponent.h"
#include "ShaderComponent.h"
#include <random>

// Fluid Sim based on 'fluid simulation with geometry nodes' by CGMatter
// https://www.youtube.com/watch?v=-RBbVCphQPw

ParticleComponent::ParticleComponent(Ref<Component> parent_,Ref<Renderer> renderer_,
	const int& particleCount_, const float& particleRadius_, const Vec3& initialVelocity_,
	Ref<MaterialComponent> material_, Ref<MeshComponent> instance_) : Component(parent_, renderer_), particleCount(particleCount_), 
	material(material_), instance(instance_), timeSinceLastSpawn(0.0f), particleRadius(particleRadius_), initialVelocity(initialVelocity_), 
	bBPosition(Vec3(0.0f, 2.0f, -5.0f)),bBHalfExtents(Vec3(10.0f, 10.0f, 10.0f)), dampening(0.96f), subFrameIterations(300), spawnDelay(0.1f)
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
	std::uniform_real_distribution<float> distribution(-25.0f, 25.0f);

	isCreated = true;
	return isCreated;
}

void ParticleComponent::OnDestroy()
{
	instance = nullptr;
}

void ParticleComponent::Update(const float deltaTime_)
{
	timeSinceLastSpawn += deltaTime_;
	if (particles.size() < particleCount && timeSinceLastSpawn >= spawnDelay)
	{
		// spawn particles
		for (int i = 0; i < 5; i++)
		{
			Ref<Particle> p = std::make_shared<Particle>(bBPosition + Vec3::up() * i, particleRadius);
			p->velocity = initialVelocity;
			particles.push_back(p);
		}

		// Reset timer
		timeSinceLastSpawn = 0.0f;
	}

	/* KD Tree neares neighbor (Not functional yet & requires rebuilding tree after every particle movement) very inefficient
	if (particles.size() >= 1)
	{
		particleTree = KDTree::insert(nullptr, std::vector<float> {particles[0]->position.x, particles[0]->position.y, particles[0]->position.z}, 0);
	}

	if (particles.size() >= 2)
	{
		for (int i = 1; i < particles.size(); i++)
		{
			KDTree::insert(particleTree, std::vector<float> {particles[i]->position.x, particles[i]->position.y, particles[i]->position.z}, i);
		}
	}
	*/
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
			MMath::scale(Vec3(1.0f, 1.0f, 1.0f) * 0.5f) * // Sphere.obj is has a radius of 2m
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
	Vec3 collisionForce = Vec3();

	if (particles.size() >= 2)
	{
		for (int i = 0; i < subFrameIterations; i++)
		{
			int index = IndexOfNearest(p);
			if (index == -1) break;

			Vec3 dir = p->position - particles[index]->position;
			float distance = VMath::mag(dir);
			if (distance > VERY_SMALL) dir = VMath::normalize(dir);
			//else dir = Vec3(0.5f, 0.5f, 0.0f);
			if (distance < 2 * p->radius)
			{
				//p->position -= dir * ((2 * p->radius) - distance);
				collisionForce += dir * ((2 * p->radius) - distance);
			}
		}
	}

	Vec3 netForce = force + collisionForce;

	// Apply forces on the particle
	p->acceleration = netForce / p->mass;
	p->velocity += p->acceleration * deltaTime;
	p->position += p->velocity * deltaTime;

	// Clamp position to be inside bounding box

	// Clamp Y Position
	{
		p->position.y = std::max(p->position.y, bBPosition.y - bBHalfExtents.y);

		if (p->position.y <= bBPosition.y - bBHalfExtents.y)
		{
			p->velocity.y *= -dampening;
		}
	}

	// Clamp X & Z Position
	{
		//p->position.x = std::clamp(p->position.x, bBPosition.x - bBHalfExtents.x, bBPosition.x + bBHalfExtents.x);

		if (abs(p->position.x - bBPosition.z) >= bBHalfExtents.x)
		{
			p->velocity.x *= -dampening;
		}

		//p->position.z = std::clamp(p->position.z, bBPosition.z - bBHalfExtents.z, bBPosition.z + bBHalfExtents.z);

		if (abs(p->position.z - bBPosition.z) >= bBHalfExtents.z)
		{
			p->velocity.z *= -dampening;
		}
	}

	// Dampen velocity to add air resistance and friction
	p->velocity *= 0.995f;


}

int ParticleComponent::IndexOfNearest(Ref<Particle> p)
{

	//return KDTree::NearestNeighbor(particleTree, std::vector<float> {p->position.x, p->position.y, p->position.z});

	float minDistance = 1000000.0f;
	float index = -1;
	for (int i = 0; i < particles.size(); i++)
	{
		if (particles[i] != p)
		{
			float distance = VMath::mag(particles[i]->position - p->position);
			if (distance < minDistance)
			{
				minDistance = distance;
				index = i;
			}
		}
	}
	return index;
}

Particle::Particle(	Vec3 position_, float radius_, Vec3 velocity_, Vec3 acceleration_, float mass_) :
	// Initialization
	position(position_), 
	radius(radius_), 
	velocity(velocity_), 
	acceleration(acceleration_), 
	mass(mass_)
{

}

