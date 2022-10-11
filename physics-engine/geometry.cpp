#include "geometry.hpp"

namespace geolib {
	/*	=========================================
	*	geometry implementation
		=========================================*/
	geometry::geometry() { 
		vertices = std::vector<vertex>();
		faces = std::vector<face>();
	}
	geometry::~geometry() { 
		clear();
	}
	/*
	Adds a vertex to the face list.

	@param v Vertex struct.
	*/
	void geometry::add_vertex(vertex v) {
		vertices.push_back(v);
	}
	/*
	Adds a face to the face list.

	@param f Face struct.
	*/
	void geometry::add_face(face f) {
		faces.push_back(f);
	}
	/*
	Determines if this geometry is empty (no vertices and no faces).
	this is a const reference.
	*/
	bool geometry::is_empty() const {
		return vertices.empty() && faces.empty();
	}
	/*
	Gets the face count of this geometry.
	@return unsigned int Number of faces.
	*/
	unsigned int geometry::get_face_count() {
		return faces.size();
	}
	/*
	Gets the vertex count of this geometry.
	@return unsigned int Number of vertices.
	*/
	unsigned int geometry::get_vertex_count() {
		return vertices.size();
	}
	/*
	Calculates and updates both the flat and smooth shading of the vertex normals.
	If they are already precomputed (e.g., from file) this function is exited.
	Algorithm from https://computergraphics.stackexchange.com/questions/4031/programmatically-generating-vertex-normals
	*/
	void geometry::calc_normals() {
		// Set all normals to zero if swapping to smooth shading
		for (auto& v : vertices) {
			v.smoothNormal = glm::vec3(0.0f);
		}
		// Smooth normal
		for (auto& f : faces) {
			vertex& A = get_vertex(f.indices[0]);
			vertex& B = get_vertex(f.indices[1]);
			vertex& C = get_vertex(f.indices[2]);
			glm::vec3 Ap = A.position;
			glm::vec3 Bp = B.position;
			glm::vec3 Cp = C.position;
			glm::vec3 normal = glm::cross(Bp - Ap, Cp - Ap);
			glm::normalize(normal);
			A.smoothNormal += normal;
			B.smoothNormal += normal;
			C.smoothNormal += normal;
		}
		for (auto& v : vertices) {
			glm::normalize(v.smoothNormal);
		}
		// Flat normals
		for (auto& v : vertices) {
			v.flatNormal = glm::vec3(0.0f);
		}
		for (auto& f : faces) {
			vertex& A = get_vertex(f.indices[0]);
			vertex& B = get_vertex(f.indices[1]);
			vertex& C = get_vertex(f.indices[2]);
			glm::vec3 Ap = A.position;
			glm::vec3 Bp = B.position;
			glm::vec3 Cp = C.position;
			glm::vec3 normal = glm::cross(Bp - Ap, Cp - Ap);
			glm::normalize(normal);
			A.flatNormal = normal;
			B.flatNormal = normal;
			C.flatNormal = normal;
		}
	}
	void geometry::clear() {
		vertices.clear();
		faces.clear();
	}

	/*	=========================================
	*	vertex_strategy_objreader implementation
		=========================================*/

	template<int L, typename S>
	vec_strategy<L, S>::vec_strategy() {
		static_assert(std::is_same<S, float>::value);
		static_assert(std::is_same<S, int>::value);
		static_assert(std::is_same<S, short>::value);
		_data = { 0, 0, 0 };
	}

	template<int L, typename S>
	glm::vec<L, S> vec_strategy<L, S>::execute(std::stringstream& stream) {
		std::string element;
		if (_data.max_size() != L) return;
		try {
			for (int i = 0; i < L; i++) {
				if (std::is_same<S, float>::value) {
					stream >> element;
					_data.at(i) = stof(element);
				}
				else if (std::is_same<S, unsigned int>::value) {
					stream >> element;
					_data.at(i) = (unsigned int)stoi(element);
				}
			}
		}
		catch (std::invalid_argument) {
			throw std::invalid_argument("Error converting OBJ file: invalid argument, datatype, element, or value for [" + element + "].\n");
		}
		catch (std::out_of_range) {
			throw std::out_of_range("Error converting OBJ file: out of range number values for [" + element + "].\n");
		}
		return glm::vec<L, S>(_data);
	}
	/* Liskov principle (upcasting) */
	geometry geometry_creator::build() {
		return (geometry)*this;
	}
	/*	=========================================
	*	geometry_obj implementation
		=========================================*/
	/*
	Instantiates a geometry object from given .obj file.
	https://people.cs.clemson.edu/~dhouse/courses/405/docs/brief-obj-file-format.html
	*/
	geometry_obj::geometry_obj(std::string filename) {
		using namespace std;
		file = fstream(filename);

		/* If not .obj or not opened, get out! */
		if (filename.substr(filename.find_last_of(".") + 1) != "obj") throw std::logic_error("Error reading OBJ file: file was not .OBJ file type");
		if (!file.is_open()) throw std::runtime_error("Error reading OBJ file: file could not be opened");

		string line = "";
		while (getline(file, line)) {
			stringstream stream(line);
			string element;
			if (!stream) continue;
			while (stream >> element) {
				if (!element.starts_with("v") &&
					!element.starts_with("f") &&
					!element.starts_with("vn") ||
					element.starts_with("#") ||
					element.empty()) break;
				switch (map.at(element)) {
				case VERTEX: {
					glm::vec3 position = vec3f_strategy.execute(stream);
					vertex* v = new vertex;
					if (v) { /* nullptr check */
						v->position = position;
						this->add_vertex(*v);
						vIndex++;
					}
					continue;
				}
				case VERTEX_TEXTURES: {
					glm::vec2 texture = vec2f_strategy.execute(stream);
					if (vtIndex <= vIndex && &vertices[vtIndex]) {
						vertices[vtIndex].texture = texture;
						texturesWerePredefined = true;
						vtIndex++;
					}
					continue;
				}
				case VERTEX_NORMALS: {
					glm::vec3 normal = vec3f_strategy.execute(stream);
					if (vnIndex <= vIndex && &vertices[vnIndex]) {
						vertices[vnIndex].smoothNormal = normal;
						normalsWerePredefined = true;
						vnIndex++;
					}
					continue;
				}
				case FACE: {
					glm::uvec3 indices = vec3i_strategy.execute(stream);
					face* f = new face;
					if (f) {
						f->indices = indices;
						add_face(*f);
						fIndex++;
					}
					continue;
				}
				default:
					continue;
				}
			}
		}
	}

