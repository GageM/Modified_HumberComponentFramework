#include <glew.h>
#include <iostream>
#include <SDL.h>
#include "Debug.h"
#include "Scene0.h"
#include <MMath.h>
#include "Debug.h"
#include "TransformComponent.h"
#include "TextureComponent.h"
#include <QMath.h>
#include "ShaderComponent.h"
#include "MeshComponent.h"
#include "ShapeComponent.h"
#include "MaterialComponent.h"
#include "PhysicsComponent.h"
#include "Physics.h"
#include "ParticleComponent.h"

#include "DrawDebug.h"

#include "VulkanRenderer.h"
#include "OpenGLRenderer.h"

#include "ControllerManager.h"

#include "imgui.h"
#include "imgui_impl_opengl3.h"

Scene0::Scene0(Ref<Renderer> renderer_) :
	Scene(renderer_, false),
	assetManager(nullptr),
	bGColor(Vec4(0.0f, 0.0f, 0.0f, 1.0f)),
	debugColor(Vec4(0.0f, 0.0f, 1.0f, 1.0f)),
	selectionColor(Vec4(1.0f, 0.5f, 0.0f, 1.0f)),
	selectedActorName(""),
	outlineScale(1.05f),
	culledActors(0),
	isClicking(false),
	gravity(Vec3(0.0f, -9.81f, 0.0f)),
	mouseScreenPos(Vec4(0.0f, 0.0f, 0.0f, 0.0f)),
	mouseWorldPos(Vec4(0.0f, 0.0f, 0.0f, 0.0f)),
	marioTransform(nullptr),
	deltaMouseScreenPos(Vec4(0.0f, 0.0f, 0.0f, 0.0f)),
	deltaMouseWorldPos(Vec4(0.0f, 0.0f, 0.0f, 0.0f)),
	constraint(MovementConstraint::None),
	cameraSpeed(300.0f),
	cameraInput(Vec3()),
	lookSensitivity(50.0f),
	cameraLook(Vec3()),

	particleTest(nullptr)
{
	assetManager = std::make_shared<XMLAssetManager>(renderer);
	if (renderer->GetRendererType() == RendererType::VULKAN) cameraSpeed *= 10.0f;
	Debug::Info("Created Scene0", __FILE__, __LINE__);
}

Scene0::~Scene0()
{
	Debug::Info("Created Scene0", __FILE__, __LINE__);
}

bool Scene0::OnCreate()	
{
	switch (renderer->GetRendererType())
	{
	case RendererType::OPENGL:
	{
		// Make sure these names match the stuff in your xml file:
		std::vector<std::string> actorNames{
			"ActorGameBoard" , "ActorChecker1", "ActorChecker2",
			"ActorSkull", "ActorCube", "ActorMario", "ActorDoomKeyCard"
		};
		for (const auto& name : actorNames) {
			actors[name] = assetManager->GetComponent<Actor>(name.c_str());
		}

		camera = assetManager->GetComponent<CameraActor>("Camera1");

		// Make sure these names match the stuff in your xml file:
		std::vector<std::string> lightNames{
			"Light1"
		};
		for (const auto& name : lightNames) {
			lights[name] = assetManager->GetComponent<LightActor>(name.c_str());
		}

		skybox = assetManager->GetComponent<Skybox>("MainSkybox"); 

		debugShader = assetManager->GetComponent<ShaderComponent>("debugShader");

		// Particle Component Testing
		particleTest = std::make_shared<ParticleComponent>(nullptr, renderer, 1, 0.1f, Vec3(3.0f, 0.0f, 0.0f));
		particleTest->SetHalfExtents(Vec3(35.0f, 35.0f, 35.0f));
		particleTest->OnCreate();

		Ref<MaterialComponent> particleMat = std::make_shared<MaterialComponent>(nullptr, renderer);
		particleMat->SetShader(assetManager->GetComponent<ShaderComponent>("PBR_Shader"));
		particleMat->SetCubemap(skybox->GetCubemap());
		particleMat->baseColor = Vec4(0.3f, 0.5f, 0.7f, 1.0f);
		particleMat->roughness = 0.1f;
		particleMat->OnCreate();

		particleTest->SetMaterial(particleMat);
	}
		break;
	case RendererType::VULKAN:
	{
		camera = assetManager->GetComponent<CameraActor>("Camera1");

		// Make sure these names match the stuff in your xml file:
		std::vector<std::string> lightNames{
			"Light1"
		};
		for (const auto& name : lightNames) {
			lights[name] = assetManager->GetComponent<LightActor>(name.c_str());
		}

		marioTransform = std::make_shared<TransformComponent>(nullptr, renderer, Vec3(0.0f, 0.0f, -10.0f), Quaternion());
		break;
	}
	default:
		break;
	}

	return true;
}

void Scene0::OnDestroy()
{
	actors.clear();

	if (controller)
	{
		controller = nullptr;
	}

	if(skybox) skybox = nullptr;

	if (marioTransform) marioTransform = nullptr;

	if (particleTest) particleTest = nullptr;
}

