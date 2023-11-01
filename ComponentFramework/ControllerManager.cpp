#include "ControllerManager.h"

ControllerManager::ControllerManager() : isCreated(false)
{
}

ControllerManager::~ControllerManager()
{
}

bool ControllerManager::OnCreate()
{
	if (isCreated) return true;


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
		controllers.push_back(SDL_GameControllerOpen(sdlEvent.cdevice.which));
		break;
	case SDL_CONTROLLERDEVICEREMOVED:
		for (int i = 0; i < controllers.size(); i++)
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
			controllers.push_back(SDL_GameControllerOpen(i));
		}
	}
}

SDL_GameController* ControllerManager::GetController()
{

	if (controllers[0] != nullptr)
	{
		return controllers[0];
	}
	return nullptr;
}
