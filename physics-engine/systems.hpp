#pragma once
#include <set>
#include "types.hpp"

namespace Systems {

}

class System {
public:
	virtual void update(float deltaTime) = 0;
	std::set<Entity> m_entities;
};

#include "camera_system.hpp"
#include "physics_system.hpp"
#include "render_system.hpp"