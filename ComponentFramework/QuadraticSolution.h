#pragma once
#include "Vector.h"
#include "VMath.h"
#include "Sphere.h"
#include <algorithm>
#include "Ray.h"

enum class NumSolutions
{
	zero = 0,
	one,
	two
};

struct QuadraticSolution
{
	NumSolutions solutions{ NumSolutions::zero };

	float firstSolution = 0.0f;
	float secondSolution = 0.0f;
	void print(const char* msg = nullptr) const;

	void SolveQuadratic(const float a, const float b, const float c)
	{
		float discriminant = (b * b) - (4 * a * c);
		if (discriminant < 0)
		{
			solutions = NumSolutions::zero;
		}
		else if (discriminant > -VERY_SMALL && discriminant < VERY_SMALL) // floats have rounding errors
		{
			solutions = NumSolutions::one;
			firstSolution = (-b + sqrt(discriminant)) / (2 * a);
			secondSolution = NAN;
		}
		else
		{
			solutions = NumSolutions::two;
			firstSolution = (-b + sqrt(discriminant)) / (2 * a);
			secondSolution = (-b - sqrt(discriminant)) / (2 * a);
			if (firstSolution > secondSolution) std::swap(firstSolution, secondSolution);
		}
	}
};


