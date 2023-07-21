#pragma once
#include "systems.hpp"

namespace Systems {
	class CameraSystem : public System {
	public:
		void update(float deltaTime) override;
	};
}