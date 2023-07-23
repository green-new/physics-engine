#include "geometry.hpp"

namespace Geometry {

	Geometry3D::Geometry3D() {
		vertices = std::vector<Geometry::vertex>();
		faces = std::vector<Geometry::face>();
	}
	Geometry3D::~Geometry3D() {
	}
	std::vector<vertex>& Geometry3D::getVertices() {
		return vertices;
	}
	std::vector<face>& Geometry3D::getFaces() {
		return faces;
	}
	vertex& Geometry3D::getVertex(size_t index) {
		return vertices.at(index);
	}
	face& Geometry3D::getFace(size_t index) {
		return faces.at(index);
	}
	bool Geometry3D::isEmpty() const {
		return vertices.empty() && faces.empty();
	}
	size_t Geometry3D::getFaceCount() const {
		return faces.size();
	}
	size_t Geometry3D::getVertexCount() const {
		return vertices.size();
	}
	/*
	Calculates and updates both the flat and smooth shading of the vertex normals.
	If they are already precomputed (e.g., from file) this function is exited.
	Algorithm from https://computergraphics.stackexchange.com/questions/4031/programmatically-generating-vertex-normals
	*/
	void Builder::calculate() {
		auto& g = *geometry.get();
		// Set all normals to zero if swapping to smooth shading
		for (auto& v : g.getVertices()) {
			v.normal = glm::vec3(0.0f);
		}
		// Smooth normal (vertex normals)
		for (auto& f : g.getFaces()) {
			vertex& A = g.getVertex(f.indices[0]);
			vertex& B = g.getVertex(f.indices[1]);
			vertex& C = g.getVertex(f.indices[2]);
			glm::vec3 Ap = A.position;
			glm::vec3 Bp = B.position;
			glm::vec3 Cp = C.position;
			glm::vec3 normal = glm::cross(Bp - Ap, Cp - Ap);
			normal = glm::normalize(normal);
			A.normal += normal;
			B.normal += normal;
			C.normal += normal;
		}
		for (auto& v : g.getVertices()) {
			glm::normalize(v.normal);
		}
	}

	ProceduralBuilder::ProceduralBuilder() {
		geometry.reset(new Geometry3D);
	}

	ProceduralBuilder& ProceduralBuilder::addTriangle(std::array<Geometry::vertex, 3> triangle) {
		addVertex(triangle[0]);
		size_t idx0 = getLastIndex();
		addVertex(triangle[1]);
		size_t idx1 = getLastIndex();
		addVertex(triangle[2]);
		size_t idx2 = getLastIndex();

		// Calculates the flat normal
		Geometry::vertex& A = triangle[0];
		Geometry::vertex& B = triangle[1];
		Geometry::vertex& C = triangle[2];
		glm::vec3 Ap = A.position;
		glm::vec3 Bp = B.position;
		glm::vec3 Cp = C.position;
		glm::vec3 normal = glm::cross(Bp - Ap, Cp - Ap);
		normal = glm::normalize(normal);

		Geometry::face tempFace = { .indices = glm::ivec3(idx0, idx1, idx2), .flatNormal = normal };

		addFace(tempFace);

		return *this;
	}

	/* quad must be in this order:
	- bottom left
	- top left
	- top right
	- bottom right */
	ProceduralBuilder& ProceduralBuilder::addQuad(std::array<Geometry::vertex, 4> quad) {
		std::array<Geometry::vertex, 3> triangle0 = { quad[0], quad[1], quad[2] };
		std::array<Geometry::vertex, 3> triangle1 = { quad[0], quad[2], quad[3] };
		addTriangle(triangle0);
		addTriangle(triangle1);

		return *this;
	}

	ProceduralBuilder& ProceduralBuilder::addPolygon(std::array<Geometry::vertex, GEOLIB_MAX_POLYGON_SIDES> polygon, size_t sides) {
		assert(sides < GEOLIB_MAX_POLYGON_SIDES&& sides > 3);
		Geometry::vertex base = polygon[0];
		unsigned short triangles = sides - 2U;

		// Fan triangulation algorithm
		for (unsigned short i = 1; i <= triangles; i++) {
			std::array<Geometry::vertex, 3> triangle = { base, polygon[i], polygon[i + 1] };
			addTriangle(triangle);
		}

		return *this;
	}

	AssetBuilder::AssetBuilder() {
	}

	AssetBuilder& AssetBuilder::load(std::string asset) {
		return *this;
	}

	GLDataAdapter::GLDataAdapter(Geometry3D& adaptee) : geometryData(adaptee) {
	}
	/* Gets the GL appropriate data from the current geometry context.
	We have to loop through the faces, not the vertices, because the faces 
	contain the correct order for which the vertices are drawn.
	This is important because we have no EBO (index buffer) implementation due to its complexity with 
	changing vertex size and normal size when swapping from flat and smooth normals. */
	GLData GLDataAdapter::requestData() {
		GLData g;
		for (auto& f : geometryData.getFaces()) {
			vertex& A = geometryData.getVertex(f.indices.x);
			vertex& B = geometryData.getVertex(f.indices.y);
			vertex& C = geometryData.getVertex(f.indices.z);
			g.smoothVertices.push_back(A.position.x);
			g.smoothVertices.push_back(A.position.y);
			g.smoothVertices.push_back(A.position.z);
			g.smoothVertices.push_back(B.position.x);
			g.smoothVertices.push_back(B.position.y);
			g.smoothVertices.push_back(B.position.z);
			g.smoothVertices.push_back(C.position.x);
			g.smoothVertices.push_back(C.position.y);
			g.smoothVertices.push_back(C.position.z);
			g.smoothVertices.push_back(A.normal.x);
			g.smoothNormals.push_back(A.normal.y);
			g.smoothNormals.push_back(A.normal.z);
			g.smoothNormals.push_back(B.normal.x);
			g.smoothNormals.push_back(B.normal.y);
			g.smoothNormals.push_back(B.normal.z);
			g.smoothNormals.push_back(C.normal.x);
			g.smoothNormals.push_back(C.normal.y);
			g.smoothNormals.push_back(C.normal.z);
		}
		for (const auto& f : geometryData.getFaces()) {
			vertex& A = geometryData.getVertex(f.indices.x);
			vertex& B = geometryData.getVertex(f.indices.y);
			vertex& C = geometryData.getVertex(f.indices.z);
			g.flatNormals.push_back(f.flatNormal.x);
			g.flatNormals.push_back(f.flatNormal.y);
			g.flatNormals.push_back(f.flatNormal.z);
			g.flatNormals.push_back(f.flatNormal.x);
			g.flatNormals.push_back(f.flatNormal.y);
			g.flatNormals.push_back(f.flatNormal.z);
			g.flatNormals.push_back(f.flatNormal.x);
			g.flatNormals.push_back(f.flatNormal.y);
			g.flatNormals.push_back(f.flatNormal.z);
			g.flatVertices.push_back(A.position.x);
			g.flatVertices.push_back(A.position.y);
			g.flatVertices.push_back(A.position.z);
			g.flatVertices.push_back(B.position.x);
			g.flatVertices.push_back(B.position.y);
			g.flatVertices.push_back(B.position.z);
			g.flatVertices.push_back(C.position.x);
			g.flatVertices.push_back(C.position.y);
			g.flatVertices.push_back(C.position.z);
		}

		return g;
	}
}