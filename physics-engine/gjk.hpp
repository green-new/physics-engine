#pragma once
#include "geometry.hpp"
#include "box.hpp"

namespace Physics {

	// Credits to winter dev!
	class Simplex {
	private:
		std::array<glm::vec3, 4> points;
		unsigned size;
	public:
		Simplex() : points({ glm::vec3(0.0f), glm::vec3(0.0f), glm::vec3(0.0f), glm::vec3(0.0f) }), size(0) {}

		Simplex& operator=(std::initializer_list<glm::vec3> list) {
			for (auto v = list.begin(); v != list.end(); v++) {
				points[std::distance(list.begin(), v)] = *v;
			}
			size = list.size();
			return *this;
		}

		void pushFront(glm::vec3 point) {
			points = { point, points[0], points[1], points[2] };
			size = std::min(size + 1, 4u);
		}

		glm::vec3& operator[](unsigned int i) { return points[i]; }
		unsigned int len() const { return size; }

		auto begin() const { return points.begin(); }
		auto end() const { return points.end() - (4 - size); }
	};

	class Collider {
	private:
		BoundingBox m_region;
	public:
		const BoundingBox& getRegion() const {
			return m_region;
		}
		virtual glm::vec3 support(glm::vec3 direction) const = 0;
	};

	class MeshCollider : public Collider {
	private:
		std::vector<glm::vec3> vertices;
	public:
		glm::vec3 support(glm::vec3 direction) const override {
			float dot = -FLT_MAX, temp = 0.0f;
			glm::vec3 support = glm::vec3(0.0f);
			for (const auto& vertex : vertices) {
				float temp = glm::dot(vertex, direction);
				if (temp > dot) {
					support = vertex;
					dot = temp;
				}
			}

			return support;
		}
	};
	bool sameDirection(const glm::vec3& direction, const glm::vec3& ao);
	bool gjk(Collider* p, Collider* q, glm::vec3 inital_axis = glm::vec3(0.0f));
	bool nearestSimplex(Simplex& simplex, glm::vec3& direction);
}