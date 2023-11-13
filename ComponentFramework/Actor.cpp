#include "Actor.h"
#include "TransformComponent.h"
#include "Debug.h"
Actor::Actor(Ref<Component> parent_, Ref<Renderer> renderer_):Component(parent_, renderer_) {}

// destructor usually deallocates memory
Actor::~Actor() {
	OnDestroy();
}

void Actor::OnDestroy() {
	RemoveAllComponents();
	Debug::Info("Deleting assets for Actor/Components: ", __FILE__, __LINE__);
	isCreated = false;
}

bool Actor::OnCreate() {
	if (isCreated) return isCreated;
	for (const auto &component : components) {
		if (component.second->OnCreate() == false) {
			Debug::Error("Loading assets for Actor/Components: ", __FILE__, __LINE__);
				isCreated = false;
				return isCreated;
		}
	}
	isCreated = true;
	return isCreated;
}

void Actor::Update(const float deltaTime) {
	std::cout << "Hello from Update\n";
}
void Actor::Render()const {}

void Actor::RemoveAllComponents() {
	// As we are using a vector list of shared ptrs, we won't have memory leaks!
	components.clear();
}

void Actor::ListComponents() const {
	std::cout << typeid(*this).name() << " contains the following components:\n";
	for (const auto& component : components)
	{
		std::cout << component.first << std::endl;
	}
	std::cout << std::endl;
}

Matrix4 Actor::GetModelMatrix()
{
	Ref<TransformComponent> transform = GetComponent<TransformComponent>();
	if (transform) {
		modelMatrix = transform->GetTransformMatrix();// if it exists, lets make a model matrix
	}
	else {
		modelMatrix.loadIdentity();
	}
	
	if (parent) { // if there is a parent, we can dynamic cast the component to an actor
		modelMatrix = std::dynamic_pointer_cast<Actor>(parent)->GetModelMatrix()* modelMatrix;
	}

	return modelMatrix;
}
