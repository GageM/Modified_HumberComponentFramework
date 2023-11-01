#include "QuadraticSolution.h"

void QuadraticSolution::print(const char* msg) const
{
	if (msg != nullptr)
	{
		printf("%s\n", msg);
	}
	switch (solutions)
	{
	case NumSolutions::zero:
		printf("No Solutions\n");
		break;
	case NumSolutions::one:
		printf("One Solution\n");
		printf("Solution: %f\n", firstSolution);
		break;
	case NumSolutions::two:
		printf("Two Solutions\n");
		printf("First Solution: %f, Second Solution: %f\n", firstSolution, secondSolution);
		break;
	default:
		break;
	}
}