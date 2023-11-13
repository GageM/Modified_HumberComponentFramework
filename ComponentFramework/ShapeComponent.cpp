#include "ShapeComponent.h"
using namespace GEOMETRY;
using namespace MATH;

// Sphere ShapeComponent Constructor: pass by value
ShapeComponent::ShapeComponent(Ref<Component> parent_, Ref<Renderer> renderer_, Sphere sphere_) :Component(parent_, renderer_)
{
	shapeType = ShapeType::sphere;
	// We are sure at this point that shape should be a sphere
	// So we can safely make a shared pointer of type sphere with the constructor Sphere(MATH::Vec3 centre, float r)
	shape = std::make_shared<Sphere>(Vec3(sphere_.x, sphere_.y, sphere_.z), sphere_.r);
}

/*
// Sphere ShapeComponent Constructor: pass by reference
ShapeComponent::ShapeComponent(Component* parent_, Sphere* sphere_) :Component(parent_)
{
	shapeType = ShapeType::sphere;
	// We are sure at this point that shape should be a cylinder
	// So we can safely make a shared pointer of type cylinder with the constructor Cylinder(Radius, CapA, CapB)
	shape = std::make_shared<Cylinder>(sphere_);
}
*/

// TODO for Assignment 1:
// The other constructors that take in Cylinder, Capsule, or Box 

// Cylinder ShapeComponent Constructor: pass by value
ShapeComponent::ShapeComponent(Ref<Component> parent_, Ref<Renderer> renderer_, Cylinder cylinder_) :Component(parent_, renderer_)
{
	shapeType = ShapeType::cylinder;
	// We are sure at this point that shape should be a cylinder
	// So we can safely make a shared pointer of type cylinder with the constructor Cylinder(Radius, CapA, CapB)
	shape = std::make_shared<Cylinder>(cylinder_.r, cylinder_.capPosA, cylinder_.capPosB);
}

/*
// Cylinder ShapeComponent Constructor: pass by reference
ShapeComponent::ShapeComponent(Component* parent_, Cylinder* cylinder_) :Component(parent_)
{
	shapeType = ShapeType::cylinder;
	// We are sure at this point that shape should be a cylinder
	// So we can safely make a shared pointer of type cylinder with the constructor Cylinder(Radius, CapA, CapB)
	shape = std::make_shared<Cylinder>(cylinder_);
}
*/

// Capsule ShapeComponent Constructor: pass by value
ShapeComponent::ShapeComponent(Ref<Component> parent_, Ref<Renderer> renderer_, Capsule capsule_) :Component(parent_, renderer_)
{
	shapeType = ShapeType::capsule;
	// We are sure at this point that shape should be a cylinder
	// So we can safely make a shared pointer of type cylinder with the constructor Capsule(Radius, CapA, CapB)
	shape = std::make_shared<Capsule>(capsule_.r, capsule_.capPosA, capsule_.capPosB);
}

/*
// Capsule ShapeComponent Constructor: pass by reference
ShapeComponent::ShapeComponent(Component* parent_, Capsule* capsule_) :Component(parent_)
{
	shapeType = ShapeType::capsule;
	// We are sure at this point that shape should be a cylinder
	// So we can safely make a shared pointer of type cylinder with the constructor Capsule(Radius, CapA, CapB)
	shape = std::make_shared<Capsule>(capsule_);
}

*/

// Capsule ShapeComponent Constructor: pass by value
ShapeComponent::ShapeComponent(Ref<Component> parent_, Ref<Renderer> renderer_, Box box_) :Component(parent_, renderer_)
{
	shapeType = ShapeType::box;
	// We are sure at this point that shape should be a box
	// So we can safely make a shared pointer of type box with the constructor Box(position, halfExtent, orientation)
	shape = std::make_shared<Box>(Vec3(box_.x, box_.y, box_.z), box_.halfExtents, box_.orientation);
}

/*
// Capsule ShapeComponent Constructor: pass by reference
ShapeComponent::ShapeComponent(Component* parent_, Box* box_) :Component(parent_)
{
	shapeType = ShapeType::box;
	// We are sure at this point that shape should be a box
	// So we can safely make a shared pointer of type box with the constructor Capsule(Radius, CapA, CapB)
	shape = std::make_shared<Capsule>(box_);
}

*/

ShapeComponent::~ShapeComponent()
{
}

bool ShapeComponent::OnCreate()
{
	return true;
}

void ShapeComponent::OnDestroy()
{
}

void ShapeComponent::Update(const float deltaTime_)
{
}

void ShapeComponent::Render() const
{
	shape->debugDraw();
}
