#pragma once
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <cstdint>
#include <string>
#include "input_manager.hpp"

#define BASIC_TITLE "Physics and rendering demo {}"

class Window {
private:
	GLFWwindow* handle;
	const uint16_t width;
	const uint16_t height;
	std::string title;

	Input::KeyboardManager keyboardManager;
	Input::MouseManager mouseManager;
public:
	Window(const uint16_t _width, const uint16_t _height, std::string _title);
	~Window();

	GLFWwindow* getHandle();
	void updateTitle(std::string title);
	uint16_t getWidth();
	uint16_t getHeight();

	void updateKeyboard();

	Input::KeyboardManager& getKeyboardManager();
	Input::MouseManager& getMouseManager();
};