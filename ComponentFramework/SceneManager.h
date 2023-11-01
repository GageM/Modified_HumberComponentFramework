#ifndef SCENEMANAGER_H
#define SCENEMANAGER_H

#include <string>
#include "Component.h"
#include "Renderer.h"
#include "OpenGLRenderer.h"
#include <thread>

class ControllerManager;
class Profiler;

class SceneManager  {
public:
	
	SceneManager();
	~SceneManager();
	void Run();
	bool Initialize(std::string name_, int width_, int height_);
	void HandleEvents();
	void HandleGUI();
	void Update();
	void Render();
	
	
private:
	enum class SCENE_NUMBER {
		SCENE0 = 0,
		SCENE1,
		SCENE2,
		SCENE3,
		SCENE4,
		SCENE5,
		SCENE6
	};

	class Scene* currentScene;
	class Timer* timer;

	RendererType rendererType;
	Ref<Renderer> renderer;

	Ref<OpenGLRenderer> glRenderer;
	
	Ref<ControllerManager> controllerManager;
	
	unsigned int fps;
	bool isRunning;
	bool fullScreen;
	void BuildNewScene(SCENE_NUMBER scene_);

	bool show_demo_window;
	const bool enableProfilers = false;

	Ref<Profiler> updateProfiler;
	Ref<Profiler> renderProfiler;
	Ref<Profiler> handleEventsProfiler;
};


#endif // SCENEMANAGER_H