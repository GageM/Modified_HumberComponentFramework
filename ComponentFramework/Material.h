#pragma once

#include "Vector.h"
#include "Component.h"

using namespace MATH;

// Forward Declarations 
class Shader;
class Texture;

class Material : public Component
{
public:
	Material();
	Material(const char* baseColorMapFile_, const char* roughnessMapFile_, const char* metallicMapFile_);
	Material(const char* baseColorMapFile_, const char* roughnessMapFile_, const char* metallicMapFile_, const char* normalMapFile_);
	Material(const char* baseColorMapFile_);
	Material(const char* baseColorMapFile_, float roughness_, float metallic_);
	Material(Vec4 baseColor_, float roughness_, float metallic_);
	~Material();

	bool OnCreate();
	void OnDestroy();
	void Render()const override;
	void Update(const float deltaTime) override;

	bool useBaseColorMap;
	bool useRoughnessMap;
	bool useMetallicMap;
	bool useNormalMap;

	const char* baseColorMapFile;
	const char* roughnessMapFile;
	const char* metallicMapFile;
	const char* normalMapFile;

	inline Vec4 GetBaseColor() { return baseColor; }
	inline void SetBaseColor(Vec4 baseColor_) { baseColor = baseColor_; }
	inline float GetRoughness() { return roughness; }
	inline void SetRoughness(float roughness_) { roughness = roughness_; }
	inline float GetMetallic() { return metallic; }
	inline void SetMetallic(float metallic_) { metallic = metallic_; }

	inline Shader* GetShader() { return shader; }
	inline Texture* GetBaseColorMap() { return baseColorMap; }
	inline Texture* GetRoughnessMap() { return roughnessMap; }
	inline Texture* GetMetallicMap() { return metallicMap; }

private:
	Shader* shader;
	Texture* baseColorMap;
	Texture* roughnessMap;
	Texture* metallicMap;
	Texture* normalMap;

	Vec4 baseColor;
	float roughness;
	float metallic;
};
