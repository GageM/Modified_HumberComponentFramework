#pragma once

#include "Shape.h"
#include <Vector.h>
#include <Quaternion.h>

namespace GEOMETRY
{
	struct Box : public MATH::Vec3, public Shape
	{
		MATH::Vec3 halfExtents;
		MATH::Quaternion orientation;

		void set(MATH::Vec3 pos, MATH::Vec3 halfExtents_, MATH::Quaternion orientation_)
		{
			x = pos.x; y = pos.y; z = pos.z;
			halfExtents = halfExtents_;
			orientation = orientation_;
			generateVerticesAndNormals();
		}

		// Default box
		Box()
		{
			set(MATH::Vec3(), MATH::Vec3(1.0f, 1.0f, 1.0f), MATH::Quaternion());
		}

		// Default Box at position
		Box(MATH::Vec3 pos)
		{
			set(pos, MATH::Vec3(1.0f, 1.0f, 1.0f), MATH::Quaternion());
		}

		// Box with Default Orientation
		Box(MATH::Vec3 pos, MATH::Vec3 halfExtents_)
		{
			set(pos, halfExtents_, MATH::Quaternion());
		}

		// Box
		Box(MATH::Vec3 pos, MATH::Vec3 halfExtents_, MATH::Quaternion orientation_)
		{
			set(pos, halfExtents_, orientation_);
		}

		void generateVerticesAndNormals() override;

		RayIntersectionInfo rayIntersectionInfo(const Ray& ray) const override;

		enum FaceDirection
		{
			PosX,
			NegX,
			PosY,
			NegY,
			PosZ,
			NegZ
		};

		RayIntersectionInfo checkFaceIntersection(const Ray& ray, const FaceDirection dir) const;
	};
}