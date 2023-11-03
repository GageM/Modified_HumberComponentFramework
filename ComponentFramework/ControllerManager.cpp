#include "ControllerManager.h"

ControllerManager::ControllerManager() : isCreated(false)
{
}

ControllerManager::~ControllerManager()
{
	for (auto& controller : controllers)
	{
		controller = nullptr;
	}
	controllers.clear();
}

bool ControllerManager::OnCreate()
{
	if (isCreated) return true;

	FindConnectedControllers();

	isCreated = true;
	return isCreated;
}

void ControllerManager::OnDestroy()
{
}

void ControllerManager::HandleEvents(const SDL_Event& sdlEvent)
{
	switch (sdlEvent.type)
	{
	case SDL_CONTROLLERDEVICEADDED:
	{
		SDL_GameController* controller = SDL_GameControllerOpen(sdlEvent.cdevice.which);
		controllers.push_back(controller);
		break;
	}
	case SDL_CONTROLLERDEVICEREMOVED:
		for (unsigned int i = 0; i < controllers.size(); i++)
		{
			if (controllers[i] && sdlEvent.cdevice.which == SDL_JoystickInstanceID(SDL_GameControllerGetJoystick(controllers[i])))
			{
				SDL_GameControllerClose(controllers[i]);
				controllers.erase(controllers.begin() + i );
			}
		}
		break;
	default:
		break;
	}
}

void ControllerManager::FindConnectedControllers()
{
	for (int i = 0; i < SDL_NumJoysticks(); i++)
	{
		if (SDL_IsGameController(i))
		{
			SDL_GameController* controller = SDL_GameControllerOpen(i);
			controllers.push_back(controller);
		}
	}
}

SDL_GameController* ControllerManager::GetController()
{
	if (controllers.size() > 0 && controllers[0] != nullptr)
	{
		return controllers[0];
	}
	return nullptr;
}
