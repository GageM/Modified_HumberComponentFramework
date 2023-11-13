#include "XMLAssetManager.h"
#include <iostream>
#include <fstream>
#include <string>
#include "ShaderComponent.h"
#include "MeshComponent.h"
#include "ShapeComponent.h"
#include "TextureComponent.h"
#include "CubemapComponent.h"
#include "MaterialComponent.h"
#include "PhysicsComponent.h"
#include "Skybox.h"
#include "CameraActor.h"
#include "LightActor.h"
#include "Sphere.h"
#include "Cylinder.h"
#include "Capsule.h"
#include <QMath.h>

XMLAssetManager::XMLAssetManager(Ref<Renderer> renderer_) : renderer(renderer_)
{
	tinyxml2::XMLDocument doc;
	doc.LoadFile("Scene0.xml");
	bool status = doc.Error();
	if (status) {
		std::cout << doc.ErrorIDToName(doc.ErrorID()) << std::endl;
		return;
	}
	// These are referred to as nodes. Get the top of the system
	tinyxml2::XMLElement* rootData = doc.RootElement();
	tinyxml2::XMLElement* sceneData = rootData->FirstChildElement("Scene0");

	for (const tinyxml2::XMLElement* child = rootData->FirstChildElement("Scene0")->FirstChildElement();
		child;
		child = child->NextSiblingElement())
	{
		switch (renderer->GetRendererType())
		{
		case RendererType::OPENGL:
			AddTexture(child);
			AddCubemap(child);
			AddShader(child);
			AddMaterial(child);
			AddCamera(child);
			AddLight(child);

			if (std::string(child->Name()) == "Mesh") {
				AddComponent<MeshComponent>(child->Attribute("name"), nullptr, renderer, child->Attribute("filename"));

				AddSphereShape(child);
				AddCylinderShape(child);
				AddCapsuleShape(child);
				AddBoxShape(child);
			}

			AddSkybox(child, xmlAssets["Camera1"]);
			break;
		case RendererType::VULKAN:
			//AddTexture(child);
			//AddCubemap(child);
			//AddShader(child);
			//AddMaterial(child);
			AddCamera(child);
			AddLight(child);

			break;
		default:
			break;
		}


	}

	// Now we have all the shared assets ready, time to build the actors
	for (const tinyxml2::XMLElement* child = rootData->FirstChildElement("Scene0")->FirstChildElement("Actor");
		child;
		child = child->NextSiblingElement())
	{
		if (std::string(child->Name()) == "Actor") {

			switch (renderer->GetRendererType())
			{
			case RendererType::OPENGL:
			{
				Ref<Component> parent = nullptr;
				if (std::string(child->Attribute("parent")) == "none") {
					parent = nullptr;
				}
				if (std::string(child->Attribute("parent")) == "ActorGameBoard") {
					parent = GetComponent<Actor>("ActorGameBoard");
				}

				Ref<Actor> actor = std::make_shared<Actor>(parent, renderer);
				// Add shared assets to the actor
				AddMeshToActor(child, actor);
				AddShaderToActor(child, actor);
				AddTextureToActor(child, actor);
				AddShapeToActor(child, actor);
				// The transform is unique for the actor. Needs the parent too
				AddTransformToActor(child, actor, parent);
				// Add physics to the actor AFTER the transform. We need them to match in position and orientation
				AddPhysicsToActor(child, actor, parent);
				AddMaterialToActor(child, actor, parent);

				actor->OnCreate();
				AddComponent(child->Attribute("actorname"), actor);
				break;
			}
			case RendererType::VULKAN:
			{
				Ref<Component> parent = nullptr;
				if (std::string(child->Attribute("parent")) == "none") {
					parent = nullptr;
				}
				if (std::string(child->Attribute("parent")) == "ActorGameBoard") {
					parent = GetComponent<Actor>("ActorGameBoard");
				}

				Ref<Actor> actor = std::make_shared<Actor>(parent, renderer);
				// Add shared assets to the actor
				//AddMeshToActor(child, actor);
				//AddShaderToActor(child, actor);
				//AddTextureToActor(child, actor);
				//AddShapeToActor(child, actor);
				// The transform is unique for the actor. Needs the parent too
				AddTransformToActor(child, actor, parent);
				// Add physics to the actor AFTER the transform. We need them to match in position and orientation
				//AddPhysicsToActor(child, actor, parent);
				//AddMaterialToActor(child, actor, parent);

				actor->OnCreate();
				AddComponent(child->Attribute("actorname"), actor);
				break;
			}
			default:
				break;
			}


		}
	}
}
XMLAssetManager::~XMLAssetManager()
{
	xmlAssets.clear();
}

