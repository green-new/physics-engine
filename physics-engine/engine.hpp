#pragma once
#include "config.hpp"
#include "clock.hpp"
#include "window.hpp"
#include "coordinator.hpp"
#include "render.hpp"
#include "physics_sim.hpp"

class Engine {
public:
    Engine() :
        m_config(),
        m_clock(),
        m_frames(0),
        m_running(false),
        m_coordinator(),
        m_resourceManager(),
        m_mouseManager(),
        m_keyboardManager(),
        m_window(m_config.getWidth(), m_config.getHeight(), "3d engine", m_mouseManager, m_keyboardManager),
        m_physics(
            m_coordinator.registerSystem<Systems::PhysicsSystem>(
                m_coordinator
            ),
            std::make_shared<double>(m_clock.m_physDelta)
        ),
        m_render(
            m_coordinator.registerSystem<Systems::RenderSystem>(
                m_coordinator,
                m_keyboardManager,
                m_mouseManager,
                m_resourceManager
            ),
            std::make_shared<double>(m_clock.m_renderDelta),
            m_resourceManager
        ) {}

    ~Engine() = default;

public:
    void init();
    void run();
    void destroy();

private:
    void mainInput(const int key, const int scancode, const int action, const int mods);
    void mainMouseInput(const Input::MouseState& ms);

private:
    Config                      m_config;
    Clock                       m_clock;
    unsigned long long          m_frames;
    bool                        m_running;

    Coordinator                 m_coordinator;
    Resources::ResourceManager  m_resourceManager;
    Input::MouseManager         m_mouseManager;
    Input::KeyboardManager      m_keyboardManager;
    Window                      m_window;

    PhysicsSim                  m_physics;
    Render                      m_render;
};