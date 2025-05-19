#include "clock.hpp"

#include <GLFW/glfw3.h>

#include <glm/glm.hpp>

void Clock::update() {

	/* Update current time */
	m_prevTime = m_currTime;
	m_currTime = glfwGetTime();
	m_deltaTime = m_currTime - m_prevTime;

	/* Reset tick booleans */
	m_physTick = m_physTick ? false : m_physTick;
	m_renderTick = m_renderTick ? false : m_renderTick;
	m_fpsTick = m_fpsTick ? false : m_fpsTick;

	/* Update tick flags */
	m_physTick = ((m_currTime - m_physTime) > (1.0f / m_physFreq));
	m_renderTick = ((m_currTime - m_renderTime) > (1.0f / m_renderFreq));
	m_fpsTick = ((m_currTime - m_fpsTime) > 1.0f);

	/* Set physics delta */
	if (m_physTick) {
		m_physDelta = m_currTime - m_physTime;
		m_physTime = m_currTime;
	}

	/* Set render delta */
	if (m_renderTick) {
		m_renderDelta = m_currTime - m_renderTime;
		m_renderTime = m_currTime;
	}

	/* Set fps delta */
	if (m_fpsTick) {
		m_fpsDelta = m_currTime - m_fpsTime;
		m_fpsTime = m_currTime;
	}
}
