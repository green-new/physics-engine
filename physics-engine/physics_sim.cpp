#include "physics_sim.hpp"

void PhysicsSim::init() {
	m_system->init();
}

void PhysicsSim::simulate() {
	m_system->update(*m_dt);
}
