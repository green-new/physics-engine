#pragma once
#include "window.hpp"
#include "coordinator.hpp"

class InputManager {

private:
	Input::MouseManager		m_mouseManager;
	Input::KeyboardManager	m_keyboardManager;
};

class Engine {
public:

private:
	Window						m_window;
	Coordinator					m_coordinator;
	Resources::ResourceManager	m_resourceManager;
	InputManager				m_inputManager;
};