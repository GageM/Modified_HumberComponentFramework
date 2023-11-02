#include "Material.h"
#include "Shader.h"
#include "Texture.h"
#include "Debug.h"

Material::Material() : Component(nullptr), baseColorMap(nullptr), roughnessMap(nullptr), metallicMap(nullptr), normalMap(nullptr)

{
	useBaseColorMap = false;
	useRoughnessMap = false;
	useMetallicMap = false;
	useNormalMap = false;

	baseColorMapFile = nullptr;
	roughnessMapFile = nullptr;
	metallicMapFile = nullptr;
	normalMapFile = nullptr;

	// Default to a light grey dielectric material

	baseColor = Vec4(0.7f, 0.7f, 0.7f, 1.0f);
	roughness = 0.5f;
	metallic = 0.0f;
}

Material::Material(const char* baseColorMapFile_, const char* roughnessMapFile_, const char* metallicMapFile_) : 
	Component(nullptr), shader(nullptr),
	useBaseColorMap(true), useRoughnessMap(true), useMetallicMap(true), useNormalMap(false),
	baseColorMapFile(baseColorMapFile_), roughnessMapFile(roughnessMapFile_), metallicMapFile(metallicMapFile_), normalMapFile(nullptr),
	baseColorMap(nullptr), roughnessMap(nullptr), metallicMap(nullptr), normalMap(nullptr),
	baseColor(Vec4(1.0f, 1.0f, 1.0f, 1.0f)), roughness(1.0f), metallic(0.0f)
{

}

Material::Material(const char* baseColorMapFile_, const char* roughnessMapFile_, const char* metallicMapFile_, const char* normalMapFile_) : 
	Component(nullptr), shader(nullptr),
	useBaseColorMap(true), useRoughnessMap(true), useMetallicMap(true), useNormalMap(true),
	baseColorMapFile(baseColorMapFile_), roughnessMapFile(roughnessMapFile_), metallicMapFile(metallicMapFile_), normalMapFile(normalMapFile_),
	baseColorMap(nullptr), roughnessMap(nullptr), metallicMap(nullptr), normalMap(nullptr),
	baseColor(Vec4(1.0f, 1.0f, 1.0f, 1.0f)), roughness(1.0f), metallic(0.0f)

{

}

Material::Material(const char* baseColorMapFile_) : 
	Component(nullptr), shader(nullptr),
	useBaseColorMap(true), useRoughnessMap(false), useMetallicMap(false), useNormalMap(false),
	baseColorMapFile(baseColorMapFile_), roughnessMapFile(nullptr), metallicMapFile(nullptr), normalMapFile(nullptr),
	baseColorMap(nullptr), roughnessMap(nullptr), metallicMap(nullptr), normalMap(nullptr),
	baseColor(Vec4(1.0f, 1.0f, 1.0f, 1.0f)), roughness(0.5f), metallic(0.0f)

{

}

Material::Material(const char* baseColorMapFile_, float roughness_, float metallic_) : 
	Component(nullptr), shader(nullptr),
	useBaseColorMap(true), useRoughnessMap(false), useMetallicMap(false), useNormalMap(false),
	baseColorMapFile(baseColorMapFile_), roughnessMapFile(nullptr), metallicMapFile(nullptr), normalMapFile(nullptr),
	baseColorMap(nullptr), roughnessMap(nullptr), metallicMap(nullptr), normalMap(nullptr),
	baseColor(Vec4(1.0f, 1.0f, 1.0f, 1.0f)), roughness(roughness_), metallic(metallic_)

{

}

Material::Material(Vec4 baseColor_, float roughness_, float metallic_) : 
	Component(nullptr), shader(nullptr),
	useBaseColorMap(false), useRoughnessMap(false), useMetallicMap(false), useNormalMap(false),
	baseColorMapFile(nullptr), roughnessMapFile(nullptr), metallicMapFile(nullptr), normalMapFile(nullptr),
	baseColorMap(nullptr), roughnessMap(nullptr), metallicMap(nullptr), normalMap(nullptr),
	baseColor(baseColor_), roughness(roughness_), metallic(metallic_)

{

}

Material::~Material()
{

}

bool Material::OnCreate()
{
	shader = new Shader(nullptr, "shaders/PBRVert.glsl", "shaders/PBRFrag.glsl");
	if (!shader->OnCreate())
	{
		Debug::Info("Failed to create Shader", __FILE__, __LINE__);
		return false;
	}

	baseColorMap = new Texture();
	if (useBaseColorMap)
	{
		baseColorMap->LoadImage(baseColorMapFile);
	}

	roughnessMap = new Texture();
	if (useRoughnessMap)
	{
		roughnessMap->LoadImage(roughnessMapFile);
	}

	metallicMap = new Texture();
	if (useMetallicMap)
	{
		metallicMap->LoadImage(metallicMapFile);
	}

	normalMap = new Texture();
	if (useNormalMap)
	{
		normalMap->LoadImage(normalMapFile);
	}

	return true;
}

void Material::OnDestroy()
{
	shader->OnDestroy();
	delete shader;

	if (baseColorMap)
	{
		delete baseColorMap;
		Debug::Info("Deleted Base Color Map", __FILE__, __LINE__);
	}

	if (roughnessMap)
	{
		delete roughnessMap;
		Debug::Info("Deleted Roughness Map", __FILE__, __LINE__);
	}

	if (metallicMap)
	{
		delete metallicMap;
		Debug::Info("Deleted Metallic Map", __FILE__, __LINE__);
	}

	if (normalMap)
	{
		delete normalMap;
		Debug::Info("Deleted Normal Map", __FILE__, __LINE__);
	}

	Debug::Info("Deleted Material", __FILE__, __LINE__);
}

void Material::Render()const
{
	glUseProgram(shader->GetProgram());

	// Pass bools for which material inputs to use
	glUniform1i(glGetUniformLocation(shader->GetProgram(), "useBaseColorMap"), useBaseColorMap);
	glUniform1i(glGetUniformLocation(shader->GetProgram(), "useRoughnessMap"), useRoughnessMap);
	glUniform1i(glGetUniformLocation(shader->GetProgram(), "useMetallicMap"), useMetallicMap);
	glUniform1i(glGetUniformLocation(shader->GetProgram(), "useNormalMap"), false); // TODO: Update after setting up shader for normal maps

	// pass raw material input values
	glUniform4fv(glGetUniformLocation(shader->GetProgram(), "baseColor_"), 4, Vec4(1.0f, 1.0f, 1.0f, 1.0f));
	glUniform1f(glGetUniformLocation(shader->GetProgram(), "roughness_"), roughness);
	glUniform1f(glGetUniformLocation(shader->GetProgram(), "metallic_"), metallic);

	// pass material textures
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, baseColorMap->getTextureID());

	glActiveTexture(GL_TEXTURE1);
	glBindTexture(GL_TEXTURE_2D, roughnessMap->getTextureID());

	glActiveTexture(GL_TEXTURE2);
	glBindTexture(GL_TEXTURE_2D, metallicMap->getTextureID());

	//glActivateTexture(GL_TEXTURE3);
	//glBindTexture(GL_TEXTURE_2D, normalMap->getTextureID());

}

void Material::Update(const float deltaTime)
{

}