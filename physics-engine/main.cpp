#define STB_IMAGE_IMPLEMENTATION
#include "stb_image/stb_image.h"

#include <iostream>
#include <sstream>
#include <format>
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include "window.hpp"
#include "mesh.hpp"
#include "resource.hpp"

void input_callback(GLFWwindow* handle, int key, int s, int action, int mods);
void reshape_callback(GLFWwindow* handle, int width, int height);
void destroy();

// Some globals for the program
window* game_window;
registry::registry* Registry;
mesh* sphere;
mesh* cube;
mesh* dodecahedron;

const uint16_t width = 1920;
const uint16_t height = 1080;
double mouseLastX, mouseLastY;

void input_callback(GLFWwindow* handle, int key, int s, int action, int mods) {
    if (action == GLFW_PRESS)
        switch (key) {
        case GLFW_KEY_ESCAPE:
            destroy();
            break;
        }
}

void mouse_callback(GLFWwindow* window, double xpos, double ypos) {
    if (!mouseLastX || !mouseLastY) {
        mouseLastX = xpos;
        mouseLastY = ypos;
    }

    double xo = xpos - mouseLastX;
    double yo = mouseLastY - ypos;

    mouseLastX = xpos;
    mouseLastY = ypos;
}

void destroy() {
    delete Registry;
    glfwDestroyWindow(game_window->get_handle());
    glfwTerminate();
    exit(0);
}

void start() {
    game_window = new window(width, height, BASIC_TITLE);
    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
        std::cout << "Failed to initalize glad" << std::endl;
        return;
    };
    glfwSetKeyCallback(game_window->get_handle(), input_callback);
    glfwSetCursorPosCallback(game_window->get_handle(), mouse_callback);

    // Register everything
    try {
        Registry = new registry::registry;
    }
    catch (std::exception e) {
        std::cerr << "Error creating resources: " << e.what();
    }
    sphere = new mesh(&Registry->get_geometry("uvsphere"), { Registry->get_texture("noise_cloud") }, GEOLIB_FLATNORMALS);
    cube = new mesh(&Registry->get_geometry("cube"), { Registry->get_texture("noise_cloud") }, GEOLIB_FLATNORMALS);
    dodecahedron = new mesh(&Registry->get_geometry("dodecahedron"), { Registry->get_texture("noise_lava") }, GEOLIB_SMOOTHNORMALS);

    // Wireframe mode
    //glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);

    // Depth buffering (3D)
    glEnable(GL_DEPTH_TEST);
}

void run() {
    const float PHYS_FREQ = 500.0f; // 500 Hz
    const float RENDER_FREQ = 2000.0f;  // 1000 Hz
    double       deltaTime = 0.0f,
        prevTime = glfwGetTime(),
        currTime = 0.0f,
        physTime = 0.0f,
        renderTime = 0.0f,
        fpsTime = 0.0f;
    bool        renderTick = false, 
                physTick = false;
    unsigned int frames = 0;

    const unsigned int n_cubes = 10;
    glm::vec3 positions[n_cubes] = {
        glm::vec3(0.0f, 0.0f, 0.0f),
        glm::vec3(-4.5f, 1.0f, 0.5f),
        glm::vec3(2.3f, 0.4f, -3.4f),
        glm::vec3(6.3f, 9.4f, 3.0f),
        glm::vec3(1.3f, 4.5f, -4.4f),
        glm::vec3(8.9f, 0.3f, 5.0f),
        glm::vec3(1.0f, 1.0f, 1.0f),
        glm::vec3(-1.0f, -6.0f, 2.0f),
        glm::vec3(-3.1f, 2.0f, 0.0f),
        glm::vec3(-5.0f, -3.0f, 1.4f)
    };
    shader& prog = Registry->get_shader("VS_transform");
    prog.use();
    prog.set_int("texture1", 0);

    /* Loop until the user closes the window */
    while (!glfwWindowShouldClose(game_window->get_handle())) {
        frames++;
        /* Time */
        currTime = glfwGetTime();
        deltaTime = currTime - prevTime;

        /* Physics update */
        physTick = currTime - physTime > 1.0f / PHYS_FREQ;
        if (physTick) {
            physTime = currTime;

            /* Update physics system */
            // physics_system->update();
        }

        /* Render update */
        renderTick = (currTime - renderTime > 1.0f / RENDER_FREQ);
        if (renderTick) {
            renderTime = currTime;
            glClearColor(0.05f, 0.05f, 0.05f, 1.0f);
            glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
            
            // view/projection transformations 
            // ===
            glm::mat4 projection = glm::perspective(glm::radians(90.0f), (float)width / (float)height, 0.1f, 1000.0f);
            glm::mat4 view = glm::mat4(1.0f);
            view = glm::translate(view, glm::vec3(0.0f, 0.0f, -3.0f));
            glm::mat4 model = glm::mat4(1.0f);
            model = glm::rotate(model, (float)currTime, glm::vec3(0.0f, 1.0f, 0.0f));
            prog.use();
            prog.set_mat4("projection", projection);
            prog.set_mat4("view", view);
            prog.set_mat4("model", model);

            //sphere->draw(prog);
            dodecahedron->draw(prog);
            //cube->draw(prog);

            /* Update render system */
            // render_system->update();

            /* Swap front and back buffers */
            glfwSwapBuffers(game_window->get_handle());
        }

        /* FPS counter */
        if (currTime - fpsTime >= 1.0f) {
            double fps = frames / (currTime - fpsTime);
            std::string t = std::format(BASIC_TITLE, fps);
            game_window->update_title(t);

            frames = 0;
            fpsTime = currTime;
        }

        prevTime = currTime;

        /* Poll for and process events */
        glfwPollEvents();
    }
}

int main() {
    /* Initialize glfw */
    if (!glfwInit())
        return -1;

    start();
    run();
    destroy();

    return 0;
}