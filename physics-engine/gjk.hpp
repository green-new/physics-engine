#pragma once
#include "geometry.hpp"

namespace Physics {
	typedef struct Environment {

	} Environment;
	const glm::vec3 support(Geometry::Geometry3D shape, glm::vec3 d = glm::vec3(0.0f));
	Geometry::Simplex3D minkowskiDiff(Geometry::Geometry3D s0, Geometry::Geometry3D s1);
	bool gjk(Geometry::Geometry3D p, Geometry::Geometry3D q, glm::vec3 inital_axis = glm::vec3(0.0f));
}