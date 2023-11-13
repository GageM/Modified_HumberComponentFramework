#pragma once
#include <glew.h>
#include "Actor.h"

//Forward Declarations
class MeshComponent;
class ShaderComponent;
class CubemapComponent;

class Skybox : public Actor
{
private:
	Ref<MeshComponent> mesh;
	Ref<ShaderComponent> shader;
	Ref<ShaderComponent> hdrShader;
	Ref<CubemapComponent> cubemap;

	GLuint hdrTextureID;

	void RenderDefaultCubemap() const;
	void RenderHDRI() const;

	bool useIBL = false;

public:
	Skybox(Ref<Component> parent_, Ref<Renderer> renderer_);
	Skybox(Ref<Component> parent_, Ref<Renderer> renderer_, Ref<MeshComponent> mesh_, Ref<ShaderComponent> shader_, Ref<CubemapComponent> cubemap_);
	~Skybox();

	inline Ref<ShaderComponent> GetShader() { return shader; }
	inline Ref<ShaderComponent> GetHDRShader() { return hdrShader; }
	inline Ref<MeshComponent> GetMesh() { return mesh; }
	inline Ref<CubemapComponent> GetCubemap() { return cubemap; }

	virtual bool OnCreate() override;
	virtual void OnDestroy() override;
	virtual void Update(const float deltaTime_) override;
	virtual void Render()const override;

	void LoadHDRI(const char* filename_);
};

