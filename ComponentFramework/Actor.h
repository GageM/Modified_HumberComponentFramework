// Umer Noor 2022
// Original code from/taught by Dr. Scott Fielder. Lucky for me I was in that class (Game Engine 4)

#pragma once
#include <vector> 
#include <unordered_map>
#include <iostream>
#include "Component.h"
#include <Matrix.h>

using namespace MATH;
class Actor: public Component {
	/// Unless you know what these do don't allow them
	/// to be created implicitly 
	Actor(const Actor&) = delete;
	Actor(Actor&&) = delete;
	Actor& operator=(const Actor&) = delete;
	Actor& operator=(Actor&&) = delete;

protected:
	// Make an unordered map of shared_ptrs
	std::unordered_map<std::string, Ref<Component>> components;
	Matrix4 modelMatrix;
public:
	Actor(Component* parent_);
	~Actor();
	bool OnCreate();
	void OnDestroy();
	void Update(const float deltaTime_);
	void Render() const;


	inline std::unordered_map<std::string, Ref<Component>> GetComponentList() const { return components; }
	
	// Components must already be created in order to add them to an actor

	/*
	// Adds a component given a name, type and construction arguments
	template<typename Str = std::string, typename Type, typename ... Args>
	void AddComponent(std::string name, Args&& ... args_)
	{
		// Check if there is a component of that name already attached to the actor
		if (GetComponent<Type>(name).get() != nullptr) 
		{
#ifdef _DEBUG
			std::cerr << "WARNING: A Component with name: " << name << " is already attached to this actor - ignoring/n";
#endif // _DEBUG
			return;
		}

		// Create and attach component to actor
		components[name] = std::make_shared<ComponentTemplate>(std::forward<Args>(args_)...));
	}
	*/

	// Adds a component given a name, type and preconstructed component
	template<typename Type>
	void AddComponent(std::string name, Ref<Type> component_)	
	{
		// Check if there is a component of that name already attached to the actor
		if (GetComponent<Type>(name).get() != nullptr) 
		{
#ifdef _DEBUG
			std::cerr << "WARNING: A Component with name: " << name << " is already attached to this actor - ignoring/n";
#endif // _DEBUG
			return;
		}

		// Create and attach component to actor
		components[name] = component_;
	}

	// Returns a component by a given name & type
	template<typename Type>
	Ref<Type> GetComponent(std::string name) const
	{
		for (const auto& component : components)
		{
			// if the component has the same name and is the right type
			if (name == component.first && std::dynamic_pointer_cast<Type>(component.second))
			{
				return std::dynamic_pointer_cast<Type>(component.second);
			}
		}

		// if no matching component was found return a nullptr
		return Ref<Type>(nullptr);
	}

	// Returns the first component of a given type
	template<typename Type>
	Ref<Type> GetComponent() const
	{
		for (const auto& component : components)
		{
			// if the component has the same name and is the right type
			if (std::dynamic_pointer_cast<Type>(component.second))
			{
				return std::dynamic_pointer_cast<Type>(component.second);
			}
		}

		// if no matching component was found return a nullptr
		return Ref<Type>(nullptr);
	}

	// Removes a component given a name and type
	template<typename Type>
	void RemoveComponent(std::string name)
	{
		for (const auto& component : components)
		{
			if (name == component.first && std::dynamic_pointer_cast<Type>(components[name]) != nullptr)
			{
				components.erase(name);
				break;
			}
		}
	}

	// Removes the first component of type
	template<typename Type>
	void RemoveComponent()
	{
		for (const auto& component : components)
		{
			if (std::dynamic_pointer_cast<Type>(component) != nullptr)
			{
				components.erase(component.first);
				break;
			}
		}
	}

	// Note that these are not templated, so will write this in cpp
	void RemoveAllComponents();
	void ListComponents() const;

	// Getting the parenting thing working
	Matrix4 GetModelMatrix();
};

