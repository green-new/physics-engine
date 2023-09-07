#pragma once
#include "box.hpp"
#include "gjk.hpp"
#include <queue>
#include <stack>
#include <algorithm>

namespace Physics {

	class NodeData {
	private:
		std::vector<std::shared_ptr<Collider>>	m_colliders;
		const BoundingBox m_region;
	public:
		NodeData()																					: m_colliders(), m_region() { }
		NodeData(std::initializer_list<std::shared_ptr<Collider>> colliders, BoundingBox region)	: m_colliders(colliders), m_region(region) {}
		NodeData(std::vector<std::shared_ptr<Collider>> colliders, BoundingBox region)				: m_colliders(colliders), m_region(region) {}
		~NodeData() {}

		bool				isEmpty() const	{ return m_colliders.empty(); }
		const BoundingBox&	getRegion()	const { return m_region; }
		unsigned int		getColliderCount() const { return m_colliders.size(); }
		void				add(const std::shared_ptr<Collider>& collider) { m_colliders.push_back(collider); }
		void				addAll(const std::vector<std::shared_ptr<Collider>>& colliders) { for (auto it = colliders.begin(); it != colliders.end(); ++it) { m_colliders.push_back(*it); } }
		void				remove(const Collider& collider) { 
			std::remove_if(m_colliders.begin(), m_colliders.end(), [&collider](const Collider& other)
				{ return &collider == &other; }); 
		}

		friend CollisionTree;
		friend TreeNode;
	};

	class TreeNode {
	private:
		NodeData										m_data;
		const std::shared_ptr<TreeNode>					m_parent;
		std::array<std::shared_ptr<TreeNode>, 8>		m_children;

		unsigned char									m_childrenUsedMask;
		bool											m_isLeaf;
	public:
		TreeNode(const std::shared_ptr<TreeNode> parent, const BoundingBox region, const std::vector<std::shared_ptr<Collider>>& colliders) : 
			m_isLeaf(true), 
			m_childrenUsedMask(0), 
			m_children({}), 
			m_parent(parent), 
			m_data(colliders, region) { }
		~TreeNode() {}

		NodeData& getData() { return m_data; }

		void split() {
			std::array<BoundingBox, 8> octletRegions = m_data.getRegion().octlets();
			std::array<std::vector<std::shared_ptr<Collider>>, 8> childrenColliders{ {} };
			std::vector<std::shared_ptr<Collider>> delist{};	// Colliders to remove from this node later on
			unsigned char usedChildren = 0;
			const std::shared_ptr<TreeNode> me = std::make_shared<TreeNode>(this);
			for (unsigned int i = 0; i < 8; i++) {
				const BoundingBox& region = octletRegions[i];

				// Find the region overlaps for each collider in each collider
				for (auto it = m_data.m_colliders.begin(); it != m_data.m_colliders.end(); ++it) {
					if ((*it).get()->getRegion().overlaps(region)) {
						delist.push_back(*it);
						childrenColliders[i].push_back(*it);
						usedChildren |= (1 << i);
					}
				}
			}

			// Create the children or add them to existing ones
			for (unsigned int i = 0; i < 8; i++) {
				if (usedChildren & (1 << i)) {
					m_children[i]->getData().addAll(childrenColliders[i]);
				}
				else {
					const BoundingBox& region = octletRegions[i];
					m_children[i] = std::make_shared<TreeNode>(new TreeNode(me, region, childrenColliders[i]));
				}
			}

			// Removing the elements from this node. An object can exist in 8 octlets at maximum
			for (auto it = delist.begin(); it != delist.end(); ++it) {
				std::remove_if(m_data.m_colliders.begin(), m_data.m_colliders.end(), [&it](const std::shared_ptr<Collider> collider) {
					return *it == collider;
					});
			}
		}

		std::shared_ptr<TreeNode> getChild(unsigned i) {
			assert(i >= 0 && i < 8, "Invalid index for child (must be between 0 and 8)");
			return m_children[i];
		}

		friend NodeData;
		friend CollisionTree;
	};

	struct CollisionPair {
		const std::shared_ptr<Collider>& colliderA;
		const std::shared_ptr<Collider>& colliderB;
	};

	class CollisionTree {
	private:
		const std::shared_ptr<TreeNode> m_root;

		void broadPhase(const std::shared_ptr<TreeNode>& node, std::vector<CollisionPair>& candidates) {
			NodeData& data = node->getData();

			// Overlap check
			for (const std::shared_ptr<Collider>& A : node->getData().m_colliders) {
				for (const std::shared_ptr<Collider>& B : node->getData().m_colliders) {
					if (A != B && A->getRegion().overlaps(B->getRegion())) {
						candidates.push_back(CollisionPair{A, B});
					}
				}
			}

			if (!node->m_isLeaf) {
				for (unsigned int i = 0; i < 8; i++) {
					if ((node->m_childrenUsedMask & (1 << i)) != 1) {
						continue;
					}
					std::shared_ptr<TreeNode>& child = node->m_children[i];
					broadPhase(child, candidates);
				}
			}
		}