void XMLAssetManager::AddSphereShape(const tinyxml2::XMLElement* child)
{
	if (std::string(child->FirstChildElement("Shape")->FirstChildElement()->Name()) == "Sphere") {
		GEOMETRY::Sphere sphere;

		sphere.r = child->FirstChildElement("Shape")->FirstChildElement("Sphere")->FloatAttribute("radius");
		
		sphere.x = child->FirstChildElement("Shape")->FirstChildElement("Sphere")->FloatAttribute("centreX");
		sphere.y = child->FirstChildElement("Shape")->FirstChildElement("Sphere")->FloatAttribute("centreY");
		sphere.z = child->FirstChildElement("Shape")->FirstChildElement("Sphere")->FloatAttribute("centreZ");

		AddComponent<ShapeComponent>(child->FirstChildElement("Shape")->Attribute("name"), nullptr, renderer, sphere);
	}
}

void XMLAssetManager::AddCylinderShape(const tinyxml2::XMLElement* child)
{
	if (std::string(child->FirstChildElement("Shape")->FirstChildElement()->Name()) == "Cylinder") {
		GEOMETRY::Cylinder cylinder;

		cylinder.r = child->FirstChildElement("Shape")->FirstChildElement("Cylinder")->FloatAttribute("radius");
				
		cylinder.capPosA.x = child->FirstChildElement("Shape")->FirstChildElement("Cylinder")->FloatAttribute("capAX");
		cylinder.capPosA.y = child->FirstChildElement("Shape")->FirstChildElement("Cylinder")->FloatAttribute("capAY");
		cylinder.capPosA.z = child->FirstChildElement("Shape")->FirstChildElement("Cylinder")->FloatAttribute("capAZ");
				
		cylinder.capPosB.x = child->FirstChildElement("Shape")->FirstChildElement("Cylinder")->FloatAttribute("capBX");
		cylinder.capPosB.y = child->FirstChildElement("Shape")->FirstChildElement("Cylinder")->FloatAttribute("capBY");
		cylinder.capPosB.z = child->FirstChildElement("Shape")->FirstChildElement("Cylinder")->FloatAttribute("capBZ");

		AddComponent<ShapeComponent>(child->FirstChildElement("Shape")->Attribute("name"), nullptr, renderer, cylinder);
	}
}

void XMLAssetManager::AddCapsuleShape(const tinyxml2::XMLElement* child)
{
	if (std::string(child->FirstChildElement("Shape")->FirstChildElement()->Name()) == "Capsule") {
		GEOMETRY::Capsule capsule;
		
		capsule.r = child->FirstChildElement("Shape")->FirstChildElement("Capsule")->FloatAttribute("radius");

		capsule.capPosA.x = child->FirstChildElement("Shape")->FirstChildElement("Capsule")->FloatAttribute("capAX");
		capsule.capPosA.y = child->FirstChildElement("Shape")->FirstChildElement("Capsule")->FloatAttribute("capAY");
		capsule.capPosA.z = child->FirstChildElement("Shape")->FirstChildElement("Capsule")->FloatAttribute("capAZ");

		capsule.capPosB.x = child->FirstChildElement("Shape")->FirstChildElement("Capsule")->FloatAttribute("capBX");
		capsule.capPosB.y = child->FirstChildElement("Shape")->FirstChildElement("Capsule")->FloatAttribute("capBY");
		capsule.capPosB.z = child->FirstChildElement("Shape")->FirstChildElement("Capsule")->FloatAttribute("capBZ");

		AddComponent<ShapeComponent>(child->FirstChildElement("Shape")->Attribute("name"), nullptr, renderer, capsule);
	}
}

