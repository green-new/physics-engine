#define STB_IMAGE_IMPLEMENTATION
#include "stb_image/stb_image.h"

#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include "engine.hpp"

const uint16_t width = 1920;
const uint16_t height = 1080;

int main() {
    /* Initialize glfw */
    if (!glfwInit()) {
        return -1;
    }

    Engine engine;

    engine.init();
    engine.run();
    engine.destroy();

    return 0;
}