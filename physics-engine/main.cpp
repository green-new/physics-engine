#define STB_IMAGE_IMPLEMENTATION
#include "stb_image/stb_image.h"

#include <iostream>
#include <sstream>
#include <format>
#include <cmath>
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include "window.hpp"
#include "mesh.hpp"
#include "resource.hpp"
#include "camera.hpp"

void input_callback(GLFWwindow* handle, int key, int s, int action, int mods);
void reshape_callback(GLFWwindow* handle, int width, int height);
void destroy();

// Some globals for the program
window* game_window;
registry::registry* Registry;
cam* camera;

typedef std::shared_ptr<mesh> dynamic_mesh;
std::unordered_map<std::string, dynamic_mesh> mesh_map;

const uint16_t width = 1920;
const uint16_t height = 1080;
double mouseLastX, mouseLastY;

bool updateNormals = false;
bool updatePolygonMode = false;
GLenum polygonMode = GL_FILL;
void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods)
{
    if (key == GLFW_KEY_F && action == GLFW_RELEASE) {
        updateNormals = true;
        std::cout << "Swapped normals" << std::endl;
    }
    if (key == GLFW_KEY_Q && action == GLFW_PRESS) {
        updatePolygonMode = true;
    }
}
void handle_input(GLFWwindow* window, float deltaTime) {
    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
        glfwSetWindowShouldClose(window, true);
 
    if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
        camera->process_input(Direction::Forward, deltaTime);
    if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
        camera->process_input(Direction::Backward, deltaTime);
    if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
        camera->process_input(Direction::Right, deltaTime);
    if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
        camera->process_input(Direction::Left, deltaTime);
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

    camera->process_mouse(xo, yo);
}

void destroy() {
    delete Registry;
    glfwDestroyWindow(game_window->get_handle());
    glfwTerminate();
    exit(0);
}

void start() {
    std::cout << "--- [ Controls ] ---\n";
    std::cout << "[ESC] \t\t\t\t--- Exit the program\n";
    std::cout << "[W, A, S, D] \t\t\t--- Move the camera\n";
    std::cout << "[F] \t\t\t\t--- Toggle flat vs smooth shading lighting\n";
    std::cout << "[Q] \t\t\t\t--- Toggle between GL_FILL, GL_POINT, and GL_LINE\n";

    game_window = new window(width, height, BASIC_TITLE);
    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
        std::cout << "Failed to initalize glad" << std::endl;
        return;
    };
    glfwSetCursorPosCallback(game_window->get_handle(), mouse_callback);
    glfwSetKeyCallback(game_window->get_handle(), key_callback);

    // Register everything
    try {
        Registry = new registry::registry;
    }
    catch (std::exception e) {
        std::cerr << "Error creating resources: " << e.what();
    }

    /* Dynamically allocate memory for the camera and meshes. */
    camera = new cam();
    mesh_map.insert({ "sphere", dynamic_mesh(new mesh(&Registry->get_geometry("uvsphere"), {Registry->get_texture("fire")})) });
    mesh_map.insert({ "sun", dynamic_mesh(new mesh(&Registry->get_geometry("uvsphere"), {Registry->get_texture("yellow")})) });
    mesh_map.insert({ "tetrahedron", dynamic_mesh(new mesh(&Registry->get_geometry("tetrahedron"), {Registry->get_texture("water")}))});
    mesh_map.insert({ "octahedron", dynamic_mesh(new mesh(&Registry->get_geometry("octahedron"), {Registry->get_texture("cloud")})) });
    mesh_map.insert({ "dodecahedron", dynamic_mesh(new mesh(&Registry->get_geometry("dodecahedron"), {Registry->get_texture("lava")})) });
    mesh_map.insert({ "icosahedron", dynamic_mesh(new mesh(&Registry->get_geometry("icosahedron"), {Registry->get_texture("blue")})) });
    mesh_map.insert({ "gourd", dynamic_mesh(new mesh(&Registry->get_geometry("gourd"), {Registry->get_texture("rock")})) });
    mesh_map.insert({ "teapot", dynamic_mesh(new mesh(&Registry->get_geometry("teapot"), {Registry->get_texture("orange")})) });
    mesh_map.insert({ "terrain", dynamic_mesh(new mesh(&Registry->get_geometry("terrain"), {Registry->get_texture("grass")})) });

    // Depth buffering (3D)
    glEnable(GL_DEPTH_TEST);
}

