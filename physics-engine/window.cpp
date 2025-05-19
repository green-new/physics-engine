#include "window.hpp"

inline void reshapeCallback(GLFWwindow* m_handle, int m_width, int m_height) {
    glViewport(0, 0, m_width, m_height);
}

void Window::init() {
    // Set the window hints (version, GL profile, window)
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    glfwWindowHint(GLFW_RESIZABLE, GL_FALSE);

    // create our handle
    m_handle = glfwCreateWindow(m_width, m_height, m_title.c_str(), nullptr, nullptr);

    if (!m_handle) {
        glfwTerminate();
        exit(-1);
    }

    glfwSetInputMode(m_handle, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
    glfwSetWindowSizeCallback(m_handle, reshapeCallback);

    /* Make the window's context current */
    glfwMakeContextCurrent(m_handle);

    // Prepare our input managers.
    glfwSetWindowUserPointer(m_handle, this);
    auto cursorPosCallback = [](GLFWwindow* m_handle, double xpos, double ypos) {
        static_cast<Window*>(glfwGetWindowUserPointer(m_handle))->m_mouseManager.update(xpos, ypos);
    };
    auto keyboardCallback = [](GLFWwindow* m_handle, int key, int scancode, int action, int mods) {
        static_cast<Window*>(glfwGetWindowUserPointer(m_handle))->m_keyboardManager.update(key, scancode, action, mods);
    };
    glfwSetKeyCallback(m_handle, keyboardCallback);
    glfwSetCursorPosCallback(m_handle, cursorPosCallback);
}

Window::~Window() {
    glfwDestroyWindow(m_handle);
}

uint16_t Window::getWidth() {
    return m_width;
}

uint16_t Window::getHeight() {
    return m_height;
}

void Window::updateTitle(std::string _m_title) {
    m_title = _m_title;
    glfwSetWindowTitle(m_handle, m_title.c_str());
}

GLFWwindow* Window::getHandle() {
    return m_handle;
}

void Window::swapBuffers() {
    glfwSwapBuffers(m_handle);
}

void Window::destroy() {
    glfwDestroyWindow(m_handle);
}

void Window::close() {
    glfwSetWindowShouldClose(m_handle, true);
}
