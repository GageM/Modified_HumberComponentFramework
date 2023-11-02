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
	Ref<CubemapComponent> cubemap;

public:
	Skybox(Ref<Component> parent_);
	~Skybox();

	inline Ref<ShaderComponent> GetShader() { return shader; }
	inline Ref<MeshComponent> GetMesh() { return mesh; }
	inline Ref<CubemapComponent> GetCubemap() { return cubemap; }

	virtual bool OnCreate() override;
	virtual void OnDestroy() override;
	virtual void Update(const float deltaTime_) override;
	virtual void Render()const override;
};

