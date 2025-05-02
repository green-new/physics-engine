//#pragma once
//#include "box.hpp"
//#include "gjk.hpp"
//#include "types.hpp"
//#include <set>
//#include <functional>
//#include <vector>
//
//#define VALID_CHILD(i, mask) (mask & (1 << i))
//
//namespace Physics {
//
//	// Author: green
//	// Contains a pair of two unique entities used for broad phase collision detection.
//	// Used in the broad phase to find out if these pairs are really colliding or not.
//	class EntityCollisionPair {
//	public:
//		EntityCollisionPair(Entity P, Entity Q) {
//			if (P < Q) {
//				this->P = P;
//				this->Q = Q;
//			} else if (P > Q) {
//				this->P = Q;
//				this->Q = P;
//			} else if (P == Q) {
//				throw std::exception("Collision candidate pair cannot have 2 of the same entities");
//			}
//		}
//		~EntityCollisionPair() = default;
//		EntityCollisionPair& operator=(const EntityCollisionPair& rhs) {
//			P = rhs.P;
//			Q = rhs.Q;
//			return *this;
//		}
//		bool operator==(EntityCollisionPair const& rhs) {
//			return ((P == rhs.P || P == rhs.Q) && (Q == rhs.P || Q == rhs.Q));
//		}
//		const Entity& getP() { return P; }
//		const Entity& getQ() { return Q; }
//	private:
//		Entity P;
//		Entity Q;
//	};
//
//	// Author: green
//	// NodeData contains the critical information with each tree node. 
//	// Currently contains the bounding region and the list of entities.
//	class NodeData {
//		friend TreeNode;
//	public:
//		NodeData() : m_entities(), m_region() { }
//		NodeData(const std::vector<Entity>& entities, BoundingBox region) : m_entities(entities), m_region(region) {}
//		NodeData(const std::initializer_list<Entity>& entities, BoundingBox region) : m_entities(entities), m_region(region) {}
//		~NodeData() = default;
//
//		bool			isEmpty() const	{ return m_entities.empty(); }
//		const			BoundingBox& getRegion() const { return m_region; }
//		unsigned int	getEntityCount() const { return m_entities.size(); }
//		void			insert(const Entity entity);
//		void			insert(const std::vector<Entity>& entities);
//		bool			remove(const Entity entity);
//		bool			remove(const std::vector<Entity>& entities);
//		void			removeAll();
//		bool			contains(const Entity& entity);
//		const std::vector<Entity>&				
//						getEntities() const { return m_entities; }
//		std::vector<Entity>&					
//						getEntities() { return m_entities; }
//		std::set<Physics::EntityCollisionPair>	
//						getUniquePairs(std::function<bool(const Entity& A, const Entity& B)> predicate) const;
//	private:
//		std::vector<Entity>	m_entities{};
//		const BoundingBox	m_region{};
//	};
//	
//	// Author: green
//	// The tree node is the basic class behind the collision tree.
//	// Has 8 children and has pointers to each one. Has a mask to determine if that
//	// child is a null pointer or not and internally handles invalid children if they are called.
//	class TreeNode {
//		friend CollisionTree;
//		friend CollisionTreeFactory;
//	private:
//		void createChild(unsigned int index, const BoundingBox& region, const std::vector<Entity>& entity);
//	public:
//		TreeNode(TreeNode *parent, const BoundingBox region, const std::vector<Entity>& entities) : 
//			m_isLeaf(true), 
//			m_childrenUsedMask(0), 
//			m_children(), 
//			m_parent(parent), 
//			m_data(entities, region),
//			m_timeInActive(0), 
//			m_markedForDeath(false) { }
//		~TreeNode() {
//			// Go through and delete the children.
//			// Does this do cascade deletion?
//			for (unsigned int i = 0; i < 8; i++) {
//				if (m_childrenUsedMask & (1 << i)) {
//					delete m_children[i];
//				}
//			}
//			m_parent->m_childrenUsedMask ^= (1 << m_index);
//		}
//		void				insert(const Entity& entity);
//		void				insert(const std::vector<Entity>& entities);
//		bool				remove(const Entity& entity);
//		bool				remove(const std::vector<Entity>& entities);
//		std::array<BoundingBox, 8>	
//							getOctlets() const;
//		void				split();
//		// Number of updates before this node is marked for death if it has no colliders.
//		static constexpr unsigned int		
//							MAX_TTL = 100;
//		// Number of entities allowed until this node splits. Splitting will stop if we've reached the minimum node size.
//		static constexpr unsigned short		
//							MAX_ENTITIES = 3;
//		// The minimum size a tree node's region is allowed to be.
//		static constexpr BoundingBox		
//							MIN_REGION = BoundingBox(0.5f, 0.5f, 0.5f);
//	private:
//		NodeData			m_data{};
//		TreeNode *const		m_parent{};
//		std::array<TreeNode*, 8>	
//							m_children{};
//		unsigned char		m_childrenUsedMask{};
//		bool				m_isLeaf{};
//		size_t				m_timeInActive{};
//		bool				m_markedForDeath{};
//	};
//
//	using TransverseFunction	= std::function<void(TreeNode*)>;
//	using TreePredicate			= std::function<bool(TreeNode*)>;
//
//
//	// Author: green
//	// This collision tree is responsible for partitioning 3D space and keeping track of inactive and active octants,
//	// each containing lists of entities which are within its partitioned space.
//	// This class is used to help the broad phase part of collision detection, and find candidate pairs of collisions
//	// that will be tested farther down the line.
//	// The root space is determined by finding the initially farthest entity and finding a power of 2 axis that encapsulates 
//	// that entity's bounding box/collider, centered on the origin.
//	// A node is allowed to live for 100 update ticks before being removed. Depending on the update rate, this may change
//	// how long it lasts in seconds/milliseconds. Considering 10-20 Hz.
//	// A node is allowed to have at maximum 3 entities before being split.
//	// The minimum size a octant is allowed to be is 0.5 x 0.5 x 0.5. A node is allowed to have as many children as 
//	// possible within this minimum space.
//	// A node after being split sends its entities down into its respective children, depending on overlapping space.
//	// A node with 1 entity will be discarded for collision checks. This means that non-leaf nodes do not contain 
//	// entities, except in a circumstance 1-2 entities are left within an octant and atleast 1 child node has been deleted.
//	// (We need to factor this in. Normally we want the insertion algorithm to prefer putting into leaf nodes, but if 
//	// its not possible it needs to place it in non-leaf, "spacially-terminating" nodes since that entity has no where
//	// else to go. We also need a way for the broad phase algorithm to consider non-leaf nodes containing entities).
//	// An entity with its region overlaping over more than 1 node is counted in each overlapping nodes' entity list.
//	class CollisionTree {
//		friend CollisionTreeFactory;
//	private:
//		void broadPhase(const TreeNode& node, std::set<Physics::EntityCollisionPair>& candidates);
//		void transverseTree(TransverseFunction function, TreePredicate predicate);
//		void transverseTree(TransverseFunction func);
//		void rebuildTree();
//		void scheduleForDeath(TreeNode &node);
//		void killInActiveNodes();
//	public:
//		CollisionTree(TreeNode *root) : m_root(root) { }
//		CollisionTree(const CollisionTree& other) : m_root(other.m_root) { }
//		~CollisionTree() {
//			// Cascade deletion
//			delete m_root;
//		}
//
//		std::set<Physics::EntityCollisionPair> 
//								broadPhase();
//		void					insert(const Entity &entity);
//		void					remove(const Entity &entity);
//		std::set<TreeNode&>		search(const Entity &entity);
//		void					update();
//	private:
//		TreeNode				*m_root;
//		std::set<TreeNode&>		m_scheduledForDeletion;
//	};
//
//	class CollisionTreeFactory {
//	private:
//
//		class TreeConstructionPhase {
//		public:
//			TreeConstructionPhase(std::vector<Entity> entities) :
//				m_entities(entities),
//				m_root(new TreeNode(nullptr, rootSpace(entities), m_entities)) { }
//			~TreeConstructionPhase() = default;
//
//			void					add(const Entity &entity) { m_entities.push_back(entity); }
//			void					addAll(const std::vector<Entity> &entities) { for (const Entity& entity : entities) { add(entity); } }
//			BoundingBox				rootSpace(const std::vector<Entity>& initial);
//			TreeNode*				buildTree();
//		private:
//			TreeNode				*m_root;
//			std::vector<Entity>		m_entities;
//			void					build(TreeNode &node);
//		};
//
//		std::unique_ptr<TreeConstructionPhase> m_construction;
//
//	public:
//		CollisionTreeFactory(const std::vector<Entity>& entities) 
//			: m_construction(std::make_unique<TreeConstructionPhase>(entities)) { }
//		~CollisionTreeFactory() { }
//
//		void		addEntity(const Entity& entity) { m_construction->add(entity); }
//		void		addEntity(const std::vector<Entity>& entities) { m_construction->addAll(entities); }
//		TreeNode*	getTree() { return m_construction->buildTree(); }
//	};
//}