void XMLAssetManager::AddBoxShape(const tinyxml2::XMLElement* child)
{
	if (std::string(child->FirstChildElement("Shape")->FirstChildElement()->Name()) == "Box") {
		GEOMETRY::Box box;

		box.x = child->FirstChildElement("Shape")->FirstChildElement("Box")->FloatAttribute("posX");
		box.y = child->FirstChildElement("Shape")->FirstChildElement("Box")->FloatAttribute("posY");
		box.z = child->FirstChildElement("Shape")->FirstChildElement("Box")->FloatAttribute("posZ");

		box.halfExtents.x = child->FirstChildElement("Shape")->FirstChildElement("Box")->FloatAttribute("hExtentX");
		box.halfExtents.y = child->FirstChildElement("Shape")->FirstChildElement("Box")->FloatAttribute("hExtentY");
		box.halfExtents.z = child->FirstChildElement("Shape")->FirstChildElement("Box")->FloatAttribute("hExtentZ");

		float angle;
		Vec3 axis;

		angle = child->FirstChildElement("Shape")->FirstChildElement("Box")->FloatAttribute("angleDeg");

		axis.x = child->FirstChildElement("Shape")->FirstChildElement("Box")->FloatAttribute("axisX");
		axis.y = child->FirstChildElement("Shape")->FirstChildElement("Box")->FloatAttribute("axisY");
		axis.z = child->FirstChildElement("Shape")->FirstChildElement("Box")->FloatAttribute("axisZ");

		box.orientation = Quaternion(angle, axis);

		AddComponent<ShapeComponent>(child->FirstChildElement("Shape")->Attribute("name"), nullptr, renderer, box);
	}
}

void XMLAssetManager::AddTexture(const tinyxml2::XMLElement* child)
{
	if (std::string(child->Name()) == "Texture") {
		Ref<TextureComponent> texture = std::make_shared<TextureComponent>(nullptr, renderer, child->Attribute("filename"));
		//texture->OnCreate();

		AddComponent<TextureComponent>(child->Attribute("name"), texture);

	}
}

void XMLAssetManager::AddCubemap(const tinyxml2::XMLElement* child)
{
	/*
	Ref<CubemapComponent> cubemap = std::make_shared<CubemapComponent>(nullptr, child->Attribute("PXfile"), child->Attribute("PYfile"), child->Attribute("PZfile"),
		child->Attribute("NXfile"), child->Attribute("NYfile"), child->Attribute("NZfile"));
	cubemap->OnCreate();
	AddComponent<CubemapComponent>(child->Attribute("name"), cubemap);
	*/
}

void XMLAssetManager::AddShader(const tinyxml2::XMLElement* child)
{
	if (std::string(child->Name()) == "Shader") {
		Ref<ShaderComponent> shader = std::make_shared<ShaderComponent>(nullptr, renderer, child->Attribute("vertFilename"), child->Attribute("fragFilename"));
		shader->OnCreate();
		AddComponent(child->Attribute("name"), shader);
	}
}

void XMLAssetManager::AddMaterial(const tinyxml2::XMLElement* child)
{
	
	if (std::string(child->Name()) == "Material")
	{
		Ref<MaterialComponent> material = std::make_shared<MaterialComponent>(nullptr, renderer,
			GetComponent<TextureComponent>(child->Attribute("BCMap")), 
			child->FloatAttribute("roughness"), 
			child->FloatAttribute("metallic"));

		material->SetShader(GetComponent<ShaderComponent>("PBR_Shader"));

		material->OnCreate();

		AddComponent(child->Attribute("name"), material);
	}
	
}

