#pragma once
#include "mesh.hpp"

namespace Components {
	struct RenderShape {
		Mesh3D* Shape;
		Mesh3D* BoxShape;
	};
}