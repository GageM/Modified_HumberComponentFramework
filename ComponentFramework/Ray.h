#pragma once

#include <glew.h>
#include <vector>
#include <Matrix.h>
#include "Vector.h"
#include "VMath.h"
#include <algorithm>
#include "Component.h"
#include "TransformComponent.h"
#include "ShaderComponent.h"

	struct RayIntersectionInfo
	{
		bool isIntersected = false;
		MATH::Vec3 intersectionPoint;
		float t = 0.0f; // Distance Along The Ray

		void Reset()
		{
			isIntersected = false;
			intersectionPoint = Vec3();
			t = 0.0f; // Distance Along The Ray
		}
	};

	class Ray
	{
	public:
		Ray()
		{
			Set(MATH::Vec3(0.0f, 0.0f, 0.0f), MATH::Vec3(0.0f, 0.0f, 1.0f), 10000);
		}

		Ray(const MATH::Vec3& start_, const MATH::Vec3& dir_, float maxDistance_ = 10000)
		{
			Set(start_, dir_, maxDistance_);
		}

		void Set(const MATH::Vec3& start_, const MATH::Vec3& dir_, float maxDistance_ = 10000)
		{
			start = start_;
			dir = dir_;
			maxDistance = maxDistance_;

			generateVerticesAndNormals();
			age = 0.0f;
		}

		MATH::Vec3 currentPos(float t) const { return start + (dir * t); }

		MATH::Vec3 GetStart() const { return start; }
		MATH::Vec3 GetDir() const { return dir; }
	
		// For Drawing Rays

		const void DrawRay();

		// THIS IS IN SECONDS
		float age;
		const float maxAge = 3.0f;

	private:
		MATH::Vec3 start;
		MATH::Vec3 dir;
		float maxDistance;

		// For Drawing Rays

		// It's handy for any shape to draw itself for debugging purposes
		virtual void generateVerticesAndNormals();
		// Some concrete data and functions that every child will need
		void debugDraw();
		void StoreMeshData(GLenum drawmode_);
		std::vector<MATH::Vec3> vertices;
		std::vector<MATH::Vec3> normals;
		GLuint vao = 0;
		GLuint vbo = 0;
		size_t dataLength = 0;
		GLenum drawmode = GL_POINTS;
	};

