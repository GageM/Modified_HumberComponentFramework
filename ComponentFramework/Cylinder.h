#pragma once

#include "Shape.h"
#include <Vector.h>
#include <vector>

namespace GEOMETRY
{
	struct Cylinder: public Shape
	{
		float r;
		MATH::Vec3 capPosA;
		MATH::Vec3 capPosB;
		void set(float r_, MATH::Vec3 capPosA_, MATH::Vec3 capPosB_)
		{
			r = r_; 
			capPosA = capPosA_;
			capPosB = capPosB_;
			generateVerticesAndNormals();
		}

		Cylinder() {
			set(1.0f, MATH::Vec3(0.0f, -1.0f, 0.0f), MATH::Vec3(0.0f, 1.0f, 0.0f));
		}

		Cylinder(float r_)
		{
			set(r_, MATH::Vec3(0.0f, -1.0f, 0.0f), MATH::Vec3(0.0f, 1.0f, 0.0f));
		}

		Cylinder(float r_, MATH::Vec3 capPosA_, MATH::Vec3 capPosB_)
		{
			set(r_, capPosA_, capPosB_);
		}

		void generateVerticesAndNormals() override;

		RayIntersectionInfo rayIntersectionInfo(const Ray& ray) const override;

		RayIntersectionInfo checkInfiniteCylinder(const Ray& ray) const;

		RayIntersectionInfo checkEndCaps(const Ray& ray) const;
	};
}