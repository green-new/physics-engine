#pragma once
#include "box.hpp"
#include "gjk.hpp"
#include <queue>
#include <algorithm>

namespace Physics {
	/* Octree containing entities that could be colliding. */
	class CollisionTree {
	private:
		BoundingBox m_region;
		CollisionTree* m_parent;
		std::vector<Physics::Collider> m_colliders;
		std::queue<Physics::Collider> m_pending;
		std::array<CollisionTree, 8> m_children;
		unsigned char m_used;

		bool isLeaf;
		
		/* number of entities allowed until the octant is split */
		static constexpr unsigned short maxEntities = 3;

		/* number of updates are allowed for an octant to live if there are no entities in it */
		static constexpr unsigned short framesToLive = 8;
	public:
		CollisionTree() : m_region(0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f),
			m_colliders(),
			m_pending(), 
			m_children(), 
			m_parent() {}
		CollisionTree(BoundingBox region, std::vector<Physics::Collider> colliders, CollisionTree* parent) : m_region(region), 
			m_colliders(colliders), 
			m_pending(), 
			m_children(), 
			m_parent(parent) {}
		~CollisionTree();
		BoundingBox& getRegion() {
			return m_region;
		}
		void addPending(const Physics::Collider& object) {
			m_pending.push(object);
		}
		void update() {
			// Empty the queue
			while (!m_pending.empty()) {
				auto& collider = m_pending.front();
				m_colliders.push_back(collider);
				m_pending.pop();
			}
			build();
		}
		/* Recursive function that builds the tree from the current colliders */
		void build() {
			// Base case (do not split if we have less than or equal to 1 entity)
			if (m_colliders.size() <= 1) {
				return;
			}

			glm::vec3 length = m_region.max - m_region.min;

			// If first pass, we need to find the enclosing cube of all entities
			if (length == glm::zero<glm::vec3>()) {
				signed int min = -1, max = 1;
				BoundingBox enclosingCube(min, min, min, max, max, max);
				std::list<Physics::Collider&> dummy(m_colliders.begin(), m_colliders.end());
				while (!dummy.empty()) {
					enclosingCube.min = glm::vec3(min, min, min); enclosingCube.max = glm::vec3(max, max, max);
					dummy.remove_if([enclosingCube](Physics::Collider c) {
						c.getRegion().overlaps(enclosingCube);
						});
					/* Does endianness here matter? */
					min << 1;
					max << 1;
				}
				m_region = enclosingCube;
			}

			// Find center
			
			glm::vec3 center = length / 2.0f;

			// 8 regions
			CollisionTree children[8];

			// Initialize first
			for (unsigned char i = 0; i < 8; i++) {
				children[i] = CollisionTree(BoundingBox(), std::vector<Physics::Collider>(), this);
			}

			// X -> (left + / right -)
			// Y ^ (up + / down -)
			// Z /> (front + / back -)
			//   +--------+
			//  /        /|
			// /        / |
			//+--------+  |
			//|        |  |
			//|        |  +
			//|        | /
			//|        |/
			//+--------+

			// Left-bottom-back
			children[0].m_region.min = m_region.min; children[0].m_region.max = center;
			// Left-top-back
			children[1].m_region.min = glm::vec3(m_region.min.x, center.y, m_region.min.z); children[1].m_region.max = glm::vec3(center.x, m_region.max.y, center.z);
			// Right-bottom-back
			children[2].m_region.min = glm::vec3(center.x, m_region.min.y, m_region.min.z); children[2].m_region.max = glm::vec3(m_region.max.x, center.y, center.z);
			// Right-top-back
			children[3].m_region.min = glm::vec3(center.x, center.y, m_region.min.z); children[3].m_region.max = glm::vec3(m_region.max.x, m_region.max.y, center.z);
			// Left-bottom-front
			children[4].m_region.min = glm::vec3(m_region.min.x, m_region.min.y, center.z); children[4].m_region.max = glm::vec3(center.x, center.y, m_region.max.z);
			// Left-top-front
			children[5].m_region.min = glm::vec3(m_region.min.x, center.y, center.z); children[5].m_region.max = glm::vec3(center.x, m_region.max.y, m_region.max.z);
			// Right-bottom-front
			children[6].m_region.min = glm::vec3(center.x, m_region.min.y, center.z); children[6].m_region.max = glm::vec3(m_region.max.x, center.y, m_region.max.z);
			// Right-top-front
			children[7].m_region.min = center; children[7].m_region.max = m_region.max;

			// Check which regions our colliders are in
			for (auto it = m_colliders.begin(); it != m_colliders.end(); ++it) {
				for (unsigned char i = 0; i < 8; i++) {
					CollisionTree child = children[i];
					/* Move our collider down the node */
					if ((*it).getRegion().overlaps(child.getRegion())) {
						child.m_colliders.push_back(std::move(*it));

						// This means this child has active colliders, mark it as used
						m_used |= (char)(1 << i);
					}
				}
			}

			// Add children nodes to this node and recursively build
			for (unsigned char j = 0; j < 8; j++) {
				if (children[j].m_colliders.size() != 0) {
					m_children[j] = children[j];
					m_children[j].build();
				}
			}
		}
	};
}