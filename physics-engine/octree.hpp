#pragma once
#include "box.hpp"
#include "gjk.hpp"
#include "types.hpp"
#include <set>
#include <functional>
#include <vector>

namespace Physics {

	struct EntityCollisionPair {
		const Entity colliderA;
		const Entity colliderB;
	};

	class NodeData {
	private:
		void sort();

	public:
		NodeData() : m_entities(), m_region() { }
		NodeData(std::vector<Entity> entities, BoundingBox region) : m_entities(entities), m_region(region) {}
		NodeData(std::initializer_list<Entity> entities, BoundingBox region) : m_entities(entities), m_region(region) {}
		~NodeData() = default;

		bool	isEmpty() const	{ return m_entities.empty(); }
		const BoundingBox&	getRegion()	const { return m_region; }
		unsigned int	getColliderCount() const { return m_entities.size(); }
		void	add(const Entity entity);
		void	addAll(const std::vector<Entity> entities);
		void	remove(const Entity entity);
		void	removeAll(const std::vector<Entity> entities);
		void	removeAll();

		const std::vector<Entity>& getEntities() const { return m_entities; }
		std::vector<Entity>& getEntities() { return m_entities; }

		std::set<Physics::EntityCollisionPair> getUniquePairs(std::function<bool(const Entity& A, const Entity& B)> predicate) const;

		friend CollisionTree;
		friend TreeNode;

	private:
		std::vector<Entity>	m_entities;
		const BoundingBox m_region;
	};

	class TreeNode {
	private:
		void createChild(unsigned int index, const BoundingBox& region, const std::vector<Entity>& entity);

	public:
		TreeNode(TreeNode *parent, const BoundingBox region, const std::vector<Entity>& entities) : 
			m_isLeaf(true), 
			m_childrenUsedMask(0), 
			m_children(), 
			m_parent(parent), 
			m_data(entities, region) { }
		~TreeNode() {
			for (unsigned int i = 0; i < 8; i++) {
				delete m_children[i];
			}
		}

		NodeData& getData() { return m_data; }
		const NodeData& getData() const { return m_data; }

		const TreeNode& getChild(unsigned int i) const {
			assert(i >= 0 && i < 8, "Invalid index for child (must be between 0 and 7, inclusive)");
			return *m_children[i];
		}
		TreeNode& getChild(unsigned int i) {
			assert(i >= 0 && i < 8, "Invalid index for child (must be between 0 and 7, inclusive)");
			return *m_children[i];
		}

		void split();

		friend NodeData;
		friend CollisionTree;

	private:
		NodeData		m_data{};
		TreeNode *const m_parent{};
		std::array<TreeNode*, 8> m_children{};

		unsigned char	m_childrenUsedMask{};
		bool			m_isLeaf{};
	};

	class CollisionTree {
	private:
		void broadPhase(const TreeNode& node, std::set<Physics::EntityCollisionPair>& candidates);
		void updateRootSize();
	public:
		CollisionTree(TreeNode *root) : m_root(root) { }
		CollisionTree(const CollisionTree& other) : m_root(other.m_root) { }
		~CollisionTree() {}

		std::set<Physics::EntityCollisionPair> broadPhase();

		void insert(const Entity entity);
		void remove(const Entity entity);

		TreeNode& search(const Entity entity);

		friend NodeData;
		friend TreeNode;
	private:
		TreeNode *const m_root;
	};

	class CollisionTreeFactory {
	private:

		class TreeFinalizationPhase {
		public:
			TreeFinalizationPhase() { }
			~TreeFinalizationPhase() = default;
			std::unique_ptr<CollisionTree> createTreeFromRoot(std::unique_ptr<TreeNode>&& root);
		};

		class TreeConstructionPhase {
		public:
			TreeConstructionPhase(std::vector<Entity> entities) :
				m_entities(entities),
				m_root(std::make_unique<TreeNode>(nullptr, rootSpace(entities), m_entities)) { }
			~TreeConstructionPhase() = default;

			void add(Entity entity) { m_entities.push_back(entity); }
			void addAll(std::vector<Entity> entities) { for (const Entity& entity : entities) { add(entity); } }

			BoundingBox rootSpace(const std::vector<Entity>& initial);

			std::unique_ptr<TreeNode> constructTree();
		private:
			std::unique_ptr<TreeNode>	m_root;
			std::vector<Entity>	m_entities;

			static constexpr unsigned short		maxEntities = 3; /* number of entities allowed until the octant is split */

			void build(TreeNode& node);
		};

		std::unique_ptr<TreeConstructionPhase> m_construction;
		std::unique_ptr<TreeFinalizationPhase> m_finalization;

	public:
		CollisionTreeFactory() : m_construction(std::make_unique<TreeConstructionPhase>()), 
								m_finalization(std::make_unique<TreeFinalizationPhase>()) { }
		~CollisionTreeFactory() { }

		void addPending(Entity entity) {
			m_construction->add(entity);
		}

		void addPending(std::vector<Entity> entities) {
			m_construction->addAll(entities);
		}

		std::unique_ptr<CollisionTree> createTree() {
			return m_finalization->createTreeFromRoot(m_construction->constructTree());
		}
	};
}