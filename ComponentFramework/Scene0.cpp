#include <glew.h>
#include <iostream>
#include <SDL.h>
#include "Debug.h"
#include "Scene0.h"
#include <MMath.h>
#include "Debug.h"
#include "TransformComponent.h"
#include "MaterialComponent.h"
#include <QMath.h>
#include "XMLAssetManager.h"
#include "ShaderComponent.h"
#include "MeshComponent.h"
#include "ShapeComponent.h"

#include "ControllerManager.h"

#include "imgui.h"
#include "imgui_impl_opengl3.h"

Scene0::Scene0(Ref<Renderer> renderer_) : Scene(renderer_), bGColor(Vec4(0.0f, 0.0f, 0.0f, 1.0f)), debugColor(Vec4(1.0f, 0.0f, 0.0f, 1.0f)),
	selectionColor(Vec4(1.0f, 0.5f, 0.0f, 1.0f)), selectedActorName("")
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
		XMLAssetManager assetManager;
		// Make sure these names match the stuff in your xml file:
		std::vector<std::string> names{
			"ActorGameBoard" , "ActorChecker1", "ActorChecker2",
			"ActorSkull", "ActorCube", "ActorMario"
		};
		for (const auto& name : names) {
			auto asset = assetManager.xmlAssets.find(name);
			actors[name] = asset->second;
		}
		camera = std::dynamic_pointer_cast<CameraActor>(assetManager.xmlAssets.find("Camera1")->second);
		light = std::dynamic_pointer_cast<LightActor>(assetManager.xmlAssets.find("Light1")->second);

		debugShader = std::make_shared<ShaderComponent>(nullptr, "shaders/debugVert.glsl", "shaders/debugFrag.glsl"); // TODO:: Set this up in the xml file
		if (debugShader->OnCreate() == false)
		{
			return false;
		}
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
}

void Scene0::HandleEvents(const SDL_Event& sdlEvent)
{
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
		// Toggle Menu
		if (sdlEvent.key.keysym.scancode == SDL_SCANCODE_TAB)
		{
			showMenu = !showMenu;
		}
		break;
		
	case SDL_MOUSEBUTTONDOWN:
		if (sdlEvent.button.button == SDL_BUTTON_LEFT && !showMenu) {
			Vec4 mouseCoords(static_cast<float>(sdlEvent.button.x), static_cast<float>(sdlEvent.button.y), 0.0f, 1.0f);
			// TODO for Assignment 2: 
			// Get a ray pointing into the world, We have the x, y pixel coordinates
			// Need to convert this into world space to build our ray
			printf("Mouse Click: \n");

			Matrix4 ndcToPixel = MMath::viewportNDC(1280, 720);
			Vec4 mouseNDCCoords = MMath::inverse(ndcToPixel) * mouseCoords;

			Matrix4 perspectiveToNDC = camera->GetProjectionMatrix();
			Vec4 mousePrespectiveCoords = MMath::inverse(perspectiveToNDC)* mouseNDCCoords;

			mousePrespectiveCoords /= mousePrespectiveCoords.w;

			Matrix4 worldToPerspective = camera->GetViewMatrix();
			Vec4 mouseWorldCoords = MMath::inverse(worldToPerspective) * mousePrespectiveCoords;

			// Create a ray from the camera
			Vec3 rayStart = -cameraTransform->pos;
			Vec3 rayDir = VMath::normalize(mouseWorldCoords - rayStart);

			Ref<Ray> ray = std::make_shared<Ray>(rayStart, rayDir);

			rays.push_back(ray);

			float distance = 1000.0f; //Arbitrary max distance for selection

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

				Ray localSpaceRay(localSpaceRayStart, localSpaceRayDir);
				
				RayIntersectionInfo rayInfo = shapeComponent->shape->rayIntersectionInfo(localSpaceRay);
				if (rayInfo.isIntersected && rayInfo.t < distance)
				{
					distance = rayInfo.t;
					std::cout << "Picked: " << it->first << ", Distance: " << distance << "\n";
					selectedActorName = it->first;
					selectedActor = actor;
					selectedTransform = selectedActor->GetComponent<TransformComponent>();
				}				
			}			
		}
		break;

	case SDL_CONTROLLERBUTTONDOWN:
		break;
	default:
		break;
	}

}

void Scene0::Update(const float deltaTime)
{
	HandleGUI();
	for (int i = 0; i < rays.size(); i++)
	{
		rays[i]->age += deltaTime;
		if (rays[i]->age >= rays[i]->maxAge)
		{
			rays.erase(rays.begin() + i);
		}
	}
}

