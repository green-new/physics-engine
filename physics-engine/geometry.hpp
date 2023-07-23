#pragma once
#include <string>
#include <numbers>
#include <unordered_map>
#include <sstream>
#include <iostream>
#include <fstream>
#include <memory>
#include <vector>
#include <array>
#include <type_traits>
#include <assert.h>
#include <glad/glad.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

/* Triangulation is a relatively expensive operation.
For procedurally generated geometry, a face can only have 12 sides. */
#define GEOLIB_MAX_POLYGON_SIDES	12

namespace Geometry {
	struct vertex {
		glm::vec3 position;
		glm::vec3 normal;
		glm::vec2 texture;
	};

	struct face {
		glm::ivec3 indices;
		glm::vec3 flatNormal;
	};

	class Simplex3D {
		bool containsOrigin();
	private:
		std::array<glm::vec3, 4> vertices;
	};

	class Geometry3D {
	public:
		Geometry3D();
		~Geometry3D();

		size_t getFaceCount() const;
		size_t getVertexCount() const;

		std::vector<vertex>& getVertices();
		std::vector<face>& getFaces();

		vertex& getVertex(size_t index);
		face& getFace(size_t index);

		bool isEmpty() const;

	private:
		friend class Builder;

		std::vector<vertex> vertices;
		std::vector<face> faces;
	};

	class Builder {
	public:
		Builder() {
			geometry.reset(new Geometry3D);
		}
		virtual ~Builder() = default;
		virtual std::unique_ptr<Geometry3D> build() {
			return std::move(geometry);
		}
		virtual size_t getLastIndex() {
			return geometry.get()->getVertexCount() - 1;
		}
		virtual Builder& addVertex(Geometry::vertex vertex) {
			geometry.get()->vertices.push_back(vertex);
			return *this;
		}
		virtual Builder& addFace(Geometry::face face) {
			geometry.get()->faces.push_back(face);
			return *this;
		}
		class ProceduralBuilder;
		class AssetBuilder;
	protected:
		void calculate();
		std::unique_ptr<Geometry3D> geometry;
	};

	class AssetBuilder : public Builder {
	public:
		AssetBuilder();
		AssetBuilder& load(std::string asset);
	};

	class ProceduralBuilder : public Builder {
	public:
		ProceduralBuilder();
		ProceduralBuilder& addTriangle(std::array<Geometry::vertex, 3> triangle);
		ProceduralBuilder& addQuad(std::array<Geometry::vertex, 4> quad);
		ProceduralBuilder& addPolygon(std::array<Geometry::vertex, GEOLIB_MAX_POLYGON_SIDES> polygon, size_t sides);
	};

	/* For use with the adapter strategy. */
	struct GLData {
		std::vector<GLfloat> smoothVertices;
		std::vector<GLfloat> smoothNormals;
		std::vector<GLfloat> flatVertices;
		std::vector<GLfloat> flatNormals;
		std::vector<GLfloat> textureData;
	};
	class GLDataAdapter {
	public:
		GLDataAdapter(Geometry3D& adaptee);
		GLData requestData();
		Geometry3D& geometryData;
	};
}
