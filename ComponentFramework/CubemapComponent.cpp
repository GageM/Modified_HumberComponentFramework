#include "CubemapComponent.h"
#include <SDL_image.h>

CubemapComponent::CubemapComponent(Ref<Component> parent_, const char* PXfile_, const char* PYfile_, const char* PZfile_,
	const char* NXfile_, const char* NYfile_, const char* NZfile_) : Component(parent_), 

	textureID(0)
{
	posXfilename = PXfile_;
	posYfilename = PYfile_;
	posZfilename = PZfile_;
	negXfilename = NXfile_;
	negYfilename = NYfile_;
	negZfilename = NZfile_;
}

CubemapComponent::~CubemapComponent()
{
}

bool CubemapComponent::OnCreate()
{
	if (isCreated) return true;

	glGenTextures(1, &textureID);
	glBindTexture(GL_TEXTURE_CUBE_MAP, textureID);

	SDL_Surface* textureSurface;
	int mode;

	// Load pos X image
	if (LoadFaceTexture(GL_TEXTURE_CUBE_MAP_POSITIVE_X, posXfilename) == false)
	{
		return false;
	}

	// Load pos Y image
	if (LoadFaceTexture(GL_TEXTURE_CUBE_MAP_POSITIVE_Y, posYfilename) == false)
	{
		return false;
	}
	
	// Load pos Z image
	if (LoadFaceTexture(GL_TEXTURE_CUBE_MAP_POSITIVE_Z, posZfilename) == false)
	{
		return false;
	}

	// Load neg X image
	if (LoadFaceTexture(GL_TEXTURE_CUBE_MAP_NEGATIVE_X, negXfilename) == false)
	{
		return false;
	}

	// Load neg Y image
	if (LoadFaceTexture(GL_TEXTURE_CUBE_MAP_NEGATIVE_Y, negYfilename) == false)
	{
		return false;
	}

	// Load neg Y image
	if (LoadFaceTexture(GL_TEXTURE_CUBE_MAP_NEGATIVE_Z, negZfilename) == false)
	{
		return false;
	}

	// Cubemapping Settins
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);

	isCreated = true;
	return isCreated;
}

void CubemapComponent::OnDestroy()
{
	// Free up the memory on the GPU
	glDeleteTextures(1, &textureID);
	isCreated = false;
}

void CubemapComponent::Update(const float deltaTime_)
{
}

void CubemapComponent::Render() const
{
}

bool CubemapComponent::LoadFaceTexture(GLenum face, const char* filename)
{
	SDL_Surface* textureSurface;
	int mode;

	textureSurface = IMG_Load(filename);
	if (textureSurface == nullptr) {
		return false;
	}
	// Assign to pos X direction of a cubemap
	mode = (textureSurface->format->BytesPerPixel == 4) ? GL_RGBA : GL_RGB;
	glTexImage2D(face, 0, mode, textureSurface->w, textureSurface->h, 0, mode, GL_UNSIGNED_BYTE, textureSurface->pixels);
	SDL_FreeSurface(textureSurface);

	return true;
}