void Scene0::Render() const
{
	switch (renderer->GetRendererType()) {

	case RendererType::OPENGL:

		glEnable(GL_DEPTH_TEST);
		glStencilOp(GL_KEEP, GL_KEEP, GL_REPLACE);
		glClearColor(bGColor.x, bGColor.y, bGColor.z, bGColor.w);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);
		glBindBuffer(GL_UNIFORM_BUFFER, camera->GetMatricesID());
		glBindBuffer(GL_UNIFORM_BUFFER, light->GetLightID());
		// Let it go
		glBindTexture(GL_TEXTURE_2D, 0);

		for (auto it = actors.begin(); it != actors.end(); ++it) {
			Ref<Actor> actor = std::dynamic_pointer_cast<Actor>(it->second);

			if (selectedActor)
			{
				if (actor == selectedActor)
				{
					glStencilFunc(GL_ALWAYS, 1, 0xFF);
					glStencilMask(0xFF);
				}
				else
				{
					glStencilMask(0x00);
				}
			}

			glUseProgram(actor->GetComponent<ShaderComponent>()->GetProgram());
			glUniformMatrix4fv(actor->GetComponent<ShaderComponent>()->GetUniformID("modelMatrix"), 1, GL_FALSE, actor->GetModelMatrix());
			glBindTexture(GL_TEXTURE_2D, actor->GetComponent<MaterialComponent>()->getTextureID());
			if (renderMeshes) {
				actor->GetComponent<MeshComponent>()->Render(GL_TRIANGLES);
			}

			glStencilMask(0x00);
			glUseProgram(debugShader->GetProgram());
			if (renderCollisionShapes) {
				// Drawing the primitive geometry associated with the mesh to help debug ray intersects, culling, and collision detection
				glUniformMatrix4fv(debugShader->GetUniformID("modelMatrix"), 1, GL_FALSE, actor->GetModelMatrix());
				glUniform4fv(debugShader->GetUniformID("debugColor"), 1, debugColor);
				actor->GetComponent<ShapeComponent>()->Render();
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

		if (selectedActor)
		{
			// Draw Outline Around Selected Object
			glStencilFunc(GL_NOTEQUAL, 1, 0xFF);
			glStencilMask(0x00);
			glDisable(GL_DEPTH_TEST);

			glUseProgram(debugShader->GetProgram());
			glUniformMatrix4fv(debugShader->GetUniformID("modelMatrix"), 1, GL_FALSE, selectedActor->GetModelMatrix());
			glUniform4fv(debugShader->GetUniformID("debugColor"), 1, selectionColor);
			selectedActor->GetComponent<MeshComponent>()->Render();

			glStencilMask(0xFF);
			glStencilFunc(GL_ALWAYS, 1, 0xFF);
			glEnable(GL_DEPTH_TEST);
		}

		break;

	case RendererType::VULKAN:

		break;

	default:
		break;

	}

}

void Scene0::HandleGUI()
{
	switch (renderer->GetRendererType())
	{
	case RendererType::OPENGL:
	{
		bool open = true;
		ImGui::Begin("Frame rate", &open, ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoBackground);
		ImGui::Text("%.1f ms/frame (%.1f FPS)", 1000.0f / ImGui::GetIO().Framerate, ImGui::GetIO().Framerate);
		ImGui::End();

		// Hide menu when interacting with scene
		if (showMenu)
		{
			ImGui::Begin("Scene Settings", &open, ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoResize);
			ImGui::SetWindowSize(ImVec2(600.0f, 300.0f));
			if (ImGui::CollapsingHeader("Colors"))
			{
				ImGui::ColorEdit4("Background Color", bGColor);
				ImGui::ColorEdit4("Debug Color", debugColor);
				ImGui::ColorEdit4("Selection Color", selectionColor);
			}
			if (ImGui::CollapsingHeader("Rendering"))
			{
				ImGui::Checkbox("Render Meshes", &renderMeshes);
				ImGui::Checkbox("Render Colliders", &renderCollisionShapes);
				ImGui::Checkbox("Render Raycasts", &renderRaycasts);
			}

			if (selectedActor)
			{
				if (ImGui::CollapsingHeader("Selected Actor"))
				{
					ImGui::Text("Actor Name: %s", selectedActorName.c_str());
					if (selectedTransform)
					{
						if (ImGui::TreeNode("Transform"))
						{
							showTransformMenu();
							ImGui::TreePop();
						}
					}
				}
			}

			ImGui::End();
		}
	}
		break;
	case RendererType::VULKAN:
		break;
	default:
		break;
	}

}

void Scene0::showTransformMenu()
{
	if (ImGui::TreeNode("Position")) {
		ImGui::DragFloat("X", &selectedTransform->pos.x, 0.1f, -100.0f, 100.0f);
		ImGui::DragFloat("Y", &selectedTransform->pos.y, 0.1f, -100.0f, 100.0f);
		ImGui::DragFloat("Z", &selectedTransform->pos.z, 0.1f, -100.0f, 100.0f);
		ImGui::TreePop();
	}

	if (ImGui::TreeNode("Rotation")) {
		ImGui::DragFloat("X", &selectedTransform->rotation.xAxis, 0.1f, -360.0f, 360.0f, "%.3f Degrees");
		ImGui::DragFloat("Y", &selectedTransform->rotation.yAxis, 0.1f, -360.0f, 360.0f, "%.3f Degrees");
		ImGui::DragFloat("Z", &selectedTransform->rotation.zAxis, 0.1f, -360.0f, 360.0f, "%.3f Degrees");
		selectedTransform->UpdateOrientation();
		ImGui::TreePop();
	}

	if (ImGui::TreeNode("Scale")) {
		ImGui::DragFloat("X", &selectedTransform->scale.x, -100.0f, 100.0f);
		ImGui::DragFloat("Y", &selectedTransform->scale.y, -100.0f, 100.0f);
		ImGui::DragFloat("Z", &selectedTransform->scale.z, -100.0f, 100.0f);
		ImGui::TreePop();
	}

}