void Scene0::HandleEvents(const SDL_Event& sdlEvent)
{
	mtx.lock();
	Ref<TransformComponent> cameraTransform = camera->GetComponent <TransformComponent>();

	// Keyboard Button Pressed
	if(sdlEvent.type == SDL_KEYDOWN)
	{
		if (!showMenu)
		{
			switch (sdlEvent.key.keysym.scancode)
			{

				// Rotate Camera Buttons
			case SDL_SCANCODE_LEFT:
			{
				//cameraTransform->SetTransform(cameraTransform->pos, cameraTransform->GetOrientation() * QMath::angleAxisRotation(-15.0f, Vec3(0.0f, 1.0f, 0.0f)));
				//camera->UpdateViewMatrix();
				cameraLook.y -= 1.0f;
				break;
			}
			case SDL_SCANCODE_RIGHT:
			{
				//cameraTransform->SetTransform(cameraTransform->pos, cameraTransform->GetOrientation() * QMath::angleAxisRotation(15.0f, Vec3(0.0f, 1.0f, 0.0f)));
				//camera->UpdateViewMatrix();
				cameraLook.y += 1.0f;
				break;
			}
			case SDL_SCANCODE_UP:
			{
				cameraLook.x -= 1.0f;
				break;
			}
			case SDL_SCANCODE_DOWN:
			{
				cameraLook.x += 1.0f;
				break;
			}

			// Raise/Lower Camera Y position
			case SDL_SCANCODE_Q:
			{
				//cameraTransform->SetTransform(cameraTransform->pos + Vec3(0.0f, 1.0f, 0.0f), cameraTransform->GetOrientation());
				//camera->UpdateViewMatrix();
				cameraInput.y += 1.0f;
				break;
			}
			case SDL_SCANCODE_E:
			{
				cameraInput.y -= 1.0f;
				//cameraTransform->SetTransform(cameraTransform->pos + Vec3(0.0f, -1.0f, 0.0f), cameraTransform->GetOrientation());
				//camera->UpdateViewMatrix();
				break;
			}

			// Move Camera on XZ Plane
			case SDL_SCANCODE_W:
			{
				cameraInput.z += 1.0f;
				//cameraTransform->SetTransform(cameraTransform->pos + QMath::rotate(Vec3(0.0f, 0.0f, 1.0f), cameraTransform->GetOrientation()), cameraTransform->GetOrientation());
				//camera->UpdateViewMatrix();
				break;
			}
			case SDL_SCANCODE_A:
			{
				cameraInput.x += 1.0f;
				//cameraTransform->SetTransform(cameraTransform->pos + QMath::rotate(Vec3(1.0f, 0.0f, 0.0f), cameraTransform->GetOrientation()), cameraTransform->GetOrientation());
				//camera->UpdateViewMatrix();
				break;
			}
			case SDL_SCANCODE_S:
			{
				cameraInput.z -= 1.0f;
				//cameraTransform->SetTransform(cameraTransform->pos + QMath::rotate(Vec3(0.0f, 0.0f, -1.0f), cameraTransform->GetOrientation()), cameraTransform->GetOrientation());
				//camera->UpdateViewMatrix();
				break;
			}
			case SDL_SCANCODE_D:
			{
				cameraInput.x -= 1.0f;
				//cameraTransform->SetTransform(cameraTransform->pos + QMath::rotate(Vec3(-1.0f, 0.0f, 0.0f), cameraTransform->GetOrientation()), cameraTransform->GetOrientation());
				//camera->UpdateViewMatrix();
				break;
			}

			// Object Manipulation
			case SDL_SCANCODE_G:
			{
				isGrabbing = !isGrabbing;
				if (!isGrabbing) constraint = MovementConstraint::None;
				break;
			}

			// Movement Constraints
			case SDL_SCANCODE_X:
			{
				if (isGrabbing || isRotating || isScaling) constraint = MovementConstraint::XAxis;
				break;
			}
			case SDL_SCANCODE_Y:
			{
				if (isGrabbing || isRotating || isScaling) constraint = MovementConstraint::YAxis;
				break;
			}
			case SDL_SCANCODE_Z:
			{
				if (isGrabbing || isRotating || isScaling) constraint = MovementConstraint::ZAxis;
				break;
			}

			// Confirm Object Manipulation
			case SDL_SCANCODE_RETURN:
			{
				isGrabbing = isRotating = isScaling = false;
				constraint = MovementConstraint::None;
				break;
			}

			default:
				break;
			}
		}
	}

	else if (sdlEvent.type == SDL_KEYUP)
	{
		if (!showMenu)
		{
			switch (sdlEvent.key.keysym.scancode)
			{
			case SDL_SCANCODE_LEFT:
			case SDL_SCANCODE_RIGHT:
				cameraLook.y = 0.0f;
				break;
			
			case SDL_SCANCODE_UP:
			case SDL_SCANCODE_DOWN:
				cameraLook.x = 0.0f;
				break;


				// Stop accelerating camera
			case SDL_SCANCODE_Q:
			case SDL_SCANCODE_E:
			case SDL_SCANCODE_W:
			case SDL_SCANCODE_A:
			case SDL_SCANCODE_S:
			case SDL_SCANCODE_D:
			{
				cameraInput = Vec3();
				break;
			}


			default:
				break;
			}
		}
	}
	// Mouse Movement
	else if (sdlEvent.type == SDL_MOUSEMOTION)
	{
		// Capture last mouse position;
		deltaMouseScreenPos = mouseScreenPos;
		deltaMouseWorldPos = mouseWorldPos;

		mouseScreenPos = Vec4(static_cast<float>(sdlEvent.button.x), static_cast<float>(sdlEvent.button.y), 0.0f, 1.0f);

		// Transform mouse pos into world pos
		Matrix4 ndcToPixel = MMath::viewportNDC(1280, 720);
		mouseNDCCoords = MMath::inverse(ndcToPixel) * mouseScreenPos;
		Matrix4 perspectiveToNDC = camera->GetProjectionMatrix();
		mousePerspectiveCoords = MMath::inverse(perspectiveToNDC) * mouseNDCCoords;
		mousePerspectiveCoords /= mousePerspectiveCoords.w;
		Matrix4 worldToPerspective = camera->GetViewMatrix();
		mouseWorldPos = MMath::inverse(worldToPerspective) * mousePerspectiveCoords;

		// Get the change in mouse position
		deltaMouseScreenPos = mouseScreenPos - deltaMouseScreenPos;
		deltaMouseWorldPos = mouseWorldPos - deltaMouseWorldPos;
	}
		
	// Mouse Button Pressed
	else if (sdlEvent.type == SDL_MOUSEBUTTONDOWN)
	{
		switch (sdlEvent.button.button)
		{
			// Left Mouse Button
		case SDL_BUTTON_LEFT:
			// Check renderer
			switch (renderer->GetRendererType())
			{
			case RendererType::OPENGL:
			{
				if (!showMenu) {
					// TODO for Assignment 2: 
					// Get a ray pointing into the world, We have the x, y pixel coordinates
					// Need to convert this into world space to build our ray
					printf("Mouse Click: \n");

					//Arbitrary max distance for selection
					float distance = 1000.0f;

					// Create a ray from the camera
					Vec3 rayStart = -cameraTransform->pos;
					Vec3 rayDir = VMath::normalize(mouseWorldPos - rayStart);

					Ref<Ray> drawRay = std::make_shared<Ray>(rayStart, rayDir, distance);
					rays.push_back(drawRay);

					// Loop through all the actors and check if the ray has collided with them
					// Pick the one with the smallest positive t value
					for (auto it = actors.begin(); it != actors.end(); ++it) {
						Ref<Actor> actor = std::dynamic_pointer_cast<Actor>(it->second);
						Ref<TransformComponent> transformComponent = actor->GetComponent <TransformComponent>();
						Ref<ShapeComponent> shapeComponent = actor->GetComponent <ShapeComponent>();
						// TODO for Assignment 2: 
						// Transform the ray into the local space of the object and check if a collision occured
						Matrix4 worldToLocalSpace = MMath::inverse(actor->GetModelMatrix());
						Vec4 localSpaceRayStart = worldToLocalSpace * Vec4(rayStart, 1.0f);
						Vec4 localSpaceRayDir = worldToLocalSpace * Vec4(rayDir, 0.0f);
						Ray localSpaceRay(localSpaceRayStart, localSpaceRayDir, distance);

						rayInfo = std::make_shared<RayIntersectionInfo>(shapeComponent->shape->rayIntersectionInfo(localSpaceRay));
						// Pick the closest object to the camera
						if (rayInfo->isIntersected && rayInfo->t < distance)
						{
							distance = rayInfo->t;
							std::cout << "Picked: " << it->first << ", Distance: " << distance << "\n";
							selectedActorName = it->first;
							selectedActor = actor;
						}
					}
				}
				break;
			}
			default:
				break;
			}
			break;

			// Right Mouse Button
		case SDL_BUTTON_RIGHT:
			if (selectedActor)
			{
				Vec3 rayStart = -cameraTransform->pos;
				Vec3 rayDir = VMath::normalize(mouseWorldPos - rayStart);

				Matrix4 worldToLocalSpace = MMath::inverse(selectedActor->GetModelMatrix());
				Vec4 localSpaceRayStart = worldToLocalSpace * Vec4(rayStart, 1.0f);
				Vec4 localSpaceRayDir = worldToLocalSpace * Vec4(rayDir, 0.0f);
				Ray localSpaceRay(localSpaceRayStart, localSpaceRayDir);

				rayInfo = std::make_shared<RayIntersectionInfo>(selectedActor->GetComponent<ShapeComponent>()->shape->rayIntersectionInfo(localSpaceRay));

				if (rayInfo->isIntersected)
				{
					mouseSelectionPos = selectedActor->GetModelMatrix() * rayInfo->intersectionPoint;
					isClicking = true;
				}


			}


			break;
		default:
			break;
		}
	}

	// Mouse Button Released
	else if (sdlEvent.type == SDL_MOUSEBUTTONUP)
	{
		switch (sdlEvent.button.button)
		{
		case SDL_BUTTON_LEFT:
			break;
		case SDL_BUTTON_RIGHT:
			if (!showMenu) isClicking = false;
			break;
		default:
			break;
		}
	}

	// Controller Button Pressed
	else if (sdlEvent.type == SDL_CONTROLLERBUTTONDOWN)
	{
		switch (sdlEvent.cbutton.button)
		{
		case SDL_GameControllerButton::SDL_CONTROLLER_BUTTON_A:
			printf("A Button Pressed\n");
			break;

		case SDL_GameControllerButton::SDL_CONTROLLER_BUTTON_B:
			printf("B Button Pressed\n");
			break;

		case SDL_GameControllerButton::SDL_CONTROLLER_BUTTON_X:
			printf("X Button Pressed\n");
			break;

		case SDL_GameControllerButton::SDL_CONTROLLER_BUTTON_Y:
			printf("Y Button Pressed\n");
			break;
		case SDL_GameControllerButton::SDL_CONTROLLER_BUTTON_RIGHTSHOULDER:
			cameraInput.y -= 1.0f;
			printf("Right Shoulder Pressed\n");
			break;
		case SDL_GameControllerButton::SDL_CONTROLLER_BUTTON_LEFTSHOULDER:
			cameraInput.y += 1.0f;
			printf("Left Shoulder Pressed\n");
			break;
		default:
			break;
		}
	}

	else if (sdlEvent.type == SDL_CONTROLLERBUTTONUP)
	{
		switch (sdlEvent.cbutton.button)
		{
		case SDL_GameControllerButton::SDL_CONTROLLER_BUTTON_A:
			printf("A Button Released\n");
			break;

		case SDL_GameControllerButton::SDL_CONTROLLER_BUTTON_B:
			printf("B Button Released\n");
			break;

		case SDL_GameControllerButton::SDL_CONTROLLER_BUTTON_X:
			printf("X Button Released\n");
			break;

		case SDL_GameControllerButton::SDL_CONTROLLER_BUTTON_Y:
			printf("Y Button Released\n");
			break;
		case SDL_GameControllerButton::SDL_CONTROLLER_BUTTON_RIGHTSHOULDER:
			cameraInput.y = 0.0f;
			printf("Right Shoulder Released\n");
			break;
		case SDL_GameControllerButton::SDL_CONTROLLER_BUTTON_LEFTSHOULDER:
			cameraInput.y = 0.0f;
			printf("Left Shoulder Released\n");
			break;
		default:
			break;
	}
	}

	// Controller Stick Movement
	else if (sdlEvent.type == SDL_CONTROLLERAXISMOTION)
	{
		switch (sdlEvent.caxis.axis)
		{
		case SDL_GameControllerAxis::SDL_CONTROLLER_AXIS_LEFTX:
			cameraInput.x = -(float)sdlEvent.caxis.value / 32767.0f;
			if (abs(cameraInput.x) < 0.05f) cameraInput.x = 0.0f; // Create dead zone
			break;
		case SDL_GameControllerAxis::SDL_CONTROLLER_AXIS_LEFTY:
			cameraInput.z = -(float)sdlEvent.caxis.value / 32767.0f;
			if (abs(cameraInput.z) < 0.05f) cameraInput.z = 0.0f;  // Create dead zone
			break;
		case SDL_GameControllerAxis::SDL_CONTROLLER_AXIS_RIGHTX:
			cameraLook.y = (float)sdlEvent.caxis.value / 32767.0f;
			if (abs(cameraLook.y) < 0.05f) cameraLook.y = 0.0f;  // Create dead zone
			break;
		case SDL_GameControllerAxis::SDL_CONTROLLER_AXIS_RIGHTY:
			cameraLook.x = (float)sdlEvent.caxis.value / 32767.0f;
			if (abs(cameraLook.x) < 0.05f) cameraLook.x = 0.0f;  // Create dead zone
			break;
		default:
			break;
		}
	}

	mtx.unlock();
}

