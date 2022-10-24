#pragma once
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <cstdint>
#include <string>

#define BASIC_TITLE "Physics and rendering demo {}"

struct Mouse {
	float lastMouseX, lastMouseY;
	float deltaX, deltaY;
};

class Window {
private:
	GLFWwindow* handle;
	const uint16_t width;
	const uint16_t height;
	std::string title;

	Mouse mMouse;

	//void mouseCallback(GLFWwindow* window, double xpos, double ypos);
	//void keyCallback(GLFWwindow* window, int key, int scancode, int action, int mods);
public:
	Window(const uint16_t _width, const uint16_t _height, std::string _title);
	~Window();

	GLFWwindow* getHandle();
	void updateTitle(std::string title);

	int getInput(int key);
	Mouse getMouse();
};