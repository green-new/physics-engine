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
#include "camera.hpp"

void input_callback(GLFWwindow* handle, int key, int s, int action, int mods);
void reshape_callback(GLFWwindow* handle, int width, int height);
void destroy();

// Some globals for the program
window* game_window;
const uint16_t width = 1920;
const uint16_t height = 1080;
double mouseLastX, mouseLastY;

texture_t NOISE_TEXTURE;
std::vector<vertex_t> basic_verts = {
    // X, y, z, normX, normY, normZ, texU, texV (ST)
    {   1.0f, 1.0f, 0.0f,   1.0f, 0.0f, 0.0f,   1.0f, 1.0f   },
    {   1.0f,-1.0f, 0.0f,   1.0f, 0.0f, 0.0f,   1.0f, 0.0f   },
    {  -1.0f,-1.0f, 0.0f,   1.0f, 0.0f, 0.0f,   0.0f, 0.0f   },
    {  -1.0f, 1.0f, 0.0f,   1.0f, 0.0f, 0.0f,   0.0f, 1.0f   }
};
const std::vector<GLuint> indices = { 0, 1, 3, 1, 2, 3 };
mesh* text;
shader* basic_prog;
cam* camera;

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
    delete text;
    delete basic_prog;
    delete camera;
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
    NOISE_TEXTURE = noise_texture(RED, 256, 256, 1.0f, 8, 4210481);
    text = new mesh(basic_verts, indices, { NOISE_TEXTURE });
    basic_prog = new shader("VS_transform.glsl", "FS_transform.glsl");
    camera = new cam(glm::vec3(0.0f, 0.0f, 0.0f)); 
    
    // glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
}

void run() {
    const float PHYS_FREQ = 500.0f; // 500 Hz
    const float RENDER_FREQ = 1000.0f;  // 1000 Hz
    double       deltaTime = 0.0f,
        prevTime = glfwGetTime(),
        currTime = 0.0f,
        physTime = 0.0f,
        renderTime = 0.0f,
        fpsTime = 0.0f;
    bool        renderTick = false, 
                physTick = false;
    unsigned int frames = 0;

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
            glClearColor(0.3f, 0.3f, 0.3f, 1.0f);
            glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

            // view/projection transformations 
            // ===
            glm::mat4 projection = glm::perspective(glm::radians(90.0f), (float)width / (float)height, 0.1f, 1000.0f);
            glm::mat4 model = glm::mat4(1.0f);
            glm::mat4 view = glm::mat4(1.0f);
            view = glm::translate(view, glm::vec3(-1.0f, 1.0f, -3.0f));
            model = glm::rotate(model, (float)currTime, glm::vec3(1.0f, 1.0f, 0.0f));

            basic_prog->use();
            basic_prog->set_mat4("model", model);
            basic_prog->set_mat4("view", view);
            basic_prog->set_mat4("projection", projection);
            text->draw(*basic_prog);

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