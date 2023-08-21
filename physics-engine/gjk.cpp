#include "gjk.hpp"

// Shape must be in world coordinates.
const glm::vec3 Physics::support(std::vector<glm::vec3> shape, glm::vec3 d) {
	float dot = -1.0f, temp;
	glm::vec3 support;
	for (const auto& vertex : shape) {
		float temp = glm::dot(vertex, d);
		if (temp > dot) {
			support = vertex;
			dot = temp;
		}
	}

	return support;
}

bool Physics::gjk(Collider* p, Collider* q, glm::vec3 initial_axis) {
	glm::vec3 A = p->support(initial_axis) - q->support(initial_axis);
	Simplex simplex;
	simplex.pushFront(A);
	glm::vec3 newDir = -A;

	while (true) {
		A = p->support(newDir) - q->support(newDir);
		if (glm::dot(A, newDir) < 0) {
			return false;
		}
		simplex.pushFront(A);
		if (nearestSimplex(simplex, newDir)) {
			return true;
		}
	}
}

bool Line(Physics::Simplex& simplex, glm::vec3& direction) {
	// Line simplex has three regions to check if it contains the origin or not.
	// To the left of B does not contain the origin because we moved to A which was in the direction of the origin.
	// Therefore we need to check the middle and far right (past A) regions to determine if the origin is contained.
	glm::vec3 a = simplex[0];
	glm::vec3 b = simplex[1];

	// Directions
	glm::vec3 ab = b - a;	// A to B.
	glm::vec3 ao = -a;		// A to the origin.

	if (Physics::sameDirection(ab, ao)) {
		direction = glm::cross(glm::cross(ab, ao), ab);
	} else {
		simplex = { a };
		// If not in the same direction, just point in to the origin.
		direction = ao;
	}
	return false;
}

bool Triangle(Physics::Simplex& simplex, glm::vec3& direction) {
	glm::vec3 a = simplex[0];
	glm::vec3 b = simplex[1];
	glm::vec3 c = simplex[2];

	glm::vec3 ab = b - a;
	glm::vec3 ac = c - a;
	glm::vec3 ao = -a;

	glm::vec3 abc = glm::cross(ab, ac);

	if (Physics::sameDirection(glm::cross(abc, ac), ao)) {
		if (Physics::sameDirection(ac, ao)) {
			simplex = { a, c };
			direction = glm::cross(glm::cross(ac, ao), ac);
		} else {
			return Line(simplex = { a, b }, direction);
		}
	}

	else {
		if (Physics::sameDirection(glm::cross(ab, abc), ao)) {
			return Line(simplex = { a, b }, direction);
		} else {
			if (Physics::sameDirection(abc, ao)) {
				direction = abc;
			}

			else {
				simplex = { a, c, b };
				direction = -abc;
			}
		}
	}
	return false;
}

bool Tetrahedron(Physics::Simplex& simplex, glm::vec3& direction) {
	glm::vec3 a = simplex[0];
	glm::vec3 b = simplex[1];
	glm::vec3 c = simplex[2];
	glm::vec3 d = simplex[3];

	glm::vec3 ab = b - a;
	glm::vec3 ac = c - a;
	glm::vec3 ad = d - a;
	glm::vec3 ao = -a;

	glm::vec3 abc = glm::cross(ab, ac);
	glm::vec3 acd = glm::cross(ac, ad);
	glm::vec3 adb = glm::cross(ad, ab);

	if (Physics::sameDirection(abc, ao)) {
		return Triangle(simplex = { a, b, c }, direction);
	}

	if (Physics::sameDirection(acd, ao)) {
		return Triangle(simplex = { a, c, d }, direction);
	}

	if (Physics::sameDirection(adb, ao)) {
		return Triangle(simplex = { a, d, b }, direction);
	}

	return true;
}

bool Physics::nearestSimplex(Simplex& simplex, glm::vec3& direction) {
	switch (simplex.size()) {
		case 2: return Line(simplex, direction);
		case 3: return Triangle(simplex, direction);
		case 4: return Tetrahedron(simplex, direction);
	}

	return false;
}