void Scene0::Update(const float deltaTime)
{
	mtx.lock();

	if (renderer->GetRendererType() == RendererType::OPENGL)
	{
		// Clear drawn rays after they age out
		for (unsigned int i = 0; i < rays.size(); i++)
		{
			rays[i]->age += deltaTime;
			if (rays[i]->age >= rays[i]->maxAge)
			{
				rays.erase(rays.begin() + i);
			}
		}

		// Solve actor physics
		if (selectedActor)
		{
			// Object physics on selected actor
			if (isClicking)
			{
				Ref<PhysicsComponent> body = selectedActor->GetComponent<PhysicsComponent>();
				if (body)
				{
					//Vec3 rayStart = -camera->GetComponent<TransformComponent>()->pos;
					//Vec3 rayDir = VMath::normalize(mouseWorldPos - rayStart);
					//
					//mouseSelectionPos = rayStart + rayDir * selectionDistance;

					float dragCoeff = 0.25f;
					Vec3 dragForce = body->vel * (-dragCoeff);
					Vec3 netForce = gravity + dragForce;

					PHYSICS::ApplyForce(body, netForce);
					PHYSICS::UpdateVel(body, deltaTime);
					PHYSICS::MouseConstraint(body, deltaTime, Vec3(mouseSelectionPos.x, mouseSelectionPos.y, mouseSelectionPos.z));


					PHYSICS::UpdatePos(body, deltaTime);
					//PHYSICS::UpdateOrientation(body, deltaTime);
					PHYSICS::UpdateTransform(selectedActor);

					//selectedActor->GetComponent<TransformComponent>()->pos = Vec3(mouseSelectionPos.x, mouseSelectionPos.y, mouseSelectionPos.z);
				}
			}

			// Move selected actor in viewport
			if (isGrabbing)
			{
				Grab(deltaTime);
			}

			// Rotate selected actor in viewport
			if (isRotating)
			{

			}

			// Scale selected actor in viewport
			if (isScaling)
			{

			}
		}

		// Update & Run particle sim
		//particleTest->Update(deltaTime);
		//particleTest->Simulate(gravity, deltaTime);
	}

	// Update vulkan meshes to spin
	else if (renderer->GetRendererType() == RendererType::VULKAN)
	{
		marioTransform->orientation = QMath::angleAxisRotation(90.0f * deltaTime, Vec3::up()) * marioTransform->orientation;
	}

	// Solve camera movement
	Ref<PhysicsComponent> cameraPhysics = camera->GetComponent<PhysicsComponent>();
	Ref<TransformComponent> cameraTransform = camera->GetComponent<TransformComponent>();

	if (VMath::mag(cameraInput) > 0.1f)
	{
		cameraInput = VMath::normalize(cameraInput);
	}
	cameraInput *= cameraSpeed;

	cameraPhysics->vel = (cameraTransform->forward() * cameraInput.z + cameraTransform->right() * cameraInput.x + cameraTransform->up() * cameraInput.y) * deltaTime;
	PHYSICS::UpdatePos(cameraPhysics, deltaTime);
	cameraTransform->pos = cameraPhysics->pos;

	if (abs(cameraLook.y) > 0.3f) cameraTransform->orientation = QMath::angleAxisRotation(lookSensitivity * deltaTime, Vec3::up() * cameraLook.y) * cameraTransform->GetOrientation();
	if (abs(cameraLook.x) > 0.3f) cameraTransform->orientation = QMath::angleAxisRotation(lookSensitivity * deltaTime, cameraTransform->right() * cameraLook.x) * cameraTransform->GetOrientation();


	//PHYSICS::UpdateTransform(camera); Can't use because it also affects orientation
	camera->UpdateViewMatrix();

	mtx.unlock();
}

