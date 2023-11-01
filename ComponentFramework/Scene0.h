#pragma once
#include "Scene.h"
#include <unordered_map>
#include <vector>
#include "Actor.h"
#include "LightActor.h"
#include "CameraActor.h"

#include <Vector.h>

class Ray;
class ShaderComponent;

struct Scene0 : public Scene
{
private:
	Vec4 bGColor;
	Vec4 debugColor;
	Vec4 selectionColor;

	bool showMenu = false;

	// For Adjusting Selected Actor
	Ref<Actor> selectedActor;
	std::string selectedActorName;
	Ref<TransformComponent> selectedTransform;

	void showTransformMenu();

public:
	bool renderMeshes = true;
	bool renderCollisionShapes = true;
	bool renderRaycasts = true;
	explicit Scene0(Ref<Renderer> renderer_);
	virtual ~Scene0();

	virtual bool OnCreate();
	virtual void OnDestroy();
	virtual void Update(const float deltaTime);
	virtual void Render() const;
	virtual void HandleEvents(const SDL_Event& sdlEvent);

	virtual void HandleGUI();

	// Keep track of Actors from XML asset manager
	std::unordered_map< std::string, Ref<Component>> actors;
	// We only have one camera and light, so they don't need to be in the map
	Ref<CameraActor> camera;
	Ref<LightActor> light;

	std::vector<Ref<Ray>> rays;

	Ref<ShaderComponent> debugShader;

	// TODO for Assignment 2:
	// We'll use a ray to click on our geometry objects 
	//RayIntersectionInfo rayInfo;
};

