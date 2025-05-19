#pragma once
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <cstdint>
#include <string>
#include "input_manager.hpp"

#define BASIC_TITLE "Physics and rendering demo {}"

class Window {
public:
	Window(const uint16_t width, 
		const uint16_t height, 
		std::string title,
		Input::MouseManager& m,
		Input::KeyboardManager& k)
		: m_width(width), 
			m_height(height), 
			m_title(title), 
			m_mouseManager(m),
			m_keyboardManager(k),
			m_handle(nullptr) {}
	~Window();

	void init();

	GLFWwindow* getHandle();
	void updateTitle(std::string title);
	uint16_t getWidth();
	uint16_t getHeight();

	void swapBuffers();

	void destroy();

	void close();

private:
	GLFWwindow* m_handle;
	const uint16_t m_width;
	const uint16_t m_height;
	std::string m_title;

	Input::MouseManager& m_mouseManager;
	Input::KeyboardManager& m_keyboardManager;
};