void Scene0::Render() const
{
	mtx.lock();
	switch (renderer->GetRendererType()) {

	case RendererType::OPENGL:
	{
		glEnable(GL_DEPTH_TEST);
		glEnable(GL_STENCIL_TEST);
		glStencilOp(GL_KEEP, GL_KEEP, GL_REPLACE);
		glClearColor(bGColor.x, bGColor.y, bGColor.z, bGColor.w);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);
		glBindBuffer(GL_UNIFORM_BUFFER, camera->GetMatricesID());
		for (auto& it : lights)
		{
			Ref<LightActor> light = it.second;
			glBindBuffer(GL_UNIFORM_BUFFER, light->GetLightID());
		}

		// Let it go
		glBindTexture(GL_TEXTURE_2D, 0);

		if (renderCubemap) skybox->Render();

		glStencilFunc(GL_ALWAYS, 1, 0xFF);
		glStencilMask(0xFF);

		culledActors = 0;

		// Draw unselected actors		
		for (auto it = actors.begin(); it != actors.end(); ++it) {
			Ref<Actor> actor = std::dynamic_pointer_cast<Actor>(it->second);

			if (actor != selectedActor)
			{
				// Check if the actor is in the frustrum || if frustrum culling is disabled
				if (camera->CheckFrustrum(actor->GetComponent<TransformComponent>()->pos) || !enableFrustrumCulling)
				{
					// Draw actor mesh
					glUseProgram(actor->GetComponent<MaterialComponent>()->GetShader()->GetProgram());
					glUniformMatrix4fv(actor->GetComponent<MaterialComponent>()->GetShader()->GetUniformID("modelMatrix"), 1, GL_FALSE, actor->GetModelMatrix());

					actor->GetComponent<MaterialComponent>()->Render();
					if (renderMeshes) {
						actor->GetComponent<MeshComponent>()->Render(GL_TRIANGLES);
					}
					actor->GetComponent<MaterialComponent>()->PostRender();

					// Draw actor collider
					glUseProgram(debugShader->GetProgram());
					if (renderCollisionShapes) {
						// Drawing the primitive geometry associated with the mesh to help debug ray intersects, culling, and collision detection
						glUniformMatrix4fv(debugShader->GetUniformID("modelMatrix"), 1, GL_FALSE, actor->GetModelMatrix());
						glUniform4fv(debugShader->GetUniformID("debugColor"), 1, debugColor);
						actor->GetComponent<ShapeComponent>()->Render();
					}
				}
				else
				{
					culledActors++;
				}

			}
		}

		// Draw selection
		if (selectedActor)
		{

			// Draw Outline Around Selected actor
			{
				glStencilFunc(GL_NOTEQUAL, 1, 0xFF);
				glStencilMask(0x00);
				glDisable(GL_DEPTH_TEST);

				Ref<TransformComponent> transform = selectedActor->GetComponent<TransformComponent>();
				// Scale outline by amount
				if (outlineScale > 0.0f) {

					transform->SetScale(transform->scale * outlineScale);
				}

				glUseProgram(debugShader->GetProgram());
				glUniformMatrix4fv(debugShader->GetUniformID("modelMatrix"), 1, GL_FALSE, selectedActor->GetModelMatrix());
				glUniform4fv(debugShader->GetUniformID("debugColor"), 1, selectionColor);
				selectedActor->GetComponent<MeshComponent>()->Render();

				// Unscale model
				if (outlineScale > 0.0f) {
					transform->SetScale(transform->scale / outlineScale);
				}

				glStencilMask(0xFF);
				glStencilFunc(GL_ALWAYS, 1, 0xFF);
				glEnable(GL_DEPTH_TEST);
			}


			// Draw selected actor
			{
				glStencilFunc(GL_ALWAYS, 1, 0xFF);
				glStencilMask(0xFF);


				// Draw actor mesh
				glUseProgram(selectedActor->GetComponent<MaterialComponent>()->GetShader()->GetProgram());
				glUniformMatrix4fv(selectedActor->GetComponent<MaterialComponent>()->GetShader()->GetUniformID("modelMatrix"), 1, GL_FALSE, selectedActor->GetModelMatrix());

				selectedActor->GetComponent<MaterialComponent>()->Render();
				if (renderMeshes) {
					selectedActor->GetComponent<MeshComponent>()->Render(GL_TRIANGLES);
				}
				selectedActor->GetComponent<MaterialComponent>()->PostRender();

				//glStencilMask(0xFF);
				glUseProgram(debugShader->GetProgram());
				if (renderCollisionShapes) {
					// Drawing the primitive geometry associated with the mesh to help debug ray intersects, culling, and collision detection
					glUniformMatrix4fv(debugShader->GetUniformID("modelMatrix"), 1, GL_FALSE, selectedActor->GetModelMatrix());
					glUniform4fv(debugShader->GetUniformID("debugColor"), 1, debugColor);
					selectedActor->GetComponent<ShapeComponent>()->Render();
				}
			}
		}

		// Draw raycasts
		if (renderRaycasts)
		{
			glUseProgram(debugShader->GetProgram());

			for (Ref<Ray> ray : rays)
			{
				glUniformMatrix4fv(debugShader->GetUniformID("modelMatrix"), 1, GL_FALSE, Matrix4());
				glUniform4fv(debugShader->GetUniformID("debugColor"), 1, debugColor);
				ray->DrawRay();
			}
		}

		// Draw debug primitives
		{
			glUseProgram(debugShader->GetProgram());
			
			glUniformMatrix4fv(debugShader->GetUniformID("modelMatrix"), 1, GL_FALSE, Matrix4());
			glUniform4fv(debugShader->GetUniformID("debugColor"), 1, debugColor);

			if (selectedActor && isClicking)
			{
				DrawDebug::DrawLine(mouseSelectionPos, selectedActor->GetComponent<TransformComponent>()->pos);
			}
			
			//Vec3 p(0.0f, 0.0f, -10.0f);
			//Quaternion q = QMath::angleAxisRotation(45.0f, Vec3(1.0f, 0.0f, 0.0f)) * Quaternion();
			
			//DrawDebug::DrawCircle(Vec3(-2.0f, 0.0f, -10.0f), q, 1.0f, 10);
			//DrawDebug::DrawRectangle(p, q, Vec2(1.0f, 1.0f));			
			//DrawDebug::DrawBox(Vec3(2.0f, 0.0f, -10.0f), q, Vec3(1.0f, 1.0f, 1.0f));
			//DrawDebug::DrawSphere(Vec3(4.0f, 0.0f, -10.0f), q, 1.0f, 12);
			//DrawDebug::DrawCylinder(Vec3(0.0f, -1.0f, -10.0f), Vec3(0.0f, 1.0f, -10.0f), q, 0.5f, 12);
		
		}

		// Draw particle system
		particleTest->Render();

		break;
	}
	case RendererType::VULKAN:
	{
		Ref<VulkanRenderer> vRenderer = std::dynamic_pointer_cast<VulkanRenderer>(renderer);
		vRenderer->SetCameraUBO(camera->GetProjectionMatrix(), camera->GetViewMatrix());
		int counter = 0;
		for (const auto& it : lights)
		{
			//Ref<LightActor> light = it.second;
			//vRenderer->SetGLightsUBO(counter, light->position, light->colour, light->colour, light->colour);
			//counter++;
		}
		vRenderer->SetGLightsUBO(0, Vec3(100.0f, 10.0f, 100.0f), Vec4(1.0f, 1.0f, 1.0f, 1.0f), Vec4(1.0f, 1.0f, 1.0f, 1.0f), Vec4(1.0f, 1.0f, 1.0f, 1.0f));
		counter = 0;

		vRenderer->SetMeshPushConstants(marioTransform->GetTransformMatrix());

		vRenderer->Render();
		break;
	}
	default:
		break;

	}
	mtx.unlock();
}

