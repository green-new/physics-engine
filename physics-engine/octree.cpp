//#include "octree.hpp"
//#include "types.hpp"
//#include "coordinator.hpp"
//#include "components.hpp"
//#include <stack>
//
//extern Coordinator gCoordinator;
//
//using namespace Physics;
//
//void NodeData::insert(const Entity entity) { 
//	m_entities.push_back(entity);
//}
//void NodeData::insert(const std::vector<Entity>& entities) {
//	for (const Entity entity : entities) { 
//		insert(entity);
//	}
//}
//bool NodeData::remove(const Entity entity) {
//	std::vector<Entity>::iterator it;
//	for (it = m_entities.begin(); it != m_entities.end() && *it != entity; it++);
//	return (it != m_entities.end()) && (m_entities.erase(it) != m_entities.end());
//}
//
//bool NodeData::remove(const std::vector<Entity>& entities) {
//	bool flag = true;
//	for (Entity entity : entities) {
//		flag = flag && remove(entity);
//	}
//	return flag;
//}
//
//void NodeData::removeAll() {
//	m_entities.clear();
//}
//
//// Gets unique pairs based on the specified predicate.
//std::set<EntityCollisionPair> NodeData::getUniquePairs(std::function<bool(const Entity& A, const Entity& B)> predicate) const {
//	std::set<EntityCollisionPair> candidates;
//
//	for (unsigned int i = 0; i < m_entities.size() - 1; i++) {
//		for (unsigned int j = i + 1; j < m_entities.size(); j++) {
//			if (predicate(m_entities[i], m_entities[j])) {
//				candidates.insert({ m_entities[i], m_entities[j] });
//			}
//		}
//	}
//
//	return candidates;
//}
//
//// Returns true if this node contains this entity. Returns false if not.
//bool NodeData::contains(const Entity &entity) {
//	return (std::find(m_entities.begin(), m_entities.end(), entity) != m_entities.end());
//}
//
//// Create a new child at the specified index, with a region, and list of initial entities.
//void TreeNode::createChild(unsigned int index, const BoundingBox& region, const std::vector<Entity>& entity) {
//	m_childrenUsedMask |= (1 << index);
//	m_children[index] = new TreeNode(this, region, entity);
//}
//
//// Get the octlets for this tree node based on our current region size.
//std::array<BoundingBox, 8> TreeNode::getOctlets() const {
//	glm::vec3 c = m_data.m_region.center();
//	std::array<BoundingBox, 8> octlets{};
//	const glm::vec3 min = m_data.m_region.min;
//	const glm::vec3 max = m_data.m_region.max;
//	for (unsigned char i = 0; i < 8; i++) {
//		octlets[i].min.x = (i & 1) ? c.x : min.x;
//		octlets[i].min.y = (i & 2) ? c.y : min.y;
//		octlets[i].min.z = (i & 4) ? c.z : min.z;
//
//		octlets[i].max.x = (i & 1) ? max.x : c.x;
//		octlets[i].max.y = (i & 2) ? max.y : c.y;
//		octlets[i].max.z = (i & 4) ? max.z : c.z;
//	}
//
//	return octlets;
//}
//
//void TreeNode::split() {
//	std::array<BoundingBox, 8> octlets = getOctlets();
//	std::array<std::vector<Entity>, 8> entitiesToMove{ {} };
//	std::vector<Entity> delist;
//	for (unsigned int i = 0; i < 8; i++) {
//		for (const Entity& entity : m_data.getEntities()) {
//			const auto& region = gCoordinator.getComponent<Components::RigidBody>(entity).Region;
//			if (region.overlaps(octlets[i])) {
//				entitiesToMove[i].push_back(entity);
//			}
//		}
//	}
//
//	for (unsigned int i = 0; i < 8; i++) {
//		if (!entitiesToMove[i].empty()) {
//			if (VALID_CHILD(i, m_childrenUsedMask)) {
//				m_children[i]->insert(entitiesToMove[i]);
//			} else {
//				createChild(i, octlets[i], entitiesToMove[i]);
//			}
//		}
//	}
//
//	// All entities should be removed by definition
//	m_data.remove(delist);
//}
//
//void TreeNode::insert(const Entity &entity) {
//	m_data.insert(entity);
//}
//
//void TreeNode::insert(const std::vector<Entity>& entities) {
//	m_data.insert(entities);
//}
//
//bool TreeNode::remove(const Entity &entity) {
//	return m_data.remove(entity);
//}
//
//bool TreeNode::remove(const std::vector<Entity>& entities) {
//	return m_data.remove(entities);
//}
//
//void CollisionTree::rebuildTree() {
//	std::vector<Entity> entities;
//	CollisionTreeFactory factory{ entities };
//	
//	// Get all entities
//	transverseTree(
//		[&factory](TreeNode* p) {
//			factory.addEntity(p->m_data.getEntities());
//		},
//		[](TreeNode* p) {
//			return !p->m_data.isEmpty();
//		}
//	);
//
//	delete m_root;
//	m_root = factory.getTree();
//}
//
//void CollisionTree::transverseTree(TransverseFunction func) {
//	transverseTree(func, [](TreeNode *p) { return true; });
//}
//
//void CollisionTree::transverseTree(TransverseFunction func, TreePredicate predicate) {
//	std::stack<TreeNode*> stack;
//	stack.push(m_root);
//
//	while (!stack.empty()) {
//		TreeNode* p = stack.top();
//		stack.pop();
//
//		if (predicate(p)) 
//			func(p);
//
//		for (unsigned int i = 0; i < 8; i++) {
//			if (VALID_CHILD(i, p->m_childrenUsedMask)) {
//		 		stack.push(p->m_children[i]);
//			}
//		}
//	}
//}
//
//std::set<EntityCollisionPair> CollisionTree::broadPhase() {
//	std::set<EntityCollisionPair> candidates;
//
//	broadPhase(*m_root, candidates);
//
//	return candidates;
//}
//
//void CollisionTree::broadPhase(const TreeNode &node, std::set<EntityCollisionPair> &candidates) {
//	transverseTree([&candidates](TreeNode* p) {
//		auto overlap = [](const Entity &A, const Entity &B) {
//			/* These are basically what we would call "colliders" so we should change these in the future rather than entity IDs.
//			Technically entity IDs are too much information to know. We just need the collider information of collidable entities */
//			/* Also make sure that these boxes are world-coordinate specific, or our overlap function won't detect it (problematic) */
//			const auto& regionA = gCoordinator.getComponent<Components::RigidBody>(A).Region;
//			const auto& regionB = gCoordinator.getComponent<Components::RigidBody>(B).Region;
//			return (regionA.overlaps(regionB));
//		};
//		std::set<EntityCollisionPair> overlaps = p->m_data.getUniquePairs(overlap);
//		candidates.insert(overlaps.begin(), overlaps.end());
//	}, 
//		// Only bother to look through nodes that more than 2 entities.
//		[](TreeNode* p) { return /* ->m_isLeaf && */ p->m_data.getEntityCount() >= 2; }
//	);
//}
//
//// Insert a specified (assumes existing) entity into the collision tree.
//void CollisionTree::insert(const Entity &entity) {
//	std::set<TreeNode*> nodesToInsert{};
//	transverseTree([&nodesToInsert, &entity](TreeNode *p) {
//		nodesToInsert.insert(p);
//		}, [&entity](TreeNode *p) {
//			const auto& region = gCoordinator.getComponent<Components::RigidBody>(entity).Region;
//			return /* p->isLeaf() && */ p->m_data.getRegion().overlaps(region);
//		});
//	for (TreeNode* node : nodesToInsert) {
//		node->m_data.insert(entity);
//	}
//	if (nodesToInsert.empty()) {
//		// No space available, rebuild and add the entity
//		rebuildTree();
//		insert(entity);
//	}
//}
//
//// Remove the specified entity from all appearences in the collision tree.
//void CollisionTree::remove(const Entity &entity) {
//	std::set<TreeNode*> nodesToRemove{};
//	transverseTree([&nodesToRemove, &entity](TreeNode* p) {
//		nodesToRemove.insert(p);
//		}, [&entity](TreeNode* p) { 
//			const auto& region = gCoordinator.getComponent<Components::RigidBody>(entity).Region;
//			return /* p->isLeaf() && */ p->m_data.getRegion().overlaps(region);
//		}
//	);
//	for (TreeNode* node : nodesToRemove) {
//		node->m_data.remove(entity);
//	}
//}
//
//// Search through the tree and find nodes that contain this entity. Returns a set
//// of unique tree nodes.
//std::set<TreeNode&> CollisionTree::search(const Entity &entity) {
//	std::set<TreeNode&> nodes;
//	transverseTree([&nodes, &entity](TreeNode* p) {
//		if (p->m_data.contains(entity)) {
//			nodes.insert(*p);
//		}
//	});
//	return nodes;
//}
//
//// Schedules the tree node for death.
//void CollisionTree::scheduleForDeath(TreeNode &node) {
//	m_scheduledForDeletion.insert(node);
//	node.m_markedForDeath = true;
//}
//
//void CollisionTree::killInActiveNodes() {
//	// Check if they still have no entities after previous physics tick
//	// If they are still empty, then delete them.
//	for (auto it = m_scheduledForDeletion.begin(); it != m_scheduledForDeletion.end(); it++) {
//		TreeNode &node = *it;
//		if (node.m_markedForDeath && node.m_data.isEmpty() && node.m_isLeaf) {
//			/* Work on this */
//			delete &node;
//		} else {
//			node.m_markedForDeath = false;
//			node.m_timeInActive = 0;
//			m_scheduledForDeletion.erase(it);
//		}
//	}
//}
//
//void CollisionTree::update() {
//	// If nodes are still inactive, then kill them
//	killInActiveNodes();
//
//	// Update inactivity based on entity count.
//	transverseTree(
//		[this](TreeNode* p) {
//			if (p->m_timeInActive > TreeNode::MAX_TTL) {
//				scheduleForDeath(*p);
//			} else {
//				p->m_timeInActive++;
//			}
//		},
//		[](TreeNode* p) { return /* p->m_isLeaf &&  */ p->m_data.isEmpty(); }
//	);
//}
//
//// Calculates the root space for the collision tree.
//// Always centered at 0.0, 0.0, 0.0. Finds the next largest power of 2 sized bounding box
//// that encapsulates at least the furthest entity (all entities initially entered into the tree).
//BoundingBox CollisionTreeFactory::TreeConstructionPhase::rootSpace(const std::vector<Entity> &initial) {
//	static const glm::vec3 center = glm::vec3(0.0f, 0.0f, 0.0f);
//	int axis = 1;
//	BoundingBox enclosing;
//	for (const Entity &entity : initial) {
//		const auto& regionA = gCoordinator.getComponent<Components::RigidBody>(entity).Region;
//		enclosing = BoundingBox(-axis, -axis, -axis, axis, axis, axis);
//		enclosing.offset(center);
//		if (!regionA.overlaps(enclosing)) {
//			axis <<= 1;
//		}
//	}
//	return enclosing;
//}
//
//void CollisionTreeFactory::TreeConstructionPhase::build(TreeNode& node) {
//	if (node.m_data.getEntityCount() <= TreeNode::MAX_ENTITIES ||
//	node.m_data.getRegion().smallerOrAt(TreeNode::MIN_REGION))  {
//		return;
//	}
//	node.split();
//	for (unsigned int i = 0; i < 8; i++) {
//		if (VALID_CHILD(i, node.m_childrenUsedMask)) {
//			build(*node.m_children[i]);
//		}
//	}
//}
//
//TreeNode* CollisionTreeFactory::TreeConstructionPhase::buildTree() {
//	if (m_entities.empty()) {
//		return;
//	}
//	m_root->m_data.insert(m_entities);
//	build(*m_root);
//	return m_root;
//}