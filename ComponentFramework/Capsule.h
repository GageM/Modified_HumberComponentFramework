#pragma once

#include "Shape.h"
#include <Vector.h>
#include <vector>

namespace GEOMETRY
{
	struct Capsule: public Shape
	{
		float r;
		MATH::Vec3 capPosA, capPosB;

		void set(float r_, MATH::Vec3 capPosA_, MATH::Vec3 capPosB_)
		{
			r = r_;
			capPosA = capPosA_; 
			capPosB = capPosB_;
			generateVerticesAndNormals();
		}
		Capsule()
		{
			set(1.0f, MATH::Vec3(0.0f, -1.0f, 0.0f), MATH::Vec3(0.0f, 1.0f, 0.0f));
		}

		Capsule(float r_)
		{
			set(1.0f, MATH::Vec3(0.0f, -1.0f, 0.0f), MATH::Vec3(0.0f, 1.0f, 0.0f));
		}

		Capsule(float r_, MATH::Vec3 capPosA_)
		{
			set(r_, capPosA_, capPosA_ + MATH::Vec3(0.0f, 2.0f, 0.0f));
		}

		Capsule(float r_, MATH::Vec3 capPosA_, MATH::Vec3 capPosB_)
		{
			set(r_, capPosA_, capPosB_);
		}

		void generateVerticesAndNormals() override;

		RayIntersectionInfo rayIntersectionInfo(const Ray& ray) const override;

		RayIntersectionInfo checkInfiniteCylinder(const Ray& ray) const;

		RayIntersectionInfo checkEndSpheres(const Ray& ray) const;
	};
}