void Scene0::HandleGUI()
{
	// Hide menu when interacting with scene
	if (showMenu)
	{
		switch (renderer->GetRendererType())
		{
		case RendererType::OPENGL:

			showSceneSettings();
			showHeirarchy();
			
			if (ImGui::TreeNode("Add Actor"))
			{
				//showAddActorMenu();
				ImGui::TreePop();
			}
			
			// If there is an actor selected show the dropdown
			if (selectedActor && ImGui::TreeNode(selectedActorName.c_str()))
			{
				showSelectionSettings();
				ImGui::TreePop();
			}
			
			break;
		default:
			break;
		}
	}
}

void Scene0::showSceneSettings()
{
	if (ImGui::TreeNode("Colors"))
	{
		ImGui::ColorEdit4("Background Color", bGColor);
		ImGui::ColorEdit4("Debug Color", debugColor);
		ImGui::ColorEdit4("Selection Color", selectionColor);
		ImGui::TreePop();
	}

	if (ImGui::TreeNode("Rendering"))
	{
		ImGui::DragFloat("Selection Outline Scale", &outlineScale, 0.1f, 0.01f, 2.0f, "%.2f");
		ImGui::Checkbox("Render Meshes", &renderMeshes);
		ImGui::Checkbox("Render Colliders", &renderCollisionShapes);
		ImGui::Checkbox("Render Raycasts", &renderRaycasts);
		ImGui::Checkbox("Render Cubemap", &renderCubemap);
		ImGui::Checkbox("Enable Frustrum Culling", &enableFrustrumCulling);
		if (enableFrustrumCulling)
		{
			ImGui::Text("Culled Actors: %d", culledActors);
		}
		ImGui::TreePop();
	}

	if (ImGui::TreeNode("Lights"))
	{
		showLightsMenu();
		ImGui::TreePop();
	}

	if (ImGui::TreeNode("Physics"))
	{
		showPhysicsMenu();
		ImGui::TreePop();
	}
}

