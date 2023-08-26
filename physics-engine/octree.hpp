#pragma once
#include "box.hpp"
#include "gjk.hpp"
#include <queue>
#include <algorithm>

namespace Physics {
	/* octree containing entities that could be colliding */
	class CollisionTree {
	private:
		BoundingBox m_region;
		CollisionTree* m_parent;
		std::vector<Physics::Collider> m_colliders;
		std::queue<Physics::Collider> m_pending;
		std::array<CollisionTree*, 8> m_children;
		unsigned char m_used;
		unsigned short updatesWhileEmpty = 0;

		bool m_isLeaf;
		
		/* number of entities allowed until the octant is split */
		static constexpr unsigned short maxEntities = 3;

		/* number of updates allowed for an octant to live if there are no entities in it */
		static constexpr unsigned short updatesUntilDeath = 8;
	public:
		CollisionTree() : m_region(0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f),
			m_colliders(),
			m_pending(), 
			m_children(), 
			m_parent(),
			m_isLeaf(true),
			m_used(0) {}

		CollisionTree(BoundingBox region, std::vector<Physics::Collider> colliders, CollisionTree* parent) : m_region(region), 
			m_colliders(colliders), 
			m_pending(), 
			m_children(), 
			m_parent(parent),
			m_isLeaf(true),
			m_used(0) {}
		
		~CollisionTree() {
			// Cascade deletion
			delete m_children[0]; 
			delete m_children[1];
			delete m_children[2];
			delete m_children[3];
			delete m_children[4];
			delete m_children[5];
			delete m_children[6];
			delete m_children[7];
		}

		BoundingBox& getRegion() {
			return m_region;
		}

		void update() {
			// Empty the queue
			while (!m_pending.empty()) {
				auto& collider = m_pending.front();
				m_colliders.push_back(collider);
				m_pending.pop();
			}
			// Get the enclosing cube before building
			solveInitialSpace();

			build();
		}

		void addPending(const Physics::Collider& object) {
			m_pending.push(object);
		}

		// Move collider from this to child
		void moveInto(Physics::Collider& collider, unsigned char index) {
			m_used |= (unsigned char)(1 << index);
			m_children[index]->m_colliders.push_back(std::move(collider));
		}

		/* ONLY WORKS moving colliders down 1 level */
		void moveCollidersDown() {
			// Check which regions our colliders are in
			for (auto it = m_colliders.begin(); it != m_colliders.end(); ++it) {
				for (unsigned char i = 0; i < 8; i++) {
					CollisionTree* child = m_children[i];

					// Move our collider down the node
					if ((*it).getRegion().overlaps(child->getRegion())) {
						moveInto(*it, i);

						// if we have too many entities, split
						if (child->m_colliders.size() > maxEntities) {
							child->build();
						}
					}
				}
			}
		}

		/* get the largest power of 2 cube enclosing entities down this node (primarily used with the root node) */
		void solveInitialSpace() {

			// Must be root node or we get weird octants
			if (m_parent != nullptr) {

				// See if there are any colliders not within the current region
				bool orphansExist = false;
				for (auto& collider : m_colliders) {
					if (!collider.getRegion().overlaps(m_region)) {
						orphansExist = true;
						break;
					}
				}

				if (orphansExist) {
					signed int min = -1, max = 1;
					// 0, 0, 0 should be the center (world coordinates)
					BoundingBox enclosingCube(min, min, min, max, max, max);
					// TODO: List vs vector here
					std::list<Physics::Collider&> dummy(m_colliders.begin(), m_colliders.end());
					while (!dummy.empty()) {
						enclosingCube.min = glm::vec3(min, min, min); enclosingCube.max = glm::vec3(max, max, max);
						dummy.remove_if([enclosingCube](Physics::Collider c) {
							c.getRegion().overlaps(enclosingCube);
							});
						min << 1;			// Check if endianness matters on other machines
						max << 1;
					}
					m_region = enclosingCube;
				}
			}
		}

		void createChildren() {
			// True center of the bounding box
			glm::vec3 center = m_region.center();

			// Utilize the heap and not the stack because our octree is probably gonna be huge
			for (unsigned char i = 0; i < 8; i++) {
				m_children[i] = new CollisionTree(BoundingBox(), std::vector<Physics::Collider>(), this);

				// We aren't a leaf anymore
				m_isLeaf = false;

				// Test and debug (thanks chatgpt)
				m_children[i]->m_region.min = glm::vec3(
					(i & 1) ? center.x : m_region.min.x,
					(i & 2) ? center.y : m_region.min.y,
					(i & 4) ? center.z : m_region.min.z
				);
				m_children[i]->m_region.max = glm::vec3(
					(i & 1) ? m_region.max.x : center.x,
					(i & 2) ? m_region.max.y : center.y,
					(i & 4) ? m_region.max.z : center.z
				);
			}
		}

		/* Recursive function that builds the tree from the current position */
		void build() {
			// Base case
			if (m_colliders.size() <= maxEntities) {
				return;
			}

			createChildren();

			moveCollidersDown();
		}

		/* Start at the root node and insert the collider */
		/* ONLY WORKS moving colliders down 1 level */
		void insertAtRoot(Physics::Collider* collider) {
			if (m_parent != nullptr) {
				if (m_isLeaf) {
					createChildren();
				}
				for (unsigned char i = 0; i < 8; i++) {
					auto child = m_children[i];
					if (child->m_region.overlaps(collider->getRegion())) {
						// Only insert if we are a leaf (smallest subregion that contains this region)
						if (m_isLeaf) {
							m_children[i]->m_colliders.push_back(*collider);
						}
						insertAtRoot(collider);
					}
				}
			}
		}

		/* start at any node and find where to insert the collider */
		/* ONLY WORKS moving colliders down 1 level */
		void insertAtNode(Physics::Collider* collider) {
			if (m_isLeaf) {
				createChildren();
			}
			for (unsigned char i = 0; i < 8; i++) {
				auto child = m_children[i];
				if (child->m_region.overlaps(collider->getRegion())) {
					// Only insert if we are a leaf
					if (m_isLeaf) {
						m_children[i]->m_colliders.push_back(*collider);
					}
					insertAtRoot(collider);
				}
			}

			// Base case
			return;
		}
	};
}