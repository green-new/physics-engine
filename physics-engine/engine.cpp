#include "engine.hpp"
#include "components.hpp"

void Engine::init() {
	std::cout << "--- [ Controls ] ---\n";
	std::cout << "[ESC] \t\t\t\t--- Exit the program\n";
	std::cout << "[W, A, S, D] \t\t\t--- Move the camera\n";
	std::cout << "[F] \t\t\t\t--- Toggle flat vs smooth shading lighting\n";
	std::cout << "[Q] \t\t\t\t--- Toggle between GL_FILL, GL_POINT, and GL_LINE\n";

	try {
		/* initialize the window */ 
		m_window.init();

		/* initialize glad / glfw after window load */
		if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
			std::cout << "[GLFW] Error loading GLFW\n";
		}

		/* get all engine resources before continuing */
		m_resourceManager.init();

		/* initialize the ECS coordinator */
		m_coordinator.registerComponent<Components::Appearence>();
		m_coordinator.registerComponent<Components::Camera>();
		m_coordinator.registerComponent<Components::Orientation>();
		m_coordinator.registerComponent<Components::RigidBody>();
		m_coordinator.registerComponent<Components::RenderShape>();
		m_coordinator.registerComponent<Components::Transform>();
		m_coordinator.registerComponent<Components::PointLight>();

		/* Set physics system component signature */
		{
			Signature signature;
			signature.set(m_coordinator.getComponentType<Components::RigidBody>());
			signature.set(m_coordinator.getComponentType<Components::Transform>());
			m_coordinator.setSystemSignature<Systems::PhysicsSystem>(signature);
		}

		/* Set render system component signature */
		{
			Signature signature;
			signature.set(m_coordinator.getComponentType<Components::Appearence>());
			signature.set(m_coordinator.getComponentType<Components::Transform>());
			signature.set(m_coordinator.getComponentType<Components::RenderShape>());
			m_coordinator.setSystemSignature<Systems::RenderSystem>(signature);
		}

		/* Main game inputs */
		m_keyboardManager.subscribe(
			[this](const int key, const int scancode, const int action, const int mods) {
				return this->mainInput(key, scancode, action, mods);
			}, { GLFW_KEY_ESCAPE });
		m_mouseManager.subscribe(
			[this](const Input::MouseState& ms) {
				return this->mainMouseInput(ms);
			}
		);

		/* Nothing went wrong, set running to true */
		m_running = true;

		/* Init the render and physics systems */
		m_render.init();
		m_physics.init();

		/* Create a ton of entities */
		std::random_device rd;
		std::mt19937 gen(rd());
		std::uniform_real_distribution<> x_dist(-10.0f, 10.0f);
		std::uniform_real_distribution<> y_dist(-10.0f, 10.0f);
		std::uniform_real_distribution<> z_dist(-10.0f, 10.0f);
		std::uniform_real_distribution<float> zero_to_one(0.0f, 1.0f);
		std::uniform_real_distribution<float> one_to_100(0.5f, 1.0f);
		std::uniform_real_distribution<float> neg_to_pos(-1.0f, 1.0f);
		std::uniform_int_distribution<> shape_dist(0, 3);

		std::shared_ptr<Components::RenderShape> shape = std::make_shared<Components::RenderShape>(Components::RenderShape{
			.Shape = new Mesh3D(GLDataAdapter(m_resourceManager.getGeometry("sphere")).requestData()),
			.BoxShape = new Mesh3D(GLDataAdapter(m_resourceManager.getGeometry("cube")).requestData())
			});

		std::shared_ptr<Components::RenderShape> cube = std::make_shared<Components::RenderShape>(Components::RenderShape{
			.Shape = new Mesh3D(GLDataAdapter(m_resourceManager.getGeometry("cube")).requestData()),
			.BoxShape = new Mesh3D(GLDataAdapter(m_resourceManager.getGeometry("cube")).requestData())
			});
		for (uint32_t i = 0; i < 50; i++) {
			Entity entity = m_coordinator.createEntity();
			const float mass = one_to_100(gen);
			m_coordinator.addComponent(entity, Components::Transform{
				.Position = glm::vec3(x_dist(gen), y_dist(gen), z_dist(gen)),
				.Rotation = glm::vec3(0.0f),
				.Scale = glm::vec3(1.0f) * mass,
				.RotationAngle = 0.0f
				});
			unsigned int index = shape_dist(gen);
			m_coordinator.addComponent(entity, Components::RenderShape{
				.Shape = shape.get()->Shape,
				.BoxShape = shape.get()->BoxShape,
				});
			m_coordinator.addComponent(entity, Components::Appearence{
				.Texture = m_resourceManager.getTexture("better_cloud"),
				.Opacity = zero_to_one(gen),
				.Reflectance = 0.0f,
				.BaseColor = glm::vec3(zero_to_one(gen), zero_to_one(gen), zero_to_one(gen))
				});
			m_coordinator.addComponent(entity, Components::RigidBody{
				.Box = BoundingBox(glm::vec3(-0.5f), glm::vec3(0.5f)),
				.Shape = m_resourceManager.getGeometry("cube"),
				.Anchored = false,
				.onGround = false,
				.Mass = mass,
				.Restitution = 1.0f,
				.Velocity = glm::vec3(0.0f),
				.Force = glm::vec3(neg_to_pos(gen), neg_to_pos(gen), neg_to_pos(gen)),
				});
		}

	} catch (std::exception& err) {
		m_running = false;
		std::cerr << err.what();
	}
}

void Engine::run() {
	while (m_running) {
		/* Update the game time */
		m_clock.update();
		
		/* Run physics sim */
		if (m_clock.isPhysicsTick()) {
			m_physics.simulate();
		}

		/* Run render */
		if (m_clock.isRenderTick()) {
			m_render.render();

			m_window.swapBuffers();

			/* Increment the number of frames displayed */
			m_frames++;
		}

		/* Run fps timer */
		if (m_clock.isFpsTick()) {
			/* Calculate the fps */
			double fps = m_frames;

			/* Display the fps to console, for now */
			std::cout << "FPS: " << fps << '\n';

			m_frames = 0;
		}

		/* Poll for and process incoming events */
		glfwPollEvents();
	}
}

void Engine::destroy() {
	m_window.destroy();
	glfwTerminate();
}

void Engine::mainInput(const int key, const int scancode, const int action, const int mods) {
	// m_keyboardManager.update(key, scancode, action, mods);
	if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS) {
		m_window.close();
		m_running = false;
	}
}

void Engine::mainMouseInput(const Input::MouseState& ms) {
	// m_mouseManager.update(x, y);
}
