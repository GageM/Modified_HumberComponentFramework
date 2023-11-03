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

#include "ControllerManager.h"

#include "imgui.h"
#include "imgui_impl_opengl3.h"

Scene0::Scene0(Ref<Renderer> renderer_) : Scene(renderer_, false), bGColor(Vec4(0.0f, 0.0f, 0.0f, 1.0f)), debugColor(Vec4(1.0f, 0.0f, 0.0f, 1.0f)),
	selectionColor(Vec4(1.0f, 0.5f, 0.0f, 1.0f)), selectedActorName(""), outlineScale(1.05f)
{
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
		std::vector<std::string> names{
			"ActorGameBoard" , "ActorChecker1", "ActorChecker2",
			"ActorSkull", "ActorCube", "ActorMario"
		};
		for (const auto& name : names) {
			auto asset = assetManager.xmlAssets.find(name);
			actors[name] = asset->second;
		}

		camera = assetManager.GetComponent<CameraActor>("Camera1");
		light = assetManager.GetComponent<LightActor>("Light1");

		skybox = std::make_shared<Skybox>(camera);
		skybox->OnCreate();

		PBR_Mat = std::make_shared<MaterialComponent>(nullptr, Vec4(1.0f, 0.0f, 0.0f, 1.0f), 0.0f, 0.0f);
		PBR_Mat->SetShader(assetManager.GetComponent<ShaderComponent>("PBR_Shader"));
		PBR_Mat->SetCubemap(skybox->GetCubemap());

		debugShader = assetManager.GetComponent<ShaderComponent>("debugShader");
	}
		break;
	case RendererType::VULKAN:
		break;
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
}

void Scene0::HandleEvents(const SDL_Event& sdlEvent)
{
	std::mutex mtx;
	mtx.lock();

	Ref<TransformComponent> cameraTransform = camera->GetComponent <TransformComponent>();
	switch (sdlEvent.type) {
	case SDL_KEYDOWN:
		if (!showMenu)
		{
			if (sdlEvent.key.keysym.scancode == SDL_SCANCODE_LEFT && !showMenu) {
				cameraTransform->SetTransform(cameraTransform->pos, cameraTransform->GetOrientation() * QMath::angleAxisRotation(-2.0f, Vec3(0.0f, 1.0f, 0.0f)));
				camera->UpdateViewMatrix();

			}
			else if (sdlEvent.key.keysym.scancode == SDL_SCANCODE_RIGHT && !showMenu) {
				cameraTransform->SetTransform(cameraTransform->pos, cameraTransform->GetOrientation() * QMath::angleAxisRotation(2.0f, Vec3(0.0f, 1.0f, 0.0f)));
				camera->UpdateViewMatrix();
			}
			else if (sdlEvent.key.keysym.scancode == SDL_SCANCODE_UP && !showMenu) {
				//cameraTransform->SetTransform(cameraTransform->pos, cameraTransform->GetOrientation() * QMath::angleAxisRotation(-2.0f, Vec3(1.0f, 0.0f, 0.0f)));
				//camera->UpdateViewMatrix();
			}
			else if (sdlEvent.key.keysym.scancode == SDL_SCANCODE_DOWN && !showMenu) {
				//cameraTransform->SetTransform(cameraTransform->pos, cameraTransform->GetOrientation() * QMath::angleAxisRotation(2.0f, Vec3(1.0f, 0.0f, 0.0f)));
				//camera->UpdateViewMatrix();
			}
			else if (sdlEvent.key.keysym.scancode == SDL_SCANCODE_Q && !showMenu)
			{
				cameraTransform->SetTransform(cameraTransform->pos + Vec3(0.0f, 1.0f, 0.0f), cameraTransform->GetOrientation());
				camera->UpdateViewMatrix();
				//camera->GetComponent<TransformComponent>()->pos.print("Camera Position: ");
			}
			else if (sdlEvent.key.keysym.scancode == SDL_SCANCODE_E && !showMenu)
			{
				cameraTransform->SetTransform(cameraTransform->pos + Vec3(0.0f, -1.0f, 0.0f), cameraTransform->GetOrientation());
				camera->UpdateViewMatrix();
				//camera->GetComponent<TransformComponent>()->pos.print("Camera Position: ");
			}
			else if (sdlEvent.key.keysym.scancode == SDL_SCANCODE_D && !showMenu) {
				cameraTransform->SetTransform(cameraTransform->pos + QMath::rotate(Vec3(-1.0f, 0.0f, 0.0f), cameraTransform->GetOrientation()), cameraTransform->GetOrientation());
				camera->UpdateViewMatrix();
				//camera->GetComponent<TransformComponent>()->pos.print("Camera Position: ");
			}
			else if (sdlEvent.key.keysym.scancode == SDL_SCANCODE_A && !showMenu) {
				cameraTransform->SetTransform(cameraTransform->pos + QMath::rotate(Vec3(1.0f, 0.0f, 0.0f), cameraTransform->GetOrientation()), cameraTransform->GetOrientation());
				camera->UpdateViewMatrix();
				//camera->GetComponent<TransformComponent>()->pos.print("Camera Position: ");

			}
			else if (sdlEvent.key.keysym.scancode == SDL_SCANCODE_W && !showMenu) {
				cameraTransform->SetTransform(cameraTransform->pos + QMath::rotate(Vec3(0.0f, 0.0f, 1.0f), cameraTransform->GetOrientation()), cameraTransform->GetOrientation());
				camera->UpdateViewMatrix();
				//camera->GetComponent<TransformComponent>()->pos.print("Camera Position: ");

			}
			else if (sdlEvent.key.keysym.scancode == SDL_SCANCODE_S && !showMenu) {
				cameraTransform->SetTransform(cameraTransform->pos + QMath::rotate(Vec3(0.0f, 0.0f, -1.0f), cameraTransform->GetOrientation()), cameraTransform->GetOrientation());
				camera->UpdateViewMatrix();
				//camera->GetComponent<TransformComponent>()->pos.print("Camera Position: ");

			}
		}

		break;
		
	case SDL_MOUSEBUTTONDOWN:
		if (sdlEvent.button.button == SDL_BUTTON_LEFT && !showMenu) {
			Vec4 mouseCoords(static_cast<float>(sdlEvent.button.x), static_cast<float>(sdlEvent.button.y), 0.0f, 1.0f);
			// TODO for Assignment 2: 
			// Get a ray pointing into the world, We have the x, y pixel coordinates
			// Need to convert this into world space to build our ray
			printf("Mouse Click: \n");

			// Transform mouse pos into world pos
			Matrix4 ndcToPixel = MMath::viewportNDC(1280, 720);
			Vec4 mouseNDCCoords = MMath::inverse(ndcToPixel) * mouseCoords;
			Matrix4 perspectiveToNDC = camera->GetProjectionMatrix();
			Vec4 mousePrespectiveCoords = MMath::inverse(perspectiveToNDC)* mouseNDCCoords;
			mousePrespectiveCoords /= mousePrespectiveCoords.w;
			Matrix4 worldToPerspective = camera->GetViewMatrix();
			Vec4 mouseWorldCoords = MMath::inverse(worldToPerspective) * mousePrespectiveCoords;

			//Arbitrary max distance for selection
			float distance = 1000.0f; 

			// Create a ray from the camera
			Vec3 rayStart = -cameraTransform->pos;
			Vec3 rayDir = VMath::normalize(mouseWorldCoords - rayStart);

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

	case SDL_CONTROLLERBUTTONDOWN:
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

		default: 
			break;
		}

		break;
	default:
		break;
	}
	mtx.unlock();
}

