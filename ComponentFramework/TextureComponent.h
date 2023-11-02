// Umer Noor 2022
// Original code from/taught by Dr. Scott Fielder. Lucky for me I was in that class (Game Engine 4)

#pragma once
#include "Component.h"
#include "glew.h" // so we can make a GLuint
class TextureComponent: public Component {
	GLuint textureID;
	const char* filename;

	public:
		TextureComponent(Ref<Component> parent_, const char* filename_);
		virtual ~TextureComponent();
		virtual bool OnCreate();
		virtual void OnDestroy();
		virtual void Update(const float deltaTime_);
		virtual void Render()const;
		inline GLuint getTextureID() const { return textureID; }

};