void run() {
    const float PHYS_FREQ = 500.0f; // 500 Hz
    const float RENDER_FREQ = 2000.0f;  // 1000 Hz
    double      deltaTime = 0.0f,
        prevTime = glfwGetTime(),
        currTime = 0.0f,
        physTime = 0.0f,
        renderTime = 0.0f,
        fpsTime = 0.0f;
    bool        renderTick = false, 
                physTick = false;
    unsigned int frames = 0;
    shader& prog = Registry->get_shader("VS_transform");
    shader& skybox = Registry->get_shader("skybox");
    prog.use();
    prog.set_int("texture1", 0);

    skybox.use();
    skybox.set_int("skybox", 0);

    glm::vec3 lightPosition = glm::vec3(0.0f, 0.0f, 0.0f);
    glm::vec3 lightColor = glm::vec3(1.0f, 1.0f, 0.3f);

    /* Set up the skybox. It is not a mesh object because it does not have normals.
    Because of this, we probably need to rework how we do mesh objects, possibly creating a 
    hierarchy of complexity based on inheritance. However, it matches that of a cube. This is where geometry_procedural
    lacks, because no matter what we build normals even though we may not want normals. So we need to define a new cube here. */
    unsigned int skyboxVao;
    unsigned int skyboxVbo;
    GLuint skyboxTexture = Registry->get_texture("skybox");
    // All one one line to save some space here.
    float skyboxVertices[] = { -1.0f,  1.0f, -1.0f, -1.0f, -1.0f, -1.0f, 1.0f, -1.0f, -1.0f, 1.0f, -1.0f, -1.0f, 1.0f,  1.0f, -1.0f, -1.0f,  1.0f, -1.0f,  -1.0f, -1.0f,  1.0f, -1.0f, -1.0f, -1.0f, -1.0f,  1.0f, -1.0f, -1.0f,  1.0f, -1.0f, -1.0f,  1.0f,  1.0f, -1.0f, -1.0f,  1.0f,  1.0f, -1.0f, -1.0f, 1.0f, -1.0f,  1.0f, 1.0f,  1.0f,  1.0f, 1.0f,  1.0f,  1.0f, 1.0f,  1.0f, -1.0f, 1.0f, -1.0f, -1.0f,  -1.0f, -1.0f,  1.0f, -1.0f,  1.0f,  1.0f, 1.0f,  1.0f,  1.0f, 1.0f,  1.0f,  1.0f, 1.0f, -1.0f,  1.0f, -1.0f, -1.0f,  1.0f,  -1.0f,  1.0f, -1.0f, 1.0f,  1.0f, -1.0f, 1.0f,  1.0f,  1.0f, 1.0f,  1.0f,  1.0f, -1.0f,  1.0f,  1.0f, -1.0f,  1.0f, -1.0f,  -1.0f, -1.0f, -1.0f, -1.0f, -1.0f,  1.0f, 1.0f, -1.0f, -1.0f, 1.0f, -1.0f, -1.0f, -1.0f, -1.0f,  1.0f, 1.0f, -1.0f,  1.0f };
    glGenVertexArrays(1, &skyboxVao);
    glGenBuffers(1, &skyboxVbo);
    glBindVertexArray(skyboxVao);
    glBindBuffer(GL_ARRAY_BUFFER, skyboxVbo);
    glBufferData(GL_ARRAY_BUFFER, sizeof(skyboxVertices), &skyboxVertices, GL_STATIC_DRAW);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(GLfloat), 0);
    glBindVertexArray(0);

    /* Loop until the user closes the window */
    while (!glfwWindowShouldClose(game_window->get_handle())) {
        frames++;
        /* Time */
        currTime = glfwGetTime();
        deltaTime = currTime - prevTime;

        camera->tick(deltaTime);

        /* Physics update */
        physTick = currTime - physTime > 1.0f / PHYS_FREQ;
        if (physTick) {
            physTime = currTime;

            /* Update physics system */
            // physics_system->update();
        }
        /* Render update */
        renderTick = (currTime - renderTime > 1.0f / (RENDER_FREQ));
        if (renderTick) {
            renderTime = currTime;
            glClearColor(0.05f, 0.05f, 0.05f, 1.0f);
            glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
            
            /* Create the projection, view, and model matrices.
            Projection does not need to be instantiated every frame, but we'll put it here to be organized. */
            glm::mat4 projection = glm::perspective(glm::radians(90.0f), (float)width / (float)height, 0.1f, 1000.0f);
            glm::mat4 view = camera->get_matview();
            glm::mat4 model = glm::mat4(1.0f);
            prog.use();
            prog.set_float("time", (float)currTime);
            prog.set_mat4("projection", projection);
            prog.set_mat4("view", view);
            prog.set_vec3("lightPos", lightPosition);
            prog.set_vec3("viewPos", camera->position);
            prog.set_vec3("lightColor", lightColor);



            /* Draw the skybox first!.
            https://learnopengl.com/Advanced-OpenGL/Cubemaps
            */
            glDepthMask(GL_FALSE);
            // We do this to remove the translation matrix.
            // This is so the skybox does not move with the player.
            view = glm::mat4(glm::mat3(camera->get_matview()));
            skybox.use();
            skybox.set_mat4("projection", projection);
            skybox.set_mat4("view", view);
            glBindVertexArray(skyboxVao);
            glActiveTexture(GL_TEXTURE0);
            glBindTexture(GL_TEXTURE_CUBE_MAP, skyboxTexture);
            glDrawArrays(GL_TRIANGLES, 0, 36);
            glDepthMask(GL_TRUE);
            

            /* Animate the light position in a circle (using sin(x), cos(z)). */
            lightPosition = glm::vec3(sin(renderTime) * 100.0f, 100.0f, cos(renderTime) * 100.0f);
            float x, y, z;
            unsigned int n = 10;
            float max = 250.0f;
            float maxhalf = max / 2.0f;
            srand(NULL);
            prog.use();
            for (auto const& [mesh_name, mesh_current] : mesh_map) {
                if (mesh_name == "terrain") {
                    // Only run once!
                    model = glm::mat4(1.0f);
                    x = 0.0f;
                    y = -100.0f;
                    z = 0.0f; 
                    model = glm::translate(model, glm::vec3(x, y, z));
                    prog.set_mat4("model", model);
                    mesh_current.get()->draw(prog);
                    continue;
                }
                if (mesh_name == "sun") {
                    // Only run once!
                    model = glm::mat4(1.0f);
                    model = glm::translate(model, lightPosition);
                    model = glm::scale(model, glm::vec3(50.0f, 50.0f, 50.0f));
                    prog.set_mat4("model", model);
                    mesh_current.get()->draw(prog);
                    continue;
                }
                for (unsigned int i = 0; i < n; i++) {
                    model = glm::mat4(1.0f);
                    x = ((float)rand() / (float)RAND_MAX) * max - maxhalf;
                    y = ((float)rand() / (float)RAND_MAX) * max;
                    z = ((float)rand() / (float)RAND_MAX) * max - maxhalf;
                    model = glm::translate(model, glm::vec3(x, y, z));
                    prog.set_mat4("model", model);
                    mesh_current.get()->draw(prog);
                }
            }

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

        if (updateNormals) {
            updateNormals = false;

            // Swap all active meshes
            for (auto const& [mesh_name, mesh_current] : mesh_map) {
                mesh_current.get()->swap();
            }
        }

        if (updatePolygonMode) {
            updatePolygonMode = false;
            polygonMode++;
            if (polygonMode > GL_FILL) {
                polygonMode = GL_POINT;
            }
            std::cout << "Wireframe mode set to: ";
            switch (polygonMode) {
            case GL_POINT:
                std::cout << "GL_POINT\n";
                break;
            case GL_LINE:
                std::cout << "GL_LINE\n";
                break;
            case GL_FILL:
                std::cout << "GL_FILL\n";
                break;
            }
            glPolygonMode(GL_FRONT_AND_BACK, polygonMode);
        }

        /* Handle input */
        handle_input(game_window->get_handle(), deltaTime);

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