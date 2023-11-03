#pragma once

#include "Vector.h"
#include "Component.h"

using namespace MATH;

// Forward Declarations 
class ShaderComponent;
class TextureComponent;
class CubemapComponent;

class MaterialComponent : public Component
{
public:
	MaterialComponent(Ref<Component> parent_, Ref<TextureComponent> BCMap_, Ref<TextureComponent> RoughMap_ = nullptr, Ref<TextureComponent> MetMap_ = nullptr);
	MaterialComponent(Ref<Component> parent_, Ref<TextureComponent> BCMap_, float roughness_ = 0.5f, float metallic_ = 0.0f);
	MaterialComponent(Ref<Component> parent_, Vec4 baseColor_ = Vec4(0.7f, 0.7f, 0.7f, 1.0f), float roughness_ = 0.5f, float metallic_ = 0.0f);
	~MaterialComponent();

	bool OnCreate();
	void OnDestroy();
	void Render()const override;
	void Update(const float deltaTime) override;

	void PostRender() const;

	bool useBaseColorMap;
	bool useRoughnessMap;
	bool useMetallicMap;
	bool useNormalMap;

	Vec4 baseColor;
	float roughness;
	float metallic;

	inline Ref<ShaderComponent> GetShader() { return shader; }
	inline void SetShader(Ref<ShaderComponent> shader_) { shader = shader_; }

	inline Ref<CubemapComponent> GetCubemap() { return cubemap; }
	inline void SetCubemap(Ref<CubemapComponent> cubemap_) { cubemap = cubemap_; }

	inline Ref<TextureComponent> GetBaseColorMap() { return baseColorMap; }
	inline Ref<TextureComponent> GetRoughnessMap() { return roughnessMap; }
	inline Ref<TextureComponent> GetMetallicMap() { return metallicMap; }

	inline void SetBaseColorMap(Ref<TextureComponent> map_) { baseColorMap = map_; }
	inline void SetRoughnessMap(Ref<TextureComponent> map_) { roughnessMap = map_; }
	inline void SetMettallicMap(Ref<TextureComponent> map_) { metallicMap = map_; }
	inline void SetNormalMap(Ref<TextureComponent> map_) { normalMap = map_; }

private:
	Ref<ShaderComponent> shader;

	Ref<CubemapComponent> cubemap;

	Ref<TextureComponent> baseColorMap;
	Ref<TextureComponent> roughnessMap;
	Ref<TextureComponent> metallicMap;
	Ref<TextureComponent> normalMap;
};
