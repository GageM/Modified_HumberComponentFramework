// Umer Noor 2022
// Original code from/taught by Dr. Scott Fielder in Game Engine 4 class. Lucky for me I was in that class
// Remember Scott's final assignment was reading in assets & Actors from XML file? Here was my attempt
#ifndef XMLASSETMANAGER_H
#define XMLASSETMANAGER_H

#include <unordered_map>
#include <vector>
#include "Component.h"
#include <iostream>
#include "tinyxml2.h"
#include "Actor.h"
#include "Renderer.h"
struct XMLAssetManager {
	// assets can contain meshes, materials, shaders, or actors
	std::unordered_map< std::string, Ref<Component> > xmlAssets;

	// Need a ref to the renderer to choose whether to build assets for openGL or Vulkan
	Ref<Renderer> renderer;

	template<typename ComponentTemplate, typename ... Args>
	void AddComponent(const char* name, Args&& ... args_) {
		// Make the component shared pointer based on the arguments
		Ref<ComponentTemplate> component = std::make_shared<ComponentTemplate>(std::forward<Args>(args_)...);
		// Add to the unordered map
		xmlAssets[name] = component;
	}

	// Have an overloaded AddComponent that takes in a fully built actor
	template<typename ComponentTemplate>
	void AddComponent(const char* name, Ref<ComponentTemplate> component) {
		xmlAssets[name] = component;
	}

	template<typename ComponentTemplate>
	Ref<ComponentTemplate> GetComponent(const char* name) const {
		Ref<ComponentTemplate> assetWeNeed;
		for (auto const& pair : xmlAssets) {
			if (std::string(pair.first) == name) {
				assetWeNeed = std::dynamic_pointer_cast<ComponentTemplate>(pair.second);
			}
		}
		return assetWeNeed;
	}

	// Returns a vector of components of a given type
	template<typename Type>
	std::unordered_map<std::string, Ref<Type>> GetAllComponentsOfType() const
	{
		std::unordered_map< std::string, Ref<Type> > componentsOfType;		

		for (auto const& pair : xmlAssets) {
			if (std::dynamic_pointer_cast<Type>(pair.second) != nullptr) {
				componentsOfType[pair.first] = std::dynamic_pointer_cast<Type>(pair.second);
			}
		}
		return componentsOfType;
	}

	void AddSphereShape(const tinyxml2::XMLElement* child);
	void AddCylinderShape(const tinyxml2::XMLElement* child); 
	void AddCapsuleShape(const tinyxml2::XMLElement* child); 
	void AddBoxShape(const tinyxml2::XMLElement* child); 
	
	void AddTexture(const tinyxml2::XMLElement* child);
	void AddCubemap(const tinyxml2::XMLElement* child);
	void AddShader(const tinyxml2::XMLElement* child);
	void AddMaterial(const tinyxml2::XMLElement* child); // NEW!
	void AddCamera(const tinyxml2::XMLElement* child);
	void AddLight(const tinyxml2::XMLElement* child);
	void AddSkybox(const tinyxml2::XMLElement* child, Ref<Component> parent); // NEW!
	void AddMeshToActor(const tinyxml2::XMLElement* child, Ref<Actor> actor);
	void AddShaderToActor(const tinyxml2::XMLElement* child, Ref<Actor> actor);
	void AddTextureToActor(const tinyxml2::XMLElement* child, Ref<Actor> actor);
	void AddShapeToActor(const tinyxml2::XMLElement* child, Ref<Actor> actor);
	void AddTransformToActor(const tinyxml2::XMLElement* child, Ref<Actor> actor, Ref<Component> parent);
	void AddPhysicsToActor  (const tinyxml2::XMLElement* child, Ref<Actor> actor, Ref<Component> parent);
	void AddMaterialToActor(const tinyxml2::XMLElement* child, Ref<Actor> actor, Ref<Component> parent); // NEW!

	void SaveAssets();


	//XMLAssetManager() {}
	XMLAssetManager(Ref<Renderer> renderer_);
	~XMLAssetManager();
};
#endif