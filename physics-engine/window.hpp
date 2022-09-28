#pragma once
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <cstdint>
#include <string>

#define BASIC_TITLE "Physics and rendering demo {}"

class window {
private:
	GLFWwindow* handle;
	const uint16_t width;
	const uint16_t height;
	std::string title;
public:
	window(const uint16_t _width, const uint16_t _height, std::string _title);
	~window();

	GLFWwindow* get_handle();
	void update_title(std::string title);
};