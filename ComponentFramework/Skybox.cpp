#include <SDL_image.h>
#include "Skybox.h"
#include "Mesh.h"
#include "Shader.h"
#include "Debug.h"

Skybox::Skybox(const char* posXfilename_, const char* posYfilename_, const char* posZfilename_,
		       const char* negXfilename_, const char* negYfilename_, const char* negZfilename_) : Component(parent)
{
	posXfilename = posXfilename_;
	posYfilename = posYfilename_;
	posZfilename = posZfilename_;
	negXfilename = negXfilename_;
	negYfilename = negYfilename_;
	negZfilename = negZfilename_;

	textureID = 0;
	mesh = nullptr;
	shader = nullptr;
}

Skybox::~Skybox()
{
	
}

bool Skybox::OnCreate()
{
	mesh = new Mesh(nullptr, "meshes/Cube.obj");
	if (!mesh->OnCreate())
	{
		Debug::Info("Failed to create Skybox Mesh", __FILE__, __LINE__);
		return false;
	}

	shader = new Shader(nullptr, "shaders/SkyboxVert.glsl", "shaders/SkyboxFrag.glsl");
	if (!shader->OnCreate())
	{
		Debug::Info("Failed to create Skybox Shader", __FILE__, __LINE__);
		return false;
	}

	glGenTextures(1, &textureID);
	glBindTexture(GL_TEXTURE_CUBE_MAP, textureID);

	SDL_Surface* textureSurface;
	int mode;

	//
	textureSurface = IMG_Load(posXfilename);
	if (textureSurface == nullptr) {
		return false;
	}
	mode = (textureSurface->format->BytesPerPixel == 4) ? GL_RGBA : GL_RGB;
	glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X, 0, mode, textureSurface->w, textureSurface->h, 0, mode, GL_UNSIGNED_BYTE, textureSurface->pixels);
	SDL_FreeSurface(textureSurface);
	//
	textureSurface = IMG_Load(posYfilename);
	if (textureSurface == nullptr) {
		return false;
	}
	mode = (textureSurface->format->BytesPerPixel == 4) ? GL_RGBA : GL_RGB;
	glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_Y, 0, mode, textureSurface->w, textureSurface->h, 0, mode, GL_UNSIGNED_BYTE, textureSurface->pixels);
	SDL_FreeSurface(textureSurface);
	//
	textureSurface = IMG_Load(posZfilename);
	if (textureSurface == nullptr) {
		return false;
	}
	mode = (textureSurface->format->BytesPerPixel == 4) ? GL_RGBA : GL_RGB;
	glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_Z, 0, mode, textureSurface->w, textureSurface->h, 0, mode, GL_UNSIGNED_BYTE, textureSurface->pixels);
	SDL_FreeSurface(textureSurface);
	//
	textureSurface = IMG_Load(negXfilename);
	if (textureSurface == nullptr) {
		return false;
	}
	mode = (textureSurface->format->BytesPerPixel == 4) ? GL_RGBA : GL_RGB;
	glTexImage2D(GL_TEXTURE_CUBE_MAP_NEGATIVE_X, 0, mode, textureSurface->w, textureSurface->h, 0, mode, GL_UNSIGNED_BYTE, textureSurface->pixels);
	SDL_FreeSurface(textureSurface);
	//
	textureSurface = IMG_Load(negYfilename);
	if (textureSurface == nullptr) {
		return false;
	}
	mode = (textureSurface->format->BytesPerPixel == 4) ? GL_RGBA : GL_RGB;
	glTexImage2D(GL_TEXTURE_CUBE_MAP_NEGATIVE_Y, 0, mode, textureSurface->w, textureSurface->h, 0, mode, GL_UNSIGNED_BYTE, textureSurface->pixels);
	SDL_FreeSurface(textureSurface);
	//
	textureSurface = IMG_Load(negZfilename);
	if (textureSurface == nullptr) {
		return false;
	}
	mode = (textureSurface->format->BytesPerPixel == 4) ? GL_RGBA : GL_RGB;
	glTexImage2D(GL_TEXTURE_CUBE_MAP_NEGATIVE_Z, 0, mode, textureSurface->w, textureSurface->h, 0, mode, GL_UNSIGNED_BYTE, textureSurface->pixels);
	SDL_FreeSurface(textureSurface);

	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);

	return true;
}
	
void Skybox::OnDestroy()
{
	if (mesh)
	{
		mesh->OnDestroy();
		delete mesh;
	}

	if (shader)
	{
		shader->OnDestroy();
		delete shader;
	}

	glDeleteTextures(1, &textureID);
}

void Skybox::Update(const float deltaTime_)
{

}

void Skybox::Render()const
{
	glDepthFunc(GL_LEQUAL);
	glDisable(GL_CULL_FACE);
	glBindTexture(GL_TEXTURE_CUBE_MAP, textureID);
	mesh->Render(GL_TRIANGLES);
	glBindTexture(GL_TEXTURE_CUBE_MAP, 0);
	glDepthFunc(GL_LESS);
	glEnable(GL_CULL_FACE);
}