void Scene0::showSelectionSettings()
{
	showComponentMenu();
	showTransformMenu();
	showMaterialMenu();
}

void Scene0::showTransformMenu()
{
	Ref<TransformComponent> transform = selectedActor->GetComponent<TransformComponent>();
	if (!transform)
	{
		return;
	}

	if (ImGui::TreeNode("Transform"))
	{
		if (ImGui::TreeNode("Position")) {
				ImGui::DragFloat("X", &transform->pos.x, 0.1f, -100.0f, 100.0f);
				ImGui::DragFloat("Y", &transform->pos.y, 0.1f, -100.0f, 100.0f);
				ImGui::DragFloat("Z", &transform->pos.z, 0.1f, -100.0f, 100.0f);
				ImGui::TreePop();
			}

		if (ImGui::TreeNode("Rotation")) {
				ImGui::DragFloat("X", &transform->rotation.xAxis, 0.1f, -360.0f, 360.0f, "%.3f Degrees");
				ImGui::DragFloat("Y", &transform->rotation.yAxis, 0.1f, -360.0f, 360.0f, "%.3f Degrees");
				ImGui::DragFloat("Z", &transform->rotation.zAxis, 0.1f, -360.0f, 360.0f, "%.3f Degrees");
				transform->UpdateOrientation();
				ImGui::TreePop();
			}

		if (ImGui::TreeNode("Scale")) {
				ImGui::DragFloat("X", &transform->scale.x, 0.1f, -100.0f, 100.0f);
				ImGui::DragFloat("Y", &transform->scale.y, 0.1f, -100.0f, 100.0f);
				ImGui::DragFloat("Z", &transform->scale.z, 0.1f, -100.0f, 100.0f);
				ImGui::TreePop();
			}

		ImGui::TreePop();
	}
}

