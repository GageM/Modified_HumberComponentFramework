#pragma once
#include "Scene.h"
#include <unordered_map>
#include <vector>
#include <mutex>
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
class ParticleComponent;

class OpenGLRenderer;
class VulkanRenderer;

enum class MovementConstraint
{
	None,
	XAxis,
	YAxis,
	ZAxis
};

struct Scene0 : public Scene
{
private:
	Ref<XMLAssetManager> assetManager;

	Vec4 bGColor;
	Vec4 debugColor;
	Vec4 selectionColor;
	float outlineScale;

	// Flags for object selection & movement
	bool isClicking;

	bool isGrabbing = false;
	bool isRotating = false;
	bool isScaling = false;

	// Enum class to dictate behaviour of grab, rotate, & scale
	MovementConstraint constraint;

	// The vector contrtolling the force of gravity
	Vec3 gravity;

	// Camera control settings
	float cameraSpeed;
	Vec3 cameraInput;
	float lookSensitivity;
	Vec3 cameraLook;

	// These will store the mouse position in 2D & 3D space
	Vec4 mouseScreenPos;
	Vec4 mouseNDCCoords;
	Vec4 mousePerspectiveCoords;
	Vec4 mouseWorldPos;

	Vec4 deltaMouseScreenPos;
	Vec4 deltaMouseWorldPos;

	// The point represented by the mouse world position + ( the normal of the camera * the distance from the camera to the selected object )
	Vec4 mouseSelectionPos;

	float selectionDistance;

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
	void showPhysicsMenu();
	void showMaterialSwapMenu();
	
	void SetActorMaterial(Ref<Actor> actor, Ref<MaterialComponent> material, std::string newMaterialName);

	void CreateNewDefaultMaterial();

	void Grab(const float deltaTime);
	void Rotate(const float deltaTime);
	void Scale(const float deltaTime);

	// For Vulkan: Variables needed before integration with main program
	Ref<TransformComponent> marioTransform;

public:
	bool renderMeshes = true;
	bool renderCollisionShapes = false;
	bool renderRaycasts = false;
	bool renderCubemap = true;
	bool enableFrustumCulling = true; // Still in develelopment!
	mutable int culledActors;


	explicit Scene0(Ref<Renderer> renderer_);
	virtual ~Scene0();

	virtual bool OnCreate();
	virtual void OnDestroy();
	virtual void Update(const float deltaTime);
	virtual void Render() const;
	virtual void HandleEvents(const SDL_Event& sdlEvent);

	virtual void HandleGUI();

	// Keep track of Actors from XML asset manager
	std::unordered_map<std::string, Ref<Actor>> actors;

	// We only have one camera and light, so they don't need to be in the map
	Ref<CameraActor> camera;

	std::unordered_map <std::string, Ref<LightActor>> lights;
	//Ref<LightActor> light;

	Ref<Skybox> skybox;

	std::vector<Ref<Ray>> rays;

	Ref<ParticleComponent> particleTest;

	Ref<ShaderComponent> debugShader;

	mutable std::mutex mtx;

	// TODO for Assignment 2:
	// We'll use a ray to click on our geometry objects 
	Ref<RayIntersectionInfo> rayInfo;
};

