#pragma once

// Based upon "Renderer.h" from Scott Fielder.

#include <SDL.h>
#include <string>

enum class RendererType
{
	NONE,
	OPENGL,
	VULKAN,
	DIRECTX11,
	DIRECTX12
};

class Renderer
{
public:
	Renderer() : renderer(nullptr), rendererType(RendererType::NONE) {}

	virtual ~Renderer() {}
	virtual SDL_Window* NewWindow(std::string name_, int width_, int height_) = 0;
	virtual bool OnCreate() = 0;
	virtual void OnDestroy() = 0;
	virtual void Render() = 0;
	RendererType GetRendererType() { return rendererType; }
	void SetRendererType(RendererType rendererType_) { rendererType = rendererType_; }

protected:
	Renderer* renderer;
	RendererType rendererType;
};