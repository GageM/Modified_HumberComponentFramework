#pragma once
#include "Renderer.h"
class OpenGLRenderer : public Renderer
{
public:
	OpenGLRenderer();
	~OpenGLRenderer();

	virtual bool OnCreate();
	virtual void OnDestroy();
	virtual void Render();

	virtual SDL_Window* NewWindow(std::string name_, int width_, int height_);

	SDL_Window* GetWindow() const { return window; }
	SDL_GLContext GetContext() const { return context; }


private:
	int width;
	int height;
	SDL_Window* window;
	SDL_GLContext context;
	void setAttributes(int major_, int minor_);
	void getInstalledOpenGLInfo(int* major, int* minor);

};

