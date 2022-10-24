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
#include "skybox.hpp"
#include "coordinator.hpp"

const uint16_t width = 1920;
const uint16_t height = 1080;

bool updateNormals = false;
bool updatePolygonMode = false;
GLenum polygonMode = GL_FILL;

std::unique_ptr<Window> gameWindow;
std::unique_ptr<registry::ResourceManager> resourceManager;

void handle_input(GLFWwindow* window, float deltaTime) {
    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
        glfwSetWindowShouldClose(window, true);
}

void start() {
    std::cout << "--- [ Controls ] ---\n";
    std::cout << "[ESC] \t\t\t\t--- Exit the program\n";
    std::cout << "[W, A, S, D] \t\t\t--- Move the camera\n";
    std::cout << "[F] \t\t\t\t--- Toggle flat vs smooth shading lighting\n";
    std::cout << "[Q] \t\t\t\t--- Toggle between GL_FILL, GL_POINT, and GL_LINE\n";

    if (gladLoadGLLoader((GLADloadproc)glfwGetProcAddress) != 0) {
        std::cerr << "[glad] Error loading OpenGL\n";
    }

    gameWindow = std::make_unique<Window>(width, height, BASIC_TITLE);
    resourceManager = std::make_unique<registry::ResourceManager>();
    gCoordinator.init();

    gCoordinator.registerComponent<Components::Appearence>();
    gCoordinator.registerComponent<Components::Camera>();
    gCoordinator.registerComponent<Components::Orientation>();
    gCoordinator.registerComponent<Components::Physics>();
    gCoordinator.registerComponent<Components::RenderShape>();
    gCoordinator.registerComponent<Components::Transform>();

    // Depth buffering (3D)
    glEnable(GL_DEPTH_TEST);
}

void destroy() {
    glfwDestroyWindow(gameWindow->getHandle());
    glfwTerminate();
    exit(0);
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

    glm::mat4 projection = glm::perspective(glm::radians(90.0f), (float)width / (float)height, 0.1f, 1000.0f);
    glm::mat4 view = glm::mat4(1.0f);
    glm::mat4 model = glm::mat4(1.0f);

    Shader& prog = resourceManager->getShader("VS_transform");
    Shader& skyboxShader = resourceManager->getShader("skybox");
    prog.use();
    prog.set_int("texture1", 0);

    texture_t skyboxTexture = resourceManager->getTexture("skybox");
    Skybox skybox(projection, skyboxShader, skyboxTexture);
    skybox.init();
    
    Entity player = gCoordinator.createEntity();
    gCoordinator.addComponent(player, Components::Camera
        {
            .FOV = 90,
            .ProjectionMatrix = projection,
            .ViewMatrix = view,
            .WorldUp = glm::vec3(0.0f, 1.0f, 0.0f)
        });
    gCoordinator.addComponent(player, Components::Transform
        {
            .Position = glm::vec3(0.0f),
            .Rotation = glm::vec3(0.0f),
            .Scale = glm::vec3(0.0f),
            .Angle = 0.0f
        });
    gCoordinator.addComponent(player, Components::Orientation
        {
            .Front = glm::vec3(0.0f),
            .Up = glm::vec3(0.0f),
            .Right = glm::vec3(0.0f)
        });

    std::vector<Entity> entities;
    const int n = 100;
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_real_distribution<> x(-50.0f, 50.0f);
    std::uniform_real_distribution<> y(-50.0f, 50.0f);
    std::uniform_real_distribution<> z(-50.0f, 50.0f);

    Mesh3D sphere = Mesh3D(&resourceManager->getGeometry("uvsphere"));
    for (unsigned int i = 0; n < 100; i++) {
        Entity entity = gCoordinator.createEntity();
        glm::vec3 position = glm::vec3(x(gen), y(gen), z(gen));

        gCoordinator.addComponent(entity, Components::RenderShape
            {
                .Shape = sphere
            });
        gCoordinator.addComponent(entity, Components::Transform
            {
                .Position = position,
                .Rotation = glm::vec3(0.0f),
                .Scale = glm::vec3(0.0f),
                .Angle = 0.0f
            });

        entities.push_back(entity);
    }

    auto physicsSystem = gCoordinator.registerSystem<Systems::PhysicsSystem>();
    {
        Signature signature;
        signature.set(gCoordinator.getComponentType<Components::Physics>(), true);
        signature.set(gCoordinator.getComponentType<Components::Transform>(), true);
        gCoordinator.setSystemSignature<Systems::PhysicsSystem>(signature);
    }

    auto renderSystem = gCoordinator.registerSystem<Systems::RenderSystem>();
    {
        Signature signature;
        signature.set(gCoordinator.getComponentType<Components::Appearence>(), true);
        signature.set(gCoordinator.getComponentType<Components::Transform>(), true);
        signature.set(gCoordinator.getComponentType<Components::RenderShape>(), true);
        gCoordinator.setSystemSignature<Systems::RenderSystem>(signature);
    }

    /* Loop until the user closes the window */
    while (!glfwWindowShouldClose(gameWindow->getHandle())) {
        frames++;
        /* Time */
        currTime = glfwGetTime();
        deltaTime = currTime - prevTime;

        /* Physics update */
        physTick = currTime - physTime > 1.0f / PHYS_FREQ;
        if (physTick) {
            physTime = currTime;

            physicsSystem->update(deltaTime);
        }
        /* Render update */
        renderTick = (currTime - renderTime > 1.0f / (RENDER_FREQ));
        if (renderTick) {
            renderTime = currTime;
            glClearColor(0.05f, 0.05f, 0.05f, 1.0f);
            glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
            
            renderSystem->update(deltaTime);

            glfwSwapBuffers(gameWindow->getHandle());
        }

        /* FPS counter */
        if (currTime - fpsTime >= 1.0f) {
            double fps = frames / (currTime - fpsTime);
            std::string t = std::format(BASIC_TITLE, fps);
            gameWindow->updateTitle(t);

            frames = 0;
            fpsTime = currTime;
        }

        prevTime = currTime;

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
        handle_input(gameWindow->getHandle(), deltaTime);

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