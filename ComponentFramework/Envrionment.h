// Umer Noor 2022
// Original code from/taught by Dr. Scott Fielder. Lucky for me I was in that class (Game Engine 4)

#pragma once
#include <vector> 
#include <iostream>
#include "Actor.h"
#include <Matrix.h>
#include "MeshComponent.h"
#include "MaterialComponent.h"
#include "ShaderComponent.h"

using namespace MATH;
class Environment: public Actor {
	/// Unless you know what these do don't allow them
	/// to be created implicitly 
	Environment(const Environment&) = delete;
	Environment(Environment&&) = delete;
	Environment& operator=(const Environment&) = delete;
	Environment& operator=(Environment&&) = delete;

protected:
	Ref<MeshComponent> mesh;
	Ref<ShaderComponent> shader;
	Ref<MaterialComponent> material;

	Matrix4 modelMatrix;

public:
	Environment(Component* parent_);
	~Environment();
	bool OnCreate();
	void OnDestroy();
	void Update(const float deltaTime_);
	void Render() const;
	
	// Getting the parenting thing working
	Matrix4 GetModelMatrix();
};

