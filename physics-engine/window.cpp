#include "window.hpp"

inline void reshape_callback(GLFWwindow* handle, int width, int height) {
    glViewport(0, 0, width, height);
}

void mouseCallback(GLFWwindow* a, double xpos, double ypos) {
    if (!mMouse.lastMouseX || !mMouse.lastMouseY) {
        mMouse.lastMouseX = xpos;
        mMouse.lastMouseY = ypos;
    }

    double xo = xpos - mMouse.lastMouseX;
    double yo = mMouse.lastMouseY - ypos;

    mMouse.lastMouseX = xpos;
    mMouse.lastMouseY = ypos;
    mMouse.deltaX = xo;
    mMouse.deltaY = yo;
}

void keyCallback(GLFWwindow* a, int key, int scancode, int action, int mods) {
    if (key == GLFW_KEY_ESCAPE && action == GLFW_RELEASE) {
        glfwSetWindowShouldClose(a, 1);
    }
}

Window::Window(const uint16_t _width, const uint16_t _height, std::string _title) : width(_width), height(_height), title(_title), mMouse() {
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
    glfwSetCursorPosCallback(handle, mouseCallback);
    glfwSetKeyCallback(handle, keyCallback);
}

Window::~Window() {
    glfwDestroyWindow(handle);
}

void Window::updateTitle(std::string _title) {
    title = _title;
    glfwSetWindowTitle(handle, title.c_str());
}

int Window::getInput(int key) {
    return glfwGetKey(handle, key);
}

Mouse Window::getMouse() {
    return mMouse;
}

GLFWwindow* Window::getHandle() {
    return handle;
}