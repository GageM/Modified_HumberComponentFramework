#pragma once
#include <Vector.h>
#include <vector>
#include <memory>

// Like many things in this project KD trees are not ready to be used for production code
// Use at you own risk


namespace KDTree {

	class Node
	{
	public:
		Node(std::vector<float> point_, int index_) : point(point_), index(index_), left(nullptr), right(nullptr)
		{
		}

		std::vector<float> point;
		int index;
		std::shared_ptr<Node> left;
		std::shared_ptr<Node> right;
	};

	// Recursive function to insert a point in the tree
	static std::shared_ptr<Node> insertRec(std::shared_ptr<Node> root, std::vector<float>& point, int index, unsigned depth)
	{
		// TODO::Shared pointer should prevent memory leaks
		if (root == nullptr) { return std::make_shared<Node>(point, index); }

		unsigned cd = depth % point.size();

		if (point[cd] < root->point[cd])
		{
			root->left = insertRec(root->left, point, index, depth + 1);
		}
		else
		{
			root->right = insertRec(root->right, point, index, depth + 1);
		}
		return root;
	}

	// Base function to insert a point into the tree
	static std::shared_ptr<Node> insert(std::shared_ptr<Node> root, std::vector<float>& point, int index)
	{
		return insertRec(root, point, index, 0);
	}

	static bool CompareVector(std::vector<float>& a, std::vector<float>& b)
	{
		for (int i = 0; i < a.size(); i++)
		{
			// if a is not almost identical to b
			if (a[i] < b[i] - VERY_SMALL || a[i] > b[i] + VERY_SMALL) return false;
		}

		return true;
	}

	// Recursive function to find the index of a point
	static int FindValueRec(std::shared_ptr<Node> root, std::vector<float>& point, unsigned depth)
	{
		// If there are no nodes in the tree return null result
		if (root == nullptr) return -1;

		// If the point is the root return the root
		if (CompareVector(point, root->point)) return root->index;

		unsigned cd = depth % point.size();

		if (point[cd] < root->point[cd])
		{
			return FindValueRec(root->left, point, depth + 1);
		}

		return FindValueRec(root->right, point, depth + 1);
	}

	static int FindValue(std::shared_ptr<Node> root, std::vector<float>& point)
	{
		return FindValueRec(root, point, 0);
	}

	static int NearestNeighborRec(std::shared_ptr<Node> root, std::vector<float>& point, unsigned depth, float& best, int& index)
	{
		if (root == nullptr) return index;

		int cd = depth % point.size();

		float test = 0.0f;
		// Get the distance from the point to the root in k dimentional space
		for (int i = 0; i < point.size(); i++)
		{
			test += pow((root->point[i] - point[i]), 2);
		}
		test = sqrt(test);

		if (test < best && !CompareVector(root->point, point))
		{
			best = test;
			index = root->index;
		}

		// Decide on the next node to search
		if (point[cd] < root->point[cd])
		{
			index = NearestNeighborRec(root->left, point, depth + 1, best, index);
		}
		else
		{
			index = NearestNeighborRec(root->right, point, depth + 1, best, index);
		}

		// If there is no chance of a closer point the other way return
		if (best < abs(point[cd] - root->point[cd]))
		{
			return index;
		}

		// Check the other direction for a closer point
		//if (point[cd] >= root->point[cd])
		//{
		//	index = NearestNeighborRec(root->left, point, depth + 1, best, index);
		//}
		//else
		//{
		//	index = NearestNeighborRec(root->right, point, depth + 1, best, index);
		//}

		return index;
	}

	static int NearestNeighbor(std::shared_ptr<Node> root, std::vector<float>& point)
	{
		float best = 100000.0f;
		int index = 0;

		index = NearestNeighborRec(root, point, 0, best, index);

		return index;
	}
}