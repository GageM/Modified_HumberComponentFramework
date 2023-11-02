#pragma once

#include <SDL.h>
#include <string>
#include <iostream>
#include "Debug.h"
#include <vector>
// Gives access to Ref
#include "Component.h"

class ControllerManager
{
private:
	std::vector<SDL_GameController*> controllers;

	bool isCreated;
public:
	ControllerManager(const ControllerManager&) = delete;
	ControllerManager(ControllerManager&&) = delete;
	ControllerManager& operator=(const ControllerManager&) = delete;
	ControllerManager& operator=(ControllerManager&&) = delete;

	ControllerManager();
	~ControllerManager();

	bool OnCreate();
	void OnDestroy();

	void HandleEvents(const SDL_Event& sdlEvent);

	void FindConnectedControllers();

	SDL_GameController* GetController();
};