	public:
		CollisionTree(std::shared_ptr<TreeNode> root) : m_root(root) { }
		~CollisionTree() {}

		std::vector<CollisionPair> broadPhase() {
			std::vector<CollisionPair> candidates;

			broadPhase(m_root, candidates);

			return candidates;
		}

		friend NodeData;
		friend TreeNode;
	};

	class CollisionTreeFactory {
	private:
		class FactoryPhase {
		public:
			virtual std::shared_ptr<FactoryPhase> advance() = 0;
			virtual ~FactoryPhase() {}
		};

		class TreeFinalizationPhase : public FactoryPhase {
		private:
			std::unique_ptr<CollisionTree> m_tree;
		public:
			TreeFinalizationPhase(std::shared_ptr<TreeNode> root) : m_tree(std::make_unique<CollisionTree>(root)) { }
			~TreeFinalizationPhase() {}
			std::shared_ptr<FactoryPhase> advance() override { return std::make_unique<TreeFinalizationPhase>(this);  }
			std::unique_ptr<CollisionTree> getTree() { return std::move(m_tree); }
		};

		class TreeConstructionPhase : public FactoryPhase {
		private:
			std::shared_ptr<TreeNode>				m_root;
			std::vector<std::shared_ptr<Collider>>	m_colliders;

			static constexpr unsigned short			maxColliders = 3; /* number of entities allowed until the octant is split */
		public:
			TreeConstructionPhase(std::vector<std::shared_ptr<Collider>> colliders) :
				m_colliders(colliders),
				m_root(std::make_shared<TreeNode>(new TreeNode(nullptr, rootSpace(m_colliders), m_colliders))) { }
			~TreeConstructionPhase() {}

			BoundingBox rootSpace(const std::vector<std::shared_ptr<Collider>>& initial) {
				int axis = 1;
				BoundingBox enclosing;
				glm::vec3 center = glm::vec3(0.0f, 0.0f, 0.0f);
				for (auto it = initial.begin(); it != initial.end(); ++it) {
					Collider& collider = *(*it).get();
					enclosing = BoundingBox(-axis, -axis, -axis, axis, axis, axis);
					enclosing.offset(center);
					if (!collider.getRegion().overlaps(enclosing)) {
						axis <<= 1;
					}
				}
				return enclosing;
			}
			void build(std::shared_ptr<TreeNode> node) {
				static BoundingBox minimumRegion = BoundingBox(-0.1f, -0.1f, -0.1f, 0.1f, 0.1f, 0.1f);
				if (node.get()->getData().getColliderCount() <= maxColliders ||
					node.get()->getData().getRegion().smallerOrAt(minimumRegion)) {
					return;
				}

				node.get()->split();

				for (unsigned int i = 0; i < 8; i++) {
					build(node.get()->getChild(i));
				}
			}

			std::shared_ptr<FactoryPhase> advance() override {
				if (m_colliders.empty()) return;

				m_root.get()->getData().addAll(m_colliders);

				build(m_root);

				return std::make_shared<TreeFinalizationPhase>(m_root);
			}
		};
		class ColliderCollectionPhase : public FactoryPhase {
		private:
			std::queue<std::shared_ptr<Collider>> m_pending;
		public:
			ColliderCollectionPhase() {}
			~ColliderCollectionPhase() {}

			void pushAll(const std::vector<std::shared_ptr<Collider>>& colliders) { for (auto it = colliders.begin(); it != colliders.end(); ++it) { m_pending.push(*it); } }
			void pushAll(const std::initializer_list<std::shared_ptr<Collider>>& colliders) { pushAll(std::vector<std::shared_ptr<Collider>>(colliders)); }
			void push(const std::shared_ptr<Collider>& collider) { m_pending.push(collider); }
			void pop() { m_pending.pop(); }
			std::shared_ptr<FactoryPhase> advance() override {
				std::vector<std::shared_ptr<Collider>> initial;
				while (!m_pending.empty()) {
					initial.push_back(m_pending.front());
					m_pending.pop();
				}
				return std::make_shared<TreeConstructionPhase>(initial);
			}
		};

		FactoryPhase* phase;

	public:
		CollisionTreeFactory() : phase(new ColliderCollectionPhase) { }
		~CollisionTreeFactory() { }

		void addPending(std::shared_ptr<Collider> collider) {
			((ColliderCollectionPhase*)phase)->push(collider);
		}

		void addPending(std::vector<std::shared_ptr<Collider>> colliders) {
			((ColliderCollectionPhase*)phase)->pushAll(colliders);
		}

		std::shared_ptr<FactoryPhase> advance() {
			return phase->advance();
		}

		std::unique_ptr<CollisionTree> finalizeTree() {
			return ((TreeFinalizationPhase*)phase)->getTree();
		}
	};
}