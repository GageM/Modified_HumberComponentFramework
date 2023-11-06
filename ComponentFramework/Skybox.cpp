#include <SDL_image.h>
#include "Skybox.h"
#include "Debug.h"
#include "CubemapComponent.h"
#include "MeshComponent.h"
#include "ShaderComponent.h"
#include "TransformComponent.h"
#include <Matrix.h>
#include <MMath.h>

#define STB_IMAGE_IMPLEMENTATION
#include"stb_image.h"

Skybox::Skybox(Ref<Component> parent_, RendererType renderer_) : Actor(parent_, renderer_), mesh(nullptr), shader(nullptr), cubemap(nullptr)
{

}

Skybox::Skybox(Ref<Component> parent_, RendererType renderer_, Ref<MeshComponent> mesh_, Ref<ShaderComponent> shader_, Ref<CubemapComponent> cubemap_) : Actor(parent_, renderer_), 
mesh(mesh_), shader(shader_), cubemap(cubemap_)
{

}

Skybox::~Skybox()
{
	OnDestroy();
}

bool Skybox::OnCreate()
{
	// Add Mesh Component
	mesh = std::make_shared<MeshComponent>(nullptr, renderer, "meshes/Cube.obj");
	mesh->OnCreate();

	// Add Shader Component
	shader = std::make_shared<ShaderComponent>(nullptr, renderer, "shaders/SkyboxVert.glsl", "shaders/SkyboxFrag.glsl");
	shader->OnCreate();

	hdrShader = std::make_shared<ShaderComponent>(nullptr, renderer, "shaders/HDRIMapVert.glsl", "shaders/HDRIMapFrag.glsl");
	hdrShader->OnCreate();

	// Add CubemapComponent
	cubemap = std::make_shared<CubemapComponent>(nullptr, renderer, "textures/CN_Tower/posx.jpg", "textures/CN_Tower/posy.jpg", "textures/CN_Tower/posz.jpg",
		"textures/CN_Tower/negx.jpg", "textures/CN_Tower/negy.jpg", "textures/CN_Tower/negz.jpg");
	cubemap->OnCreate();

	switch (renderer)
	{
	case RendererType::NONE:
		break;
	case RendererType::OPENGL:
		LoadHDRI("textures/lebombo_4k.hdr");
		break;
	case RendererType::VULKAN:
		break;
	case RendererType::DIRECTX11:
		break;
	case RendererType::DIRECTX12:
		break;
	default:
		break;
	}


	
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
	switch (renderer)
	{
	case RendererType::NONE:
		break;
	case RendererType::OPENGL:
		if (useIBL)
		{
			RenderHDRI();
		}
		else
		{
			RenderDefaultCubemap();
		}
		break;
	case RendererType::VULKAN:
		break;
	case RendererType::DIRECTX11:
		break;
	case RendererType::DIRECTX12:
		break;
	default:
		break;
	}

}

void Skybox::RenderDefaultCubemap() const
{
	// use skybox shader
	glUseProgram(shader->GetProgram());

	glDepthFunc(GL_LEQUAL);
	glDisable(GL_CULL_FACE);

	// Set skybox position
	Matrix4 modelMatrix = Matrix4();

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
	glActiveTexture(GL_TEXTURE4);
	glBindTexture(GL_TEXTURE_CUBE_MAP, cubemap->getTextureID());
	// Render skybox
	mesh->Render(GL_TRIANGLES);

	glBindTexture(GL_TEXTURE_CUBE_MAP, 0);
	glActiveTexture(GL_TEXTURE0);
	glDepthFunc(GL_LESS);
	glEnable(GL_CULL_FACE);
}

void Skybox::RenderHDRI() const
{
	// use skybox shader
	glUseProgram(hdrShader->GetProgram());

	glDepthFunc(GL_LEQUAL);
	glDisable(GL_CULL_FACE);

	// Set skybox position
	Matrix4 modelMatrix = Matrix4();

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
	glActiveTexture(GL_TEXTURE5);
	glBindTexture(GL_TEXTURE_2D, hdrTextureID);
	// Render skybox
	mesh->Render(GL_TRIANGLES);

	glBindTexture(GL_TEXTURE_2D, 0);
	glActiveTexture(GL_TEXTURE0);
	glDepthFunc(GL_LESS);
	glEnable(GL_CULL_FACE);
}

void Skybox::LoadHDRI(const char* filename_)
{
	stbi_set_flip_vertically_on_load(true);
	int width, height, nrComponents;

	float* data = stbi_loadf(filename_, &width, &height, &nrComponents, 0);

	if (data)
	{
		glGenTextures(1, &hdrTextureID);
		glActiveTexture(GL_TEXTURE5);
		glBindTexture(GL_TEXTURE_2D, hdrTextureID);
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RG16F, width, height, 0, GL_RGB, GL_FLOAT, data);

		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

		stbi_image_free(data);
		glActiveTexture(GL_TEXTURE0);

		Debug::Info("Image Loaded", __FILE__, __LINE__);
	}
	else
	{
		Debug::FatalError("Failed to load HDRI", __FILE__, __LINE__);
	}
}