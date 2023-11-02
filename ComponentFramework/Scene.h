#ifndef SCENE_H
#define SCENE_H
union SDL_Event;
#include "Component.h"
#include "Renderer.h"
#include <mutex>

class ControllerManager;


class Scene{
public:	
	explicit Scene(Ref<Renderer> renderer_, bool showMenu_): renderer(renderer_), showMenu(showMenu_), controller(nullptr), controllerManager(nullptr) {};
	virtual ~Scene() {}
	virtual bool OnCreate() = 0;
	virtual void OnDestroy() = 0;
	virtual void Update(const float deltaTime) = 0;
	virtual void Render() const = 0;
	virtual void HandleEvents(const SDL_Event &sdlEvent) = 0;
	virtual void HandleGUI() = 0;
	
	Ref<ControllerManager> controllerManager;
	SDL_GameController* controller;
	bool showMenu;

protected:
	Ref<Renderer> renderer;
};
#endif