#pragma once
#include <set>
#include "types.hpp"

namespace Systems {

}

class Coordinator;

class System {
public:
	System(Coordinator& c)
		: m_coordinator(c) {}
public:
	virtual void update(float deltaTime) = 0;
	std::set<Entity> m_entities;
	Coordinator& m_coordinator;
};

#include "camera_system.hpp"
#include "physics_system.hpp"
#include "render_system.hpp"