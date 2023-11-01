#include "Envrionment.h"
#include "TransformComponent.h"
#include "Debug.h"
Environment::Environment(Component* parent_):Actor(parent_) {}

// destructor usually deallocates memory
Environment::~Environment() {
	OnDestroy();
}

void Environment::OnDestroy() {
	RemoveAllComponents();
	Debug::Info("Deleting assets for Environment/Components: ", __FILE__, __LINE__);
	isCreated = false;
}

bool Environment::OnCreate() {
	if (isCreated) return isCreated;

	mesh = std::make_shared<MeshComponent>(nullptr, "meshes/Sphere.obj");
	mesh->OnCreate();
	shader = std::make_shared<ShaderComponent>(nullptr, "Shaders/EnvMap.vert", "Shaders/EnvMap.frag");
	shader->OnCreate();
	material = std::make_shared<MaterialComponent>(nullptr, "textures/bonifacio_street.exr"); // Hopefully this will just load
	material->OnCreate();

	isCreated = true;
	return isCreated;

}

void Environment::Update(const float deltaTime) {
	std::cout << "Hello from Update\n";
}
void Environment::Render()const {}

Matrix4 Environment::GetModelMatrix()
{
	Ref<TransformComponent> transform = GetComponent<TransformComponent>();
	if (transform) {
		modelMatrix = transform->GetTransformMatrix();// if it exists, lets make a model matrix
	}
	else {
		modelMatrix.loadIdentity();
	}
	
	if (parent) { // if there is a parent, we can dynamic cast the component to an Environment
		modelMatrix = dynamic_cast<Environment*>(parent)->GetModelMatrix()* modelMatrix;
	}

	return modelMatrix;
}
