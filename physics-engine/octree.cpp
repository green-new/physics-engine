#include "octree.hpp"
#include "types.hpp"
#include "coordinator.hpp"
#include "components.hpp"
#include <stack>

extern Coordinator gCoordinator;

void Physics::NodeData::add(const Entity entity) { 
	m_entities.push_back(entity);

	sort();

}
void Physics::NodeData::addAll(const std::vector<Entity> entities) {
	for (auto it = entities.begin(); it != entities.end(); ++it) { 
		m_entities.push_back(*it); 
	}

	sort();
}
void Physics::NodeData::remove(const Entity entity) {
	std::remove_if(m_entities.begin(), m_entities.end(), 
		[&entity](Entity other) { 
			return entity == other; 
		}
	);

	sort();
}

void Physics::NodeData::removeAll(const std::vector<Entity> entities) {
	for (Entity entity : entities) {
		std::remove_if(m_entities.begin(), m_entities.end(), [&entity](const Entity& other) {
			return entity == other;
		});
	}

	sort();
}

void Physics::NodeData::removeAll() {
	m_entities.clear();
}

void Physics::NodeData::sort() {
	// Insertion sort
	for (unsigned int j = 1; j < m_entities.size(); j++) {
		Entity key = m_entities[j];
		// Insert A[j] into the partial sorted sequence A[1 .. j - 1]
		unsigned i = j - 1;
		while (i >= 0 && m_entities[i] > key) {
			m_entities[i + 1] = m_entities[i];
			--i;
		}
		m_entities[j + 1] = key;
	}
}



std::set<Physics::EntityCollisionPair> Physics::NodeData::getUniquePairs(std::function<bool(const Entity& A, const Entity& B)> predicate) const {
	std::set<Physics::EntityCollisionPair> candidates;

	for (unsigned int i = 0; i < m_entities.size() - 1; i++) {
		for (unsigned int j = i + 1; j < m_entities.size(); j++) {
			if (predicate(m_entities[i], m_entities[j])) {
				candidates.insert({ m_entities[i], m_entities[j] });
			}
		}
	}

	return candidates;
}

void Physics::TreeNode::createChild(unsigned int index, const BoundingBox& region, const std::vector<Entity>& entity) {
	m_children[index] = new Physics::TreeNode(this, region, entity);
}

void Physics::TreeNode::split() {
	std::array<BoundingBox, 8> octlets = m_data.getRegion().octlets();
	std::array<std::vector<Entity>, 8> entitiesToMove{ {} };
	unsigned char usedChildren = 0;
	for (unsigned int i = 0; i < 8; i++) {
		for (const Entity& entity : m_data.getEntities()) {
			const auto& region = gCoordinator.getComponent<Components::RigidBody>(entity).Region;
			if (region.overlaps(octlets[i])) {
				entitiesToMove[i].push_back(entity);
				usedChildren |= (1 << i);
			}
		}
	}

	for (unsigned int i = 0; i < 8; i++) {
		createChild(i, octlets[i], entitiesToMove[i]);
	}

	m_data.removeAll();
}

void Physics::CollisionTree::updateRootSize() {
	// Must be able to also expand child nodes
	static glm::vec3 center = glm::vec3(0.0f, 0.0f, 0.0f);
	int axis = 1;
	BoundingBox enclosing;
	std::stack<Physics::TreeNode*> stack;
	stack.push(m_root);

	while (!stack.empty()) {
		TreeNode* p = stack.top();
		stack.pop();

		// Check for stuff
		for (const Entity& entity : p->getData().getEntities()) {
			const auto& regionA = gCoordinator.getComponent<Components::RigidBody>(entity).Region;
			enclosing = BoundingBox(-axis, -axis, -axis, axis, axis, axis);
			enclosing.offset(center);
			if (!regionA.overlaps(enclosing)) {
				axis <<= 1;
			}
		}

		for (unsigned int i = 0; i < 8; i++) {
			TreeNode* child = &p->getChild(i);
			if (child && !child->m_data.isEmpty()) {
				stack.push(child);
			}
		}
	}
}

std::set<Physics::EntityCollisionPair> Physics::CollisionTree::broadPhase() {
	std::set<Physics::EntityCollisionPair> candidates;

	broadPhase(*m_root, candidates);

	return candidates;
}

void Physics::CollisionTree::broadPhase(const TreeNode& node, std::set<Physics::EntityCollisionPair>& candidates) {
	if (!node.m_isLeaf) {
		// If we're not a leaf, go down till we are
		for (unsigned int i = 0; i < 8; i++) {
			if ((node.m_childrenUsedMask & (1 << i)) != 1) {
				continue;
			}
			const TreeNode& child = node.getChild(i);
			broadPhase(child, candidates);
		}
	} else {
		// We're at a leaf, find collisions
		const NodeData& data = node.getData();
		const std::vector<Entity>& entities = data.getEntities();

		// gCoordinator must be accessible from getUniquePairs
		auto overlap = [](const Entity& A, const Entity& B) {
			const auto& regionA = gCoordinator.getComponent<Components::RigidBody>(A).Region;
			const auto& regionB = gCoordinator.getComponent<Components::RigidBody>(B).Region;
			return (regionA.overlaps(regionB));
		};
		std::set<Physics::EntityCollisionPair> overlaps = node.getData().getUniquePairs(overlap);
		candidates.insert(overlaps.begin(), overlaps.end());
	}
}

BoundingBox Physics::CollisionTreeFactory::TreeConstructionPhase::rootSpace(const std::vector<Entity>& initial) {
	static glm::vec3 center = glm::vec3(0.0f, 0.0f, 0.0f);
	int axis = 1;
	BoundingBox enclosing;
	for (const Entity& entity : initial) {
		const auto& regionA = gCoordinator.getComponent<Components::RigidBody>(entity).Region;
		enclosing = BoundingBox(-axis, -axis, -axis, axis, axis, axis);
		enclosing.offset(center);
		if (!regionA.overlaps(enclosing)) {
			axis <<= 1;
		}
	}
	return enclosing;
}

void Physics::CollisionTreeFactory::TreeConstructionPhase::build(TreeNode& node) {
	static BoundingBox minimumRegion = BoundingBox(-0.1f, -0.1f, -0.1f, 0.1f, 0.1f, 0.1f);
	if (node.getData().getColliderCount() <= maxEntities ||
		node.getData().getRegion().smallerOrAt(minimumRegion)) {
		return;
	}

	node.split();

	for (unsigned int i = 0; i < 8; i++) {
		build(node.getChild(i));
	}
}

std::unique_ptr<Physics::TreeNode> Physics::CollisionTreeFactory::TreeConstructionPhase::constructTree() {
	if (m_entities.empty()) return;

	m_root->getData().addAll(m_entities);

	build(*m_root);

	return std::move(m_root);
}

std::unique_ptr<Physics::CollisionTree> Physics::CollisionTreeFactory::TreeFinalizationPhase::createTreeFromRoot(std::unique_ptr<Physics::TreeNode>&& root) {
	return std::make_unique<CollisionTree>(root);
}