void Scene0::showHeirarchy()
{
	if (ImGui::TreeNode("Heirarchy"))
	{
		// List all the actors
		for (auto& it : actors)
		{
			Ref<Actor> actor = std::dynamic_pointer_cast<Actor>(it.second);

			// List the components in the actor
			if (ImGui::TreeNode(it.first.c_str()))
			{
				for (auto& component : actor->GetComponentList())
				{
					ImGui::Text(typeid(*component.second.get()).name());
				}
				ImGui::TreePop();
			}
		}

		ImGui::TreePop();
	}
}

void Scene0::showComponentMenu()
{
	// TODO:: Allow adding components to actor
	if (ImGui::TreeNode("Add Component"))
	{
		for (const auto& component : assetManager->xmlAssets)
		{
			ImGui::Text("%s", component.first.c_str());
		}
		ImGui::TreePop();
	}

	// TODO:: Allow removal of components from actor
	if (ImGui::TreeNode("Remove Component"))
	{
		ImGui::TreePop();
	}
}

void Scene0::showAddActorMenu()
{
	// Print out all usable components
	if (ImGui::TreeNode("Meshes"))
	{
		for (const auto& pair : assetManager->GetAllComponentsOfType<MeshComponent>())
		{
			ImGui::Text("%s", pair.first.c_str());
		}
		ImGui::TreePop();
	}


	if (ImGui::TreeNode("Textures"))
	{
		for (const auto& pair : assetManager->GetAllComponentsOfType<TextureComponent>())
		{
			ImGui::Text("%s", pair.first.c_str());
		}
		ImGui::TreePop();
	}

}

void Scene0::showMaterialMenu()
{
	Ref<MaterialComponent> material = selectedActor->GetComponent<MaterialComponent>();
	if (!material) return;

	showMaterialSwapMenu();

	if (ImGui::Button("Add New Material"))
	{
		CreateNewDefaultMaterial();
	}

	if (ImGui::TreeNode("Material Properties"))
	{
		ImGui::ColorEdit4("Base Color", material->baseColor);
		ImGui::DragFloat("Roughness", &material->roughness, 0.01f, 0.0f, 1.0f);
		ImGui::DragFloat("Metallic", &material->metallic, 0.01f, 0.0f, 1.0f);

		if (material->GetBaseColorMap())
		{
			ImGui::Checkbox("Enable Base Color Map", &material->useBaseColorMap);
		}

		if (material->GetRoughnessMap())
		{
			ImGui::Checkbox("Enable Roughness Map", &material->useRoughnessMap);
		}

		if (material->GetMetallicMap())
		{
			ImGui::Checkbox("Enable Metallic Map", &material->useMetallicMap);
		}

		ImGui::TreePop();
	}
}

