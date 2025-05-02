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
#include "box.hpp"

extern Entity mCamera;

const uint16_t width = 1920;
const uint16_t height = 1080;

bool paused = false;
bool updateGravity = false;
bool updateNormals = false;
bool updatePolygonMode = false;
bool spawnEntity = false;
bool drawBoxes = false;
GLenum polygonMode = GL_FILL;

std::unique_ptr<Window> gameWindow;
std::unique_ptr<Resources::ResourceManager> resourceManager;
Coordinator gCoordinator;

void main_input(const int key, const int scancode, const int action, const int mods) {
    if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS) {
        glfwSetWindowShouldClose(gameWindow->getHandle(), true);
    }
    if (key == GLFW_KEY_Q && action == GLFW_PRESS) {
        updateGravity = true;
    }
    if (key == GLFW_KEY_P && action == GLFW_PRESS) {
        spawnEntity = true;
    }
    if (key == GLFW_KEY_H && action == GLFW_PRESS) {
        drawBoxes = true;
    }
    if (key == GLFW_KEY_B && action == GLFW_PRESS) {
        updatePolygonMode = true;
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
    gameWindow->getKeyboardManager().subscribe(&main_input, {GLFW_KEY_ESCAPE, GLFW_KEY_Q, GLFW_KEY_P, GLFW_KEY_H});
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
    const float PHYS_FREQ = 1000.0f; // 1000 Hz
    const float RENDER_FREQ = 1000.0f;  // 500 Hz

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
    {   // Set the physics system signature
        Signature signature;
        signature.set(gCoordinator.getComponentType<Components::RigidBody>());
        signature.set(gCoordinator.getComponentType<Components::Transform>());
        gCoordinator.setSystemSignature<Systems::PhysicsSystem>(signature);
    }

    auto renderSystem = gCoordinator.registerSystem<Systems::RenderSystem>();
    {   // Set the render system signature
        Signature signature;
        signature.set(gCoordinator.getComponentType<Components::Appearence>());
        signature.set(gCoordinator.getComponentType<Components::Transform>());
        signature.set(gCoordinator.getComponentType<Components::RenderShape>());
        gCoordinator.setSystemSignature<Systems::RenderSystem>(signature);
    }

    physicsSystem->init();
    renderSystem->init();

    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_real_distribution<> x_dist(-20.0f, 20.0f);
    std::uniform_real_distribution<> y_dist(-20.0f, 20.0f);
    std::uniform_real_distribution<> z_dist(-20.0f, 20.0f);
    std::uniform_real_distribution<float> zero_to_one(0.0f, 1.0f);
    std::uniform_real_distribution<float> one_to_100(0.5f, 1.0f);
    std::uniform_real_distribution<float> neg_to_pos(-100.0f, 100.0f);
    std::uniform_int_distribution<> shape_dist(0, 3);

    std::shared_ptr<Components::RenderShape> shape = std::make_shared<Components::RenderShape>(Components::RenderShape{
        .Shape = new Mesh3D(GLDataAdapter(resourceManager->getGeometry("sphere")).requestData()),
        .BoxShape = new Mesh3D(GLDataAdapter(resourceManager->getGeometry("cube")).requestData())
        });
    for (uint32_t i = 0; i < 100; i++) {
        Entity entity = gCoordinator.createEntity();
        const float mass = one_to_100(gen);
        gCoordinator.addComponent(entity, Components::Transform{
            .Position = glm::vec3(x_dist(gen), y_dist(gen), z_dist(gen)),
            .Rotation = glm::vec3(0.0f),
            .Scale = glm::vec3(1.0f) * mass,
            .RotationAngle = 0.0f
            });
        unsigned int index = shape_dist(gen);
        gCoordinator.addComponent(entity, Components::RenderShape{
            .Shape = shape.get()->Shape,
            .BoxShape = shape.get()->BoxShape,
            });
        gCoordinator.addComponent(entity, Components::Appearence{
            .Texture = resourceManager->getTexture("better_cloud"),
            .Opacity = zero_to_one(gen),
            .Reflectance = 0.0f,
            .BaseColor = glm::vec3(zero_to_one(gen), zero_to_one(gen), zero_to_one(gen))
            });
        gCoordinator.addComponent(entity, Components::RigidBody{
            .Box = BoundingBox(glm::vec3 {-mass}, glm::vec3 {mass}),
            .Shape = resourceManager->getGeometry("cube"),
            .Anchored = false,
            .onGround = false,
            .Mass = mass,
            .Velocity = glm::vec3(0.0f),
            .Force = glm::vec3(neg_to_pos(gen), neg_to_pos(gen), neg_to_pos(gen)),
            });
    }

    /* Loop until the user closes the window */
    while (!glfwWindowShouldClose(gameWindow->getHandle())) {
        frames++;
        /* Time */
        currTime = glfwGetTime();
        deltaTime = currTime - prevTime;

        /* Physics update */
        physTick = (currTime - physTime > (1.0f / PHYS_FREQ));
        if (physTick) {
            physTime = currTime;

            physicsSystem->update((float)deltaTime);

            /* Poll for and process events */
            // glfwPollEvents();
        }
        /* Render update */
        renderTick = (currTime - renderTime > (1.0f / RENDER_FREQ));
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

        /* Random update stuff */
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

        if (updateGravity) {
            physicsSystem->switchGravity();
            updateGravity = false;
        }

        if (drawBoxes) {
            renderSystem->toggleBoxRendering();
            drawBoxes = false;
        }

        if (spawnEntity) {
            spawnEntity = false;

            Entity entity = gCoordinator.createEntity();
            const float mass = one_to_100(gen);
            const glm::vec3 force = gCoordinator.getComponent<Components::Orientation>(mCamera).Front * 1000.0f;
            gCoordinator.addComponent(entity, Components::Transform{
                .Position = gCoordinator.getComponent<Components::Transform>(mCamera).Position + gCoordinator.getComponent<Components::Orientation>(mCamera).Front * 2.5f,
                .Rotation = glm::vec3(0.0f),
                .Scale = glm::vec3(1.0f) * mass,
                .RotationAngle = 0.0f
                });
            unsigned int index = shape_dist(gen);
            gCoordinator.addComponent(entity, Components::RenderShape{
                .Shape = shape.get()->Shape,
                .BoxShape = shape.get()->BoxShape,
                });
            gCoordinator.addComponent(entity, Components::Appearence{
                .Texture = resourceManager->getTexture("cloud"),
                .Opacity = zero_to_one(gen),
                .Reflectance = 0.0f,
                .BaseColor = glm::vec3(zero_to_one(gen), zero_to_one(gen), zero_to_one(gen))
                });
            gCoordinator.addComponent(entity, Components::RigidBody{
                .Box = BoundingBox(glm::vec3 {-mass}, glm::vec3 {mass}),
                .Shape = resourceManager->getGeometry("sphere"),
                .Anchored = false,
                .Mass = mass,
                .Velocity = glm::vec3(0.0f),
                .Force = force,
                });
        }

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