void Scene0::Update(const float deltaTime)
{
	std::mutex mtx;
	mtx.lock();
	for (unsigned int i = 0; i < rays.size(); i++)
	{
		rays[i]->age += deltaTime;
		if (rays[i]->age >= rays[i]->maxAge)
		{
			rays.erase(rays.begin() + i);
		}
	}
	mtx.unlock();
}

void Scene0::Render() const
{
	std::mutex mtx;
	mtx.lock();
	switch (renderer->GetRendererType()) {

	case RendererType::OPENGL:
		glEnable(GL_DEPTH_TEST);
		glEnable(GL_STENCIL_TEST);
		glStencilOp(GL_KEEP, GL_KEEP, GL_REPLACE);
		glClearColor(bGColor.x, bGColor.y, bGColor.z, bGColor.w);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);
		glBindBuffer(GL_UNIFORM_BUFFER, camera->GetMatricesID());
		glBindBuffer(GL_UNIFORM_BUFFER, light->GetLightID());
		// Let it go
		glBindTexture(GL_TEXTURE_2D, 0);

		if(renderCubemap) skybox->Render();

		glStencilFunc(GL_ALWAYS, 1, 0xFF);
		glStencilMask(0xFF);

		// Draw unselected actors		
		for (auto it = actors.begin(); it != actors.end(); ++it) {
			Ref<Actor> actor = std::dynamic_pointer_cast<Actor>(it->second);
			
			if (actor != selectedActor)
			{
				// Draw actor mesh
				glUseProgram(actor->GetComponent<MaterialComponent>()->GetShader()->GetProgram());
				glUniformMatrix4fv(actor->GetComponent<MaterialComponent>()->GetShader()->GetUniformID("modelMatrix"), 1, GL_FALSE, actor->GetModelMatrix());

				actor->GetComponent<MaterialComponent>()->SetCubemap(skybox->GetCubemap());

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
		}		

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

				selectedActor->GetComponent<MaterialComponent>()->SetCubemap(skybox->GetCubemap());

				selectedActor->GetComponent<MaterialComponent>()->Render();
				if (renderMeshes) {
					selectedActor->GetComponent<MeshComponent>()->Render(GL_TRIANGLES);
				}
				

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

		break;

	case RendererType::VULKAN:

		break;

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
		if (ImGui::CollapsingHeader("Scene"))
		{
			showSceneSettings();
			showHeirarchy();

			if (ImGui::TreeNode("Add Actor"))
			{
				showAddActorMenu();
				ImGui::TreePop();
			}

			// If there is an actor selected show the dropdown
			if (selectedActor && ImGui::TreeNode(selectedActorName.c_str()))
			{
				showSelectionSettings();
				ImGui::TreePop();
			}
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
		for (const auto& component : assetManager.xmlAssets)
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
	// TODO:: Add an add actor menu
}

void Scene0::showMaterialMenu()
{
	Ref<MaterialComponent> material = selectedActor->GetComponent<MaterialComponent>();
	if (!material) return;

	if (ImGui::TreeNode("Material"))
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