	/*	=========================================
	 *	geometry_procedural implementation
		=========================================*/

	geometry_procedural::geometry_procedural() { }
	unsigned int geometry_procedural::add_vertex(float v0, float v1, float v2) {
		glm::vec3 position = glm::vec3(v0, v1, v2);
		vertex* v = new vertex;
		v->position = position;
		(geometry(*this)).add_vertex(*v);

		return this->get_vertex_count() - 1;
	}
	void geometry_procedural::set_smooth_normal(unsigned int idx, float v0, float v1, float v2) {
		glm::vec3 smoothNormal = glm::normalize(glm::vec3(v0, v1, v2));
		get_vertex(idx).smoothNormal = smoothNormal;
	}
	void geometry_procedural::set_flat_normal(unsigned int idx, float v0, float v1, float v2) {
		glm::vec3 flatNormal = glm::normalize(glm::vec3(v0, v1, v2));
		get_vertex(idx).flatNormal = flatNormal;
	}
	void geometry_procedural::set_flat_normal(unsigned int idx, float v0, float v1, float v2) {
		set_smooth_normal(idx, v0, v1, v2);
		set_flat_normal(idx, v0, v1, v2);
	}
	/*
	Must be entered in counter-clockwise.
	*/
	void geometry_procedural::add_triangle(unsigned int i0, unsigned int i1, unsigned int i2) {
		glm::ivec3 indices = glm::ivec3(i0, i1, i2);
		face* f = new face;
		f->indices = indices;
		add_face(*f);
	}
	/*
	Must be entered in counter-clockwise.
	*/
	void geometry_procedural::add_quad(unsigned int i0, unsigned int i1, unsigned int i2, unsigned int i3) {
		add_triangle(i0, i1, i2);
		add_triangle(i2, i3, i0);
	}
	/*
	Fan triangulation.
	Can only be a convex polygon or weird meshes will result.
	*/
	void geometry_procedural::add_polygon(unsigned int is[GEOLIB_MAX_POLYGON_SIDES], size_t n) {
		if (n > GEOLIB_MAX_POLYGON_SIDES) return;
		unsigned int base = is[0];
		unsigned int n_triangles = n - 2;
		for (int i = 1; i <= n_triangles; i++) {
			add_triangle(base, is[i], is[i + 1]);
		}
	}

	/*	=========================================
	 *	geometry_adapter implementation
		=========================================*/

	geometry_adapter::geometry_adapter(geometry* adaptee) {
		_geo_data = adaptee;
	}
	std::vector<GLfloat> geometry_adapter::request_vertices(unsigned int normalConfig) {
		std::vector<GLfloat> vertex_data;
		_geo_data.
		if (normalConfig == GEOLIB_FLATNORMALS)
			for (vertex* ) {
				vertex_data.push_back(current->position.x);
				vertex_data.push_back(current->position.y);
				vertex_data.push_back(current->position.z);
				vertex_data.push_back(current->flatNormal.x);
				vertex_data.push_back(current->flatNormal.y);
				vertex_data.push_back(current->flatNormal.z);
				vertex_data.push_back(current->texture.x);
				vertex_data.push_back(current->texture.y);
			}
		else if (normalConfig == GEOLIB_SMOOTHNORMALS)
			for (vertex* current = _geo_data->get_vertex_head(); current; current = current->next) {
				vertex_data.push_back(current->position.x);
				vertex_data.push_back(current->position.y);
				vertex_data.push_back(current->position.z);
				vertex_data.push_back(current->smoothNormal.x);
				vertex_data.push_back(current->smoothNormal.y);
				vertex_data.push_back(current->smoothNormal.z);
				vertex_data.push_back(current->texture.x);
				vertex_data.push_back(current->texture.y);
			}
		else
			return;

		return vertex_data;
	}
	std::vector<GLuint> geometry_adapter::request_faces() {
		std::vector<GLuint> idx_data;
		for (face* current = _geo_data->get_face_head(); current; current = current->next) {
			idx_data.push_back(current->indices.x);
			idx_data.push_back(current->indices.y);
			idx_data.push_back(current->indices.z);
		}
	}
}