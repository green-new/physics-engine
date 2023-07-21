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
#include "types.hpp"
#include "systems.hpp"
#include "components.hpp"
#include "coordinator.hpp"
#include "resource.hpp"
#include "input_manager.hpp"

const uint16_t width = 1920;
const uint16_t height = 1080;

bool updateNormals = false;
bool updatePolygonMode = false;
GLenum polygonMode = GL_FILL;

std::unique_ptr<Window> gameWindow;
std::unique_ptr<Resources::ResourceManager> resourceManager;
Coordinator gCoordinator;

void main_input(Input::GLFWKey key, bool state) {
    using namespace Input;
    if (key == GLFWKey::Escape && state == GLFW_PRESS) {
        glfwSetWindowShouldClose(gameWindow->getHandle(), true);
    }
}

void start() {
    std::cout << "--- [ Controls ] ---\n";
    std::cout << "[ESC] \t\t\t\t--- Exit the program\n";
    std::cout << "[W, A, S, D] \t\t\t--- Move the camera\n";
    std::cout << "[F] \t\t\t\t--- Toggle flat vs smooth shading lighting\n";
    std::cout << "[Q] \t\t\t\t--- Toggle between GL_FILL, GL_POINT, and GL_LINE\n";
    
    gameWindow = std::make_unique<Window>(width, height, BASIC_TITLE);
        
    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
        std::cerr << "[glad] Error loading OpenGL\n";
    }

    resourceManager = std::make_unique<Resources::ResourceManager>();
    gameWindow->getKeyboardManager().subscribe(&main_input, {Input::GLFWKey::Escape}, 1);
    resourceManager->init();

    // Register the components
    gCoordinator.init();
    gCoordinator.registerComponent<Components::Appearence>();
    gCoordinator.registerComponent<Components::Camera>();
    gCoordinator.registerComponent<Components::Orientation>();
    gCoordinator.registerComponent<Components::RigidBody>();
    gCoordinator.registerComponent<Components::RenderShape>();
    gCoordinator.registerComponent<Components::Transform>();

    // Depth buffering (3D)
    glEnable(GL_DEPTH_TEST);
    glClearColor(0.05f, 0.05f, 0.05f, 1.0f);
}

void destroy() {
    glfwDestroyWindow(gameWindow->getHandle());
    glfwTerminate();
    exit(0);
}

void run() {
    const float PHYS_FREQ = 500.0f; // 500 Hz
    const float RENDER_FREQ = 500.0f;  // 500 Hz

    double      deltaTime = 0.0f,
                prevTime = glfwGetTime(),
                currTime = 0.0f,
                physTime = 0.0f,
                renderTime = 0.0f,
                fpsTime = 0.0f;

    bool        renderTick = false, 
                physTick = false;
    unsigned int frames = 0;

    auto physicsSystem = gCoordinator.registerSystem<Systems::PhysicsSystem>();
    {
        Signature signature;
        signature.set(gCoordinator.getComponentType<Components::RigidBody>());
        signature.set(gCoordinator.getComponentType<Components::Transform>());
        gCoordinator.setSystemSignature<Systems::PhysicsSystem>(signature);
    }

    auto renderSystem = gCoordinator.registerSystem<Systems::RenderSystem>();
    {
        Signature signature;
        signature.set(gCoordinator.getComponentType<Components::Appearence>());
        signature.set(gCoordinator.getComponentType<Components::Transform>());
        signature.set(gCoordinator.getComponentType<Components::RenderShape>());
        gCoordinator.setSystemSignature<Systems::RenderSystem>(signature);
    }

    renderSystem->init();

    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_real_distribution<> x_dist(-20.0f, 20.0f);
    std::uniform_real_distribution<> y_dist(-20.0f, 20.0f);
    std::uniform_real_distribution<> z_dist(-20.0f, 20.0f);
    std::uniform_real_distribution<float> zero_to_one(0.0f, 1.0f);
    std::uniform_int_distribution<> shape_dist(0, 3);

    //std::array<std::shared_ptr<Components::RenderShape>, 4> shape_set = {
    //    std::shared_ptr<Components::RenderShape> {.Shape = new Mesh3D(GLDataAdapter(resourceManager->getGeometry("tetrahedron")).requestData()) },
    //    Components::RenderShape {.Shape = new Mesh3D(GLDataAdapter(resourceManager->getGeometry("octahedron")).requestData()) },
    //    Components::RenderShape {.Shape = new Mesh3D(GLDataAdapter(resourceManager->getGeometry("cube")).requestData()) },
    //    Components::RenderShape {.Shape = new Mesh3D(GLDataAdapter(resourceManager->getGeometry("uvsphere")).requestData()) }
    //};
    std::shared_ptr<Components::RenderShape> shape = std::make_shared<Components::RenderShape>(Components::RenderShape{
        .Shape = new Mesh3D(GLDataAdapter(resourceManager->getGeometry("icosahedron")).requestData())
        });
    for (uint32_t i = 0; i < 100; i++) {
        Entity entity = gCoordinator.createEntity();
        gCoordinator.addComponent(entity, Components::Transform{
            .Position = glm::vec3(x_dist(gen), y_dist(gen), z_dist(gen)),
            .Rotation = glm::vec3(0.0f),
            .Scale = glm::vec3(1.0f),
            .RotationAngle = 0.0f
            });
        unsigned int index = shape_dist(gen);
        gCoordinator.addComponent(entity, Components::RenderShape{
            .Shape = shape.get()->Shape
            });
        gCoordinator.addComponent(entity, Components::Appearence{
            .Texture = resourceManager->getTexture("cloud"),
            .Opacity = zero_to_one(gen),
            .Reflectance = 0.0f,
            .BaseColor = glm::vec3(zero_to_one(gen), zero_to_one(gen), zero_to_one(gen))
            });
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

            physicsSystem->update((float)deltaTime);
        }
        /* Render update */
        renderTick = (currTime - renderTime > 1.0f / (RENDER_FREQ));
        if (renderTick) {
            renderTime = currTime;
            glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
            
            renderSystem->update((float)deltaTime);

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

        gameWindow->updateKeyboard();

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