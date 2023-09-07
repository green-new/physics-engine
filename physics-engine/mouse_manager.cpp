#include "mouse_manager.hpp"

namespace Input {
	size_t MouseManager::subscribe(mouse_callback_fn func) {
		m_callbacks.push_back(func);
		return m_callbacks.size();
	}

	void MouseManager::unsubscribe(size_t index) {
		m_callbacks.erase(m_callbacks.begin() + index);
	}

	void MouseManager::update(double xpos, double ypos) {
		m_mState.prev.mouseXo = m_mState.curr.mouseXo;
		m_mState.prev.mouseYo = m_mState.curr.mouseYo;

		m_mState.curr.mouseXo = xpos;
		m_mState.curr.mouseYo = ypos;

		for (const auto& func : m_callbacks) {
			func(m_mState);
		}
	}
}