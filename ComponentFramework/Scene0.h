#pragma once
#include "Scene.h"
#include <unordered_map>
#include <vector>
#include "Actor.h"
#include "LightActor.h"
#include "CameraActor.h"
#include "Skybox.h"
#include "XMLAssetManager.h"

#include <Vector.h>

class Ray;
struct RayIntersectionInfo;
class ShaderComponent;
class MaterialComponent;

class OpenGLRenderer;
class VulkanRenderer;

struct Scene0 : public Scene
{
private:
	Ref<XMLAssetManager> assetManager;

	Vec4 bGColor;
	Vec4 debugColor;
	Vec4 selectionColor;
	float outlineScale;

	// For Adjusting Selected Actor
	Ref<Actor> selectedActor;
	std::string selectedActorName;

	void showSelectionSettings();
	void showTransformMenu();
	void showSceneSettings();
	void showHeirarchy();
	void showComponentMenu();
	void showAddActorMenu();
	void showMaterialMenu();
	void showLightsMenu();

public:
	bool renderMeshes = true;
	bool renderCollisionShapes = false;
	bool renderRaycasts = false;
	bool renderCubemap = true;


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

	std::unordered_map <std::string, Ref<LightActor>> lights;
	//Ref<LightActor> light;
	Ref<Skybox> skybox;

	std::vector<Ref<Ray>> rays;

	Ref<ShaderComponent> debugShader;

	// TODO for Assignment 2:
	// We'll use a ray to click on our geometry objects 
	Ref<RayIntersectionInfo> rayInfo;
};

