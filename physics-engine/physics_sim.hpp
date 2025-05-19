#pragma once

#include "systems.hpp"

class PhysicsSim {
public:
	PhysicsSim(
		std::shared_ptr<Systems::PhysicsSystem> s,
		std::shared_ptr<double> dt
	) : 
		m_system(s),
		m_dt(dt) { }
	~PhysicsSim() = default;
public:
	void init();
	void simulate();
private:
	std::shared_ptr<Systems::PhysicsSystem> m_system;
	std::shared_ptr<double> m_dt;
};
