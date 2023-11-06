// Umer Noor 2022
// Original code from/taught by Dr. Scott Fielder in Game Engine 4 class. Lucky for me I was in that class

#pragma once
#include <glew.h>
#include "Actor.h"
#include <Vector.h>

// Let's make an enumeration to make constants want to make the different types of light styles
// Scott was comparing to what Unreal has
enum class LightStyle {
	// cool, if we set the first one as 1, the rest just increment by 1
	DirectionLight = 1, // direction is the cheapest, fastest one. Don't need where the light is
	PointLight, // for point light, photons dissapates by the inverse square of the distance
	SkyLight, 
	SpotLight
};

class LightActor : public Actor{
	LightActor(const LightActor&) = delete;
	LightActor(LightActor&&) = delete;
	LightActor& operator = (const LightActor&) = delete;
	LightActor& operator = (LightActor&&) = delete;

private:
	// the only valid inputs will be from the enum 
	LightStyle lightStyle;

	float intensity;
	Vec3 fallOff;
	// we're gonna try a UBO for lights too
	GLuint uboLightDataID; // can be an unsigned int instead of a GLuint
	const GLuint bindingPoint = 1; // remember we used = 0 for the camera

public:
	LightActor(Ref<Component> parent_, RendererType renderer_, LightStyle lightStyle_, Vec3 position_, Vec4 colour_,
		float intensity_ = 1.0f, Vec3 fallOff_ = Vec3(0.0f, 0.0f, 0.0f));
	LightActor(Ref<Component> parent_, RendererType renderer_);
	~LightActor();

	Vec3 position;
	Vec4 colour;

	Vec3 GetPosition() const { return position; }
	void SetPosition(Vec3 position_) { position = position_; }

	Vec3 GetColor() const { return colour; }
	void SetColor(Vec3 color_) { colour = color_; }

	bool OnCreate() override;
	void OnDestroy() override;

	void UpdateLightData();
	GLuint GetLightID() const { return uboLightDataID; }
};

