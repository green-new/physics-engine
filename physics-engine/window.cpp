#include "window.hpp"

inline void reshape_callback(GLFWwindow* handle, int width, int height) {
    glViewport(0, 0, width, height);
}

window::window(const uint16_t _width, const uint16_t _height, std::string _title) : width(_width), height(_height), title(_title) {
    // Set the window hints (version, GL profile, window)
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    glfwWindowHint(GLFW_RESIZABLE, GL_FALSE);

	handle = glfwCreateWindow(width, height, title.c_str(), nullptr, nullptr);

    if (!handle) {
        glfwTerminate();
        exit(-1);
    }

    /* Make the window's context current */
    glfwMakeContextCurrent(handle);

    glfwSetInputMode(handle, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
    glfwSetWindowSizeCallback(handle, reshape_callback);
}

window::~window() {
    glfwDestroyWindow(handle);
}

void window::update_title(std::string _title) {
    title = _title;
    glfwSetWindowTitle(handle, title.c_str());
}

GLFWwindow* window::get_handle() {
    return handle;
}