void Scene0::showLightsMenu()
{
	for (auto& it : lights)
	{
		Ref<LightActor> light = it.second;
		if (ImGui::TreeNode(it.first.c_str()))
		{
			ImGui::ColorEdit4("Color", light->colour);

			if (ImGui::TreeNode("Position")) 
			{
				ImGui::DragFloat("X", &light->position.x, 0.1f, -100.0f, 100.0f);
				ImGui::DragFloat("Y", &light->position.y, 0.1f, -100.0f, 100.0f);
				ImGui::DragFloat("Z", &light->position.z, 0.1f, -100.0f, 100.0f);
				ImGui::TreePop();
			}

			light->UpdateLightData();

			ImGui::TreePop();
		}
	}
}

void Scene0::showPhysicsMenu()
{
	if (ImGui::TreeNode("Gravity"))
	{
		ImGui::DragFloat("X", &gravity.x, 0.1f, -100.0f, 100.0f);
		ImGui::DragFloat("Y", &gravity.y, 0.1f, -100.0f, 100.0f);
		ImGui::DragFloat("Z", &gravity.z, 0.1f, -100.0f, 100.0f);
		ImGui::TreePop();
	}
}

void Scene0::showMaterialSwapMenu()
{
	// Get a list of all the material names
	std::vector<std::string> names;
	for (const auto& pair : assetManager->GetAllComponentsOfType<MaterialComponent>())
	{
		names.push_back(pair.first);
	}

	static int item_current_idx = -1;

	std::unordered_map <std::string, Ref<MaterialComponent>> materialList = selectedActor->GetAllComponentsOfType<MaterialComponent>();
	std::pair <std::string, Ref<MaterialComponent>> pair = std::pair <std::string, Ref<MaterialComponent>>(materialList.begin()->first, materialList.begin()->second);

	int materialSlot = 0;

	for (const auto& pair : materialList)
	{
		std::string slotName = "Material Slot "; slotName.append(std::to_string(materialSlot));
		if (ImGui::BeginCombo(slotName.c_str(), pair.first.c_str()))
		{
			for (int i = 0; i < names.size(); i++)
			{
				const bool is_selected = (item_current_idx == i);
				if (ImGui::Selectable(names[i].c_str(), is_selected))
				{
					item_current_idx = i;
					SetActorMaterial(selectedActor, assetManager->GetComponent<MaterialComponent>(names[i].c_str()), names[i]);
				}

				// Set the initial focus when opening the combo (scrolling + keyboard navigation focus)
				if (is_selected)
				{
					ImGui::SetItemDefaultFocus();
				}
			}
			ImGui::EndCombo();
		}
		materialSlot++;
	}

}

void Scene0::SetActorMaterial(Ref<Actor> actor, Ref<MaterialComponent> material, std::string newMaterialName)
{
	actor->RemoveComponent<MaterialComponent>();
	actor->AddComponent<MaterialComponent>(newMaterialName, material);
}

void Scene0::CreateNewDefaultMaterial()
{
	// Create a new default material
	Ref<MaterialComponent> material = std::make_shared<MaterialComponent>(nullptr, renderer);
	material->SetCubemap(skybox->GetCubemap());
	material->SetShader(assetManager->GetComponent<ShaderComponent>("PBR_Shader"));
	material->OnCreate();

	// Create a default name for the new material
	std::string materialName = "Material ";
	materialName.append(std::to_string(assetManager->GetAllComponentsOfType<MaterialComponent>().size()));

	// Add the new material to the asset manager
	assetManager->AddComponent(materialName.c_str(), material);
}

void Scene0::Grab(const float deltaTime)
{
	if (VMath::mag(Vec3(deltaMouseScreenPos.x, deltaMouseScreenPos.y, deltaMouseScreenPos.z)) > 2.0f)
	{
		switch (constraint)
		{
		case MovementConstraint::None:
			// Move object along view plane
			break;
		case MovementConstraint::XAxis:
			// Move object on X axis
			selectedActor->GetComponent<TransformComponent>()->pos.x += deltaMouseScreenPos.x * deltaTime;
			break;
		case MovementConstraint::YAxis:
			// Move object on Y axis
			selectedActor->GetComponent<TransformComponent>()->pos.y += deltaMouseScreenPos.y * deltaTime;
			break;
		case MovementConstraint::ZAxis:
			// Move object on Z axis
			selectedActor->GetComponent<TransformComponent>()->pos.z += deltaMouseScreenPos.y * deltaTime;
			break;
		default:
			break;
		}
	}
}

void Scene0::Rotate(const float deltaTime)
{
	switch (constraint)
	{
	case MovementConstraint::None:
		// Rotate object along view plane
		break;
	case MovementConstraint::XAxis:
		// Rotate object on X axis
		break;
	case MovementConstraint::YAxis:
		// Rotate object on Y axis
		break;
	case MovementConstraint::ZAxis:
		// Rotate object on Z axis
		break;
	default:
		break;
	}
}

void Scene0::Scale(const float deltaTime)
{
	switch (constraint)
	{
	case MovementConstraint::None:
		// Scale object uniformly
		break;
	case MovementConstraint::XAxis:
		// Scale object on X axis
		break;
	case MovementConstraint::YAxis:
		// Scale object on Y axis
		break;
	case MovementConstraint::ZAxis:
		// Scale object on Z axis
		break;
	default:
		break;
	}
}

