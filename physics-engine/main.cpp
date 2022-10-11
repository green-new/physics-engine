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
#include "texture.hpp"
#include "mesh.hpp"

void input_callback(GLFWwindow* handle, int key, int s, int action, int mods);
void reshape_callback(GLFWwindow* handle, int width, int height);
void destroy();

// Some globals for the program
window* game_window;
const uint16_t width = 1920;
const uint16_t height = 1080;
double mouseLastX, mouseLastY;

texture_t txt_noise_red, txt_noise_green, txt_noise_blue, txt_red, txt_green, txt_blue, txt_xor_red, txt_xor_green, txt_xor_blue, txt_flat1;
mesh* obj;
shader* basic_prog;

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
    delete basic_prog;

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

    /* Create entities, load models, meshes, and textures */
    /* Generate textures */
    uint32_t seed = 81293754;
    txt_noise_red = noise_texture(RED, 64, 64, 1.0f, 8, seed); 
    txt_noise_green = noise_texture(GREEN, 64, 64, 1.0f, 8, seed);
    txt_noise_blue = noise_texture(BLUE, 64, 64, 1.0f, 8, seed);
    txt_red = solid_colored_texture(RED);
    txt_green = solid_colored_texture(GREEN);
    txt_blue = solid_colored_texture(BLUE);
    txt_xor_red = xor_texture(RED, 256, 256);
    txt_xor_green = xor_texture(GREEN, 256, 256);
    txt_xor_blue = xor_texture(BLUE, 256, 256);
    txt_flat1 = build_texture("flat_1.png");

    /* Generate meshes */
    try {
        
    }
    catch (std::exception e) {
        std::cerr << e.what();
    }
    obj = new mesh(&icosahedron, {txt_noise_blue});

    /* Allocate shader program */
    basic_prog = new shader("VS_transform.glsl", "FS_transform.glsl");
    
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

    basic_prog->use();
    basic_prog->set_int("texture1", 0);
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
            glm::mat4 projection = glm::perspective(glm::radians(90.0f), (float)width / (float)height, 0.1f, 10000.0f);
            glm::mat4 view = glm::mat4(1.0f);
            view = glm::translate(view, glm::vec3(0.0f, 0.0f, -3.0f));
            basic_prog->set_mat4("view", view);
            basic_prog->set_mat4("projection", projection);
            for (unsigned int i = 0; i < 10; i++) {
                glm::mat4 model = glm::mat4(1.0f);
                model = glm::scale(model, glm::vec3(0.75f));
                model = glm::translate(model, positions[i]);
                model = glm::rotate(model, (float)currTime, glm::vec3(0.0f, 1.0f, 1.0f));

                basic_prog->use();
                basic_prog->set_mat4("model", model);
                obj->draw(*basic_prog);
            }
            // text->draw(*basic_prog);

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