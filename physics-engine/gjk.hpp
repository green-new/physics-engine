#pragma once
#include "geometry.hpp"

const Geometry::vertex& support(Geometry::Geometry3D shape, glm::vec3 d = glm::vec3(0.0f));
float GJK_intersection(Geometry::Geometry3D p, Geometry::Geometry3D q, glm::vec3 inital_axis = glm::vec3(0.0f));