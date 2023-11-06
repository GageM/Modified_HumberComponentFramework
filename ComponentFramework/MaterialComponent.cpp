#include "MaterialComponent.h"
#include "ShaderComponent.h"
#include "TextureComponent.h"
#include "CubemapComponent.h"
#include "Debug.h"

MaterialComponent::MaterialComponent(Ref<Component> parent_, RendererType renderer_, Ref<TextureComponent> BCMap_, Ref<TextureComponent> RoughMap_, Ref<TextureComponent> MetMap_) : Component(parent_, renderer_),
useBaseColorMap(true), useNormalMap(false),
baseColorMap(BCMap_), roughnessMap(RoughMap_), metallicMap(MetMap_),
baseColor(Vec4(0.7f, 0.7f, 0.7f, 1.0f)), roughness(0.5f), metallic(0.0f)
{
	if (RoughMap_) useRoughnessMap = true;
	else useRoughnessMap = false;

	if (MetMap_) useMetallicMap = true;
	else useMetallicMap = false;
}

MaterialComponent::MaterialComponent(Ref<Component> parent_, RendererType renderer_, Ref<TextureComponent> BCMap_, float roughness_, float metallic_) : Component(parent_, renderer_),
useBaseColorMap(true), useRoughnessMap(false), useMetallicMap(false), useNormalMap(false),
baseColorMap(BCMap_), roughnessMap(nullptr), metallicMap(nullptr),
baseColor(Vec4(0.7f, 0.7f, 0.7f, 1.0f)), roughness(roughness_), metallic(metallic_)
{

}

MaterialComponent::MaterialComponent(Ref<Component> parent_, RendererType renderer_, Vec4 baseColor_, float roughness_, float metallic_) : Component(parent_, renderer_),
useBaseColorMap(false), useRoughnessMap(false), useMetallicMap(false), useNormalMap(false), 
baseColorMap(nullptr), roughnessMap(nullptr), metallicMap(nullptr),
baseColor(baseColor_), roughness(roughness_), metallic(metallic_)
{


}

MaterialComponent::~MaterialComponent()
{
	OnDestroy();
}

bool MaterialComponent::OnCreate()
{
	return true;
}

void MaterialComponent::OnDestroy()
{
	if (shader) shader = nullptr;
	if (baseColorMap) baseColorMap = nullptr;
	if (roughnessMap) roughnessMap = nullptr;
	if (metallicMap) metallicMap = nullptr;

	Debug::Info("Deleted MaterialComponent", __FILE__, __LINE__);
}

void MaterialComponent::Render()const
{
	switch (renderer)
	{
	case RendererType::NONE:
		break;
	case RendererType::OPENGL:
	{
		// This assumes that a PBR shader is used!

		glUseProgram(shader->GetProgram());

		// Pass bools for which material inputs to use
		glUniform1i(glGetUniformLocation(shader->GetProgram(), "useBaseColorMap"), useBaseColorMap);
		glUniform1i(glGetUniformLocation(shader->GetProgram(), "useRoughnessMap"), useRoughnessMap);
		glUniform1i(glGetUniformLocation(shader->GetProgram(), "useMetallicMap"), useMetallicMap);
		glUniform1i(glGetUniformLocation(shader->GetProgram(), "useNormalMap"), false); // TODO: Update after setting up shader for normal maps

		// pass raw material input values
		glUniform4fv(glGetUniformLocation(shader->GetProgram(), "baseColor_"), 1, baseColor);
		glUniform1f(glGetUniformLocation(shader->GetProgram(), "roughness_"), roughness);
		glUniform1f(glGetUniformLocation(shader->GetProgram(), "metallic_"), metallic);

		// pass material textures
		if (useBaseColorMap)
		{
			glActiveTexture(GL_TEXTURE0);
			glBindTexture(GL_TEXTURE_2D, baseColorMap->getTextureID());
		}

		if (useRoughnessMap)
		{
			glActiveTexture(GL_TEXTURE1);
			glBindTexture(GL_TEXTURE_2D, roughnessMap->getTextureID());
		}

		if (useMetallicMap)
		{
			glActiveTexture(GL_TEXTURE2);
			glBindTexture(GL_TEXTURE_2D, metallicMap->getTextureID());
		}

		//glActivateTexture(GL_TEXTURE3);
		//glBindTexture(GL_TEXTURE_2D, normalMap->getTextureID());

		// pass the cubemap
		glActiveTexture(GL_TEXTURE4);
		glBindTexture(GL_TEXTURE_CUBE_MAP, cubemap->getTextureID());

		break;
	}
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

void MaterialComponent::PostRender() const
{
	switch (renderer)
	{
	case RendererType::NONE:
		break;
	case RendererType::OPENGL:
	{
		//glUseProgram(0);
		glActiveTexture(GL_TEXTURE0);
		break;
	}
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

void MaterialComponent::Update(const float deltaTime)
{

}