void XMLAssetManager::AddCamera(const tinyxml2::XMLElement* child)
{
	// Build camera
	Vec3 cameraPos;
	Vec3 axis;
	Vec3 scale;
	float angleDeg = 0.0f;
	Ref<Component> cameraParent = nullptr;

	if (child->Attribute("cameraname")) {
		if (child->FirstChildElement("Transform")) {
			cameraPos.x = child->FirstChildElement("Transform")->FloatAttribute("posx");
			cameraPos.y = child->FirstChildElement("Transform")->FloatAttribute("posy");
			cameraPos.z = child->FirstChildElement("Transform")->FloatAttribute("posz");
			angleDeg = child->FirstChildElement("Transform")->FloatAttribute("angleDeg");;
			axis.x = child->FirstChildElement("Transform")->FloatAttribute("axisx");
			axis.y = child->FirstChildElement("Transform")->FloatAttribute("axisy");
			axis.z = child->FirstChildElement("Transform")->FloatAttribute("axisz");
			scale.x = child->FirstChildElement("Transform")->FloatAttribute("scalex");
			scale.y = child->FirstChildElement("Transform")->FloatAttribute("scaley");
			scale.z = child->FirstChildElement("Transform")->FloatAttribute("scalez");

			if (std::string(child->Attribute("parent")) == "none") {
				cameraParent = nullptr;
			}
		}
	}

	if (std::string(child->Name()) == "Camera") {
		Ref<CameraActor> camera = std::make_shared<CameraActor>(cameraParent, renderer);
		Ref<TransformComponent> transform = std::make_shared<TransformComponent>(nullptr, renderer, cameraPos, QMath::angleAxisRotation(angleDeg, axis));

		camera->AddComponent<TransformComponent>("Transform", transform);
		camera->OnCreate();
		AddComponent(child->Attribute("cameraname"), camera);
	}
}

void XMLAssetManager::AddLight(const tinyxml2::XMLElement* child)
{
	// Build Light
	Vec3 lightPos;
	Vec4 colour;
	Vec3 falloff;
	Ref<Component> lightParent = nullptr;
	LightStyle lightstyle;
	float intensity = 1.0f;
	if (child->Attribute("lightname")) {
		lightPos.x = child->FloatAttribute("posx");
		lightPos.y = child->FloatAttribute("posy");
		lightPos.z = child->FloatAttribute("posz");

		colour.x = child->FloatAttribute("red");
		colour.y = child->FloatAttribute("green");
		colour.z = child->FloatAttribute("blue");
		colour.w = child->FloatAttribute("alpha");

		intensity = child->FloatAttribute("intensity");;

		falloff.x = child->FloatAttribute("falloffx");
		falloff.y = child->FloatAttribute("falloffy");
		falloff.z = child->FloatAttribute("falloffz");

		if (std::string(child->Attribute("parent")) == "none") {
			lightParent = nullptr;
		}

		if (std::string(child->Attribute("lightstyle")) == "DirectionLight") {
			lightstyle = LightStyle::DirectionLight;
		}

	}

	if (std::string(child->Name()) == "Light") {
		Ref<LightActor> light = std::make_shared<LightActor>(lightParent, renderer, lightstyle, lightPos, colour, intensity, falloff);

		light->OnCreate();

		AddComponent(child->Attribute("lightname"), light);
	}
}

void XMLAssetManager::AddSkybox(const tinyxml2::XMLElement* child, Ref<Component> parent)
{
	if (std::string(child->Name()) == "Skybox")
	{
		Ref<Skybox> skybox = std::make_shared<Skybox>(GetComponent<Component>(child->Attribute("parent")), renderer);

		AddComponent(child->Attribute("name"), skybox);
	}
}

void XMLAssetManager::AddMeshToActor(const tinyxml2::XMLElement* child, Ref<Actor> actor)
{
	if (child->FirstChildElement("Mesh")) {
		const char* name = child->FirstChildElement("Mesh")->Attribute("name");
		Ref<MeshComponent> mesh = GetComponent<MeshComponent>(name);
		actor->AddComponent<MeshComponent>(name, mesh);
	}

}

void XMLAssetManager::AddShaderToActor(const tinyxml2::XMLElement* child, Ref<Actor> actor)
{
	if (child->FirstChildElement("Shader")) {
		const char* name = child->FirstChildElement("Shader")->Attribute("name");
		Ref<ShaderComponent> shader = GetComponent<ShaderComponent>(name);
		actor->AddComponent<ShaderComponent>(name, shader);
	}
}

void XMLAssetManager::AddTextureToActor(const tinyxml2::XMLElement* child, Ref<Actor> actor)
{
	if (child->FirstChildElement("Texture")) {
		const char* name = child->FirstChildElement("Texture")->Attribute("name");
		Ref<TextureComponent> texture = GetComponent<TextureComponent>(name);
		actor->AddComponent<TextureComponent>(name, texture);
	}
}

void XMLAssetManager::AddShapeToActor(const tinyxml2::XMLElement* child, Ref<Actor> actor)
{
	if (std::string(child->FirstChildElement("Shape")->Name()) == "Shape") {
		const char* name = child->FirstChildElement("Shape")->Attribute("name");
		Ref<ShapeComponent> shape = GetComponent<ShapeComponent>(name);
		actor->AddComponent<ShapeComponent>(name, shape);
	}
}

void XMLAssetManager::AddTransformToActor(const tinyxml2::XMLElement* child, Ref<Actor> actor, Ref<Component> parent)
{
	if (child->FirstChildElement("Transform")) {
		Vec3 pos;
		Vec3 axis;
		Vec3 scale;
		pos.x = child->FirstChildElement("Transform")->FloatAttribute("posx");
		pos.y = child->FirstChildElement("Transform")->FloatAttribute("posy");
		pos.z = child->FirstChildElement("Transform")->FloatAttribute("posz");
		float angleDeg = child->FirstChildElement("Transform")->FloatAttribute("angleDeg");;
		axis.x = child->FirstChildElement("Transform")->FloatAttribute("axisx");
		axis.y = child->FirstChildElement("Transform")->FloatAttribute("axisy");
		axis.z = child->FirstChildElement("Transform")->FloatAttribute("axisz");
		scale.x = child->FirstChildElement("Transform")->FloatAttribute("scalex");
		scale.y = child->FirstChildElement("Transform")->FloatAttribute("scaley");
		scale.z = child->FirstChildElement("Transform")->FloatAttribute("scalez");
		
		Ref<TransformComponent> transform = std::make_shared<TransformComponent>(parent, renderer, pos, QMath::angleAxisRotation(angleDeg, axis), scale);
		actor->AddComponent<TransformComponent>("Transform", transform);
	}
}

void XMLAssetManager::AddPhysicsToActor(const tinyxml2::XMLElement* child, Ref<Actor> actor, Ref<Component> parent)
{
	if (child->FirstChildElement("Physics"))
	{
		// TODO for assignment 3
		// Add a default physics component to the actor, but don't forget to match transform component's position & orientation
		// You're gonna have to assume the transform component has already been built (fingers crossed!)
		Ref<TransformComponent> transform = actor->GetComponent<TransformComponent>();

		// Check if the actor has a transform component
		if (!transform)	return;

		Ref<PhysicsComponent> physics = nullptr;
		physics = std::make_shared<PhysicsComponent>(parent, renderer, transform, child->FirstChildElement("Physics")->FloatAttribute("mass"));
		actor->AddComponent<PhysicsComponent>("Physics", physics);
	}

}

void XMLAssetManager::AddMaterialToActor(const tinyxml2::XMLElement* child, Ref<Actor> actor, Ref<Component> parent)
{
	if (child->FirstChildElement("Material")) {
		const char* name = child->FirstChildElement("Material")->Attribute("name");
		Ref<MaterialComponent> material = GetComponent<MaterialComponent>(name);
		actor->AddComponent<MaterialComponent>(name, material);
	}
}



