#include <SDL_image.h>
#include "Skybox.h"
#include "Debug.h"
#include "CubemapComponent.h"
#include "MeshComponent.h"
#include "ShaderComponent.h"
#include "TransformComponent.h"
#include <Matrix.h>
#include <MMath.h>

Skybox::Skybox(Ref<Component> parent_) : Actor(parent_), mesh(nullptr), shader(nullptr), cubemap(nullptr)
{

}

Skybox::~Skybox()
{
	OnDestroy();
}

bool Skybox::OnCreate()
{
	// Add Mesh Component
	mesh = std::make_shared<MeshComponent>(nullptr, "meshes/Cube.obj");
	mesh->OnCreate();

	// Add Shader Component
	shader = std::make_shared<ShaderComponent>(nullptr, "shaders/SkyboxVert.glsl", "shaders/SkyboxFrag.glsl");
	shader->OnCreate();

	// Add CubemapComponent
	cubemap = std::make_shared<CubemapComponent>(nullptr, 
		"textures/CN_Tower/posx.jpg", "textures/CN_Tower/posy.jpg", "textures/CN_Tower/posz.jpg",
		"textures/CN_Tower/negx.jpg", "textures/CN_Tower/negy.jpg", "textures/CN_Tower/negz.jpg");
	cubemap->OnCreate();
	
	return true;
}
	
void Skybox::OnDestroy()
{
	if(shader) shader->OnDestroy();
	if (mesh) mesh->OnDestroy();
	if (cubemap) cubemap->OnDestroy();
	shader = nullptr;
	mesh = nullptr;
	cubemap = nullptr;
}

void Skybox::Update(const float deltaTime_)
{

}

void Skybox::Render()const
{
	// use skybox shader
	glUseProgram(shader->GetProgram());

	glDepthFunc(GL_LEQUAL);
	glDisable(GL_CULL_FACE);

	// Set skybox position
	Matrix4 modelMatrix = MMath::translate(Vec3(10.0f, 0.0f, 0.0f)) * Matrix4();

	Ref<Actor> parentActor = std::dynamic_pointer_cast<Actor>(parent);
	if (parentActor)
	{
		Ref<TransformComponent> parentTransform = parentActor->GetComponent<TransformComponent>();
		if (parentTransform)
		{
			modelMatrix = MMath::translate(-parentTransform->pos) * Matrix4();
		}
	}

	// Pass skybox model matrix
	glUniformMatrix4fv(shader->GetUniformID("modelMatrix"), 1, GL_FALSE, modelMatrix);

	// Bind cubemap texture
	glBindTexture(GL_TEXTURE_CUBE_MAP, cubemap->getTextureID());
	// Render skybox
	mesh->Render(GL_TRIANGLES);

	glBindTexture(GL_TEXTURE_CUBE_MAP, 0);
	glDepthFunc(GL_LESS);
	glEnable(GL_CULL_FACE);
}