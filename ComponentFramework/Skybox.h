#pragma once
#include <glew.h>
#include "Component.h"
#include "Camera.h"

//Forward Declarations
class Mesh;
class Shader;

class Skybox : public Component
{
private:
	GLuint textureID;
	Mesh* mesh;
	Shader* shader;
	const char *posXfilename, *posYfilename, *posZfilename, *negXfilename, *negYfilename, *negZfilename;

public:
	Skybox(const char *posXfilename_, const char *posYfilename_, const char *posZfilename_, 
		   const char *negXfilename_, const char *negYfilename_, const char *negZfilename_);
	~Skybox();

	inline Shader* GetShader() { return shader; }
	inline GLuint GetTextureID() { return textureID; }

	bool OnCreate() override;
	void OnDestroy() override;
	void Update(const float deltaTime_) override;
	void Render()const override;
};

