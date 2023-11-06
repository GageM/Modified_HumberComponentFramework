#pragma once
#include "Component.h"
#include <glew.h>

class CubemapComponent: public Component
{
public:
	CubemapComponent(Ref<Component> parent_, RendererType renderer_, const char* PXfile_, const char* PYfile_, const char* PZfile_,
										 const char* NXfile_, const char* NYfile_, const char* NZfile_);
	virtual ~CubemapComponent();
	virtual bool OnCreate();
	virtual void OnDestroy();
	virtual void Update(const float deltaTime_);
	virtual void Render()const;
	inline GLuint getTextureID() const { return textureID; }

private:
	bool isCreated = false;

	GLuint textureID;
	const char* posXfilename;
	const char* posYfilename;
	const char* posZfilename;
	const char* negXfilename;
	const char* negYfilename;
	const char* negZfilename;

	bool LoadFaceTexture(GLenum face, const char* filename);
};

