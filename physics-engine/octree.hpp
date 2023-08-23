#pragma once
#include "box.hpp"
#include <queue>

class Octree {
private:
	BoundingBox m_region;
	std::vector</* Collider class */> m_colliders;
	std::queue</* Collider class */> m_pending;
	std::array<Octree, 8> m_children;

	Octree parent;
};