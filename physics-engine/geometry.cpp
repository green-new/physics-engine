#include "geometry.hpp"

namespace geolib {

	/*	=========================================
	 *	geometry implementation
		=========================================*/
	geometry::geometry() { 
		vertices = std::vector<dynamic_vertex>();
		faces = std::vector<dynamic_face>();
	}
	geometry::~geometry() { 
		clear();
	}
	void geometry::add_vertex(vertex v) {
		vertices.push_back(dynamic_vertex(new vertex(v)));
	}
	void geometry::add_face(face f) {
		faces.push_back(dynamic_face(new face(f)));
	}
	void geometry::remove_vertex(unsigned int idx) {
		vertices.erase(vertices.begin() + idx);
	}
	void geometry::remove_face(unsigned int idx) {
		faces.erase(faces.begin() + idx);
	}
	const std::vector<dynamic_vertex>& geometry::get_vdata() {
		return vertices;
	}
	const std::vector<dynamic_face>& geometry::get_fdata() {
		return faces;
	}
	vertex& geometry::get_vertex(unsigned int idx) {
		return *vertices.at(idx).get();
	}
	face& geometry::get_face(unsigned int idx) {
		return *faces.at(idx).get();
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
		return (unsigned int)faces.size();
	}
	/*
	Gets the vertex count of this geometry.
	@return unsigned int Number of vertices.
	*/
	unsigned int geometry::get_vertex_count() {
		return (unsigned int)vertices.size();
	}
	/*
	Calculates and updates both the flat and smooth shading of the vertex normals.
	If they are already precomputed (e.g., from file) this function is exited.
	Algorithm from https://computergraphics.stackexchange.com/questions/4031/programmatically-generating-vertex-normals
	*/
	void geometry::calc_normals() {
		// Set all normals to zero if swapping to smooth shading
		for (auto& v : vertices) {
			vertex& vcpy = *v.get();
			vcpy.smoothNormal = glm::vec3(0.0f);
		}
		// Smooth normal
		for (auto& f : faces) {
			const face& fcpy = *f.get();
			vertex& A = get_vertex(fcpy.indices[0]);
			vertex& B = get_vertex(fcpy.indices[1]);
			vertex& C = get_vertex(fcpy.indices[2]);
			glm::vec3 Ap = A.position;
			glm::vec3 Bp = B.position;
			glm::vec3 Cp = C.position;
			glm::vec3 normal = glm::cross(Cp - Ap, Bp - Ap);
			normal = glm::normalize(normal);
			A.smoothNormal += normal;
			B.smoothNormal += normal;
			C.smoothNormal += normal;
		}
		for (auto& v : vertices) {
			vertex& vcpy = *v.get();
			vcpy.smoothNormal = glm::normalize(vcpy.smoothNormal);
		}
		// Flat normals
		for (auto& f : faces) {
			face& fcpy = *f.get();
			fcpy.flatNormal = glm::vec3(0.0f);
		}
		for (auto& f : faces) {
			face& fcpy = *f.get();
			vertex& A = get_vertex(fcpy.indices[0]);
			vertex& B = get_vertex(fcpy.indices[1]);
			vertex& C = get_vertex(fcpy.indices[2]);
			glm::vec3 Ap = A.position;
			glm::vec3 Bp = B.position;
			glm::vec3 Cp = C.position;
			glm::vec3 normal = glm::cross(Cp - Ap, Bp - Ap);
			normal = glm::normalize(normal);
			fcpy.flatNormal = normal;
		}
	}
	/*
	Calculates the U, V (S, T) coordinates of the given geometry.
	*/
	void geometry::calc_textures() {
		for (auto& f : faces) {
			const face& fcpy = *f.get();
			vertex& A = get_vertex(fcpy.indices[0]);
			vertex& B = get_vertex(fcpy.indices[1]);
			vertex& C = get_vertex(fcpy.indices[2]);
			glm::vec2 F;


		}

	}
	void geometry::clear() {
		vertices.clear();
		faces.clear();
	}

	/*	=========================================
	*	vertex_strategy_objreader implementation
		=========================================*/

	template<int L, class S>
	vec_strategy<L, S>::vec_strategy() {
		static_assert(std::is_same_v<S, float> ||
			std::is_same_v<S, int> ||
			std::is_same_v<S, unsigned int> ||
			std::is_same_v<S, short>,
			"Must be of type float, int, unsigned int, or short.");
		_data = { 0 };
	}

	template<int L, class S>
	glm::vec<L, S> vec_strategy<L, S>::execute(std::stringstream& stream) {
		std::string element;
		glm::vec<L, S> vector;
		if (_data.max_size() != L) throw std::out_of_range("Error converting OBJ file: invalid range for vec_strategy");
		try {
			for (unsigned int i = 0; i < L; i++) {
				if constexpr (std::is_same_v<S, float>) {
					stream >> element;
					_data.at(i) = stof(element);
				}
				else if constexpr (std::is_same_v<S, unsigned int>) {
					stream >> element;
					_data.at(i) = (unsigned int)stoi(element);
				}
				vector[i] = _data.at(i);
			}
		}
		catch (std::invalid_argument) {
			throw std::invalid_argument("Error converting OBJ file: invalid argument, datatype, element, or value for [" + element + "].\n");
		}
		catch (std::out_of_range) {
			throw std::out_of_range("Error converting OBJ file: out of range number values for [" + element + "].\n");
		}
		return vector;
	}
	geometry geometry_creator::build() {
		if (!normalsWerePredefined) _g.calc_normals();
		if (!texturesWerePredefined) _g.calc_textures();
		return _g;
	}
	/*
	Instantiates a geometry object from given .obj file.
	https://people.cs.clemson.edu/~dhouse/courses/405/docs/brief-obj-file-format.html
	*/
	geometry_obj::geometry_obj(std::string filename) {
		using namespace std;
		file = fstream(filename);
		vIndex = 0;
		vnIndex = 0;
		vtIndex = 0;
		fIndex = 0;
		normalsWerePredefined = false;
		texturesWerePredefined = false;

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
					vertex v;
					v.position = position;
					_g.add_vertex(v);
					vIndex++;
					continue;
				}
				case VERTEX_TEXTURES: {
					glm::vec2 texture = vec2f_strategy.execute(stream);
					if (vtIndex <= vIndex) {
						_g.get_vertex(vtIndex).texture = texture;
						texturesWerePredefined = true;
						vtIndex++;
					}
					continue;
				}
				case VERTEX_NORMALS: {
					glm::vec3 normal = vec3f_strategy.execute(stream);
					if (vnIndex <= vIndex) {
						_g.get_vertex(vnIndex).smoothNormal = normal;
						normalsWerePredefined = true;
						vnIndex++;
					}
					continue;
				}
				case FACE: {
					glm::uvec3 indices = vec3i_strategy.execute(stream);
					indices.x -= 1;
					indices.y -= 1;
					indices.z -= 1;
					if (indices.x >= _g.get_vertex_count()) {
						indices.x = 0;
					}
					else if (indices.y >= _g.get_vertex_count()) {
						indices.y = 0;
					} 
					else if (indices.z >= _g.get_vertex_count()) {
						indices.z = 0;
					}

					face f {};
					f.indices = indices;
					_g.add_face(f);
					fIndex++;
					continue;
				}
				default:
					continue;
				}
			}
		}
	}
	geometry_obj::~geometry_obj() {
		if (file)
			file.close();
		if (stream)
			stream.clear();
	}

	/*	=========================================
	 *	geometry_procedural implementation
		=========================================*/

	geometry_procedural::geometry_procedural() { }
	unsigned int geometry_procedural::add_vertex(float v0, float v1, float v2) {
		glm::vec3 position = glm::vec3(v0, v1, v2);
		vertex v;
		v.position = position;
		_g.add_vertex(v);

		return _g.get_vertex_count() - 1;
	}
	void geometry_procedural::set_smooth_normal(unsigned int idx, float v0, float v1, float v2) {
		normalsWerePredefined = true;
		glm::vec3 smoothNormal = glm::normalize(glm::vec3(v0, v1, v2));
		_g.get_vertex(idx).smoothNormal = smoothNormal;
	}
	void geometry_procedural::set_flat_normal(unsigned int idx, float v0, float v1, float v2) {
		normalsWerePredefined = true;
		glm::vec3 flatNormal = glm::normalize(glm::vec3(v0, v1, v2));
		_g.get_face(idx).flatNormal = flatNormal;
	}
	void geometry_procedural::set_normal(unsigned int idx, float v0, float v1, float v2) {
		set_smooth_normal(idx, v0, v1, v2);
		set_flat_normal(idx, v0, v1, v2);
	}
	/*
	Must be entered in counter-clockwise.
	*/
	void geometry_procedural::add_triangle(unsigned int i0, unsigned int i1, unsigned int i2) {
		glm::ivec3 indices = glm::ivec3(i0, i1, i2);
		face f;
		f.indices = indices;
		_g.add_face(f);
	}
	/*
	Must be entered in counter-clockwise.
	*/
	void geometry_procedural::add_quad(unsigned int i0, unsigned int i1, unsigned int i2, unsigned int i3) {
		add_triangle(i0, i1, i2);
		add_triangle(i2, i0, i3);
	}
	/*
	Fan triangulation.
	Can only be a convex polygon or weird meshes will result.
	*/
	void geometry_procedural::add_polygon(unsigned int is[GEOLIB_MAX_POLYGON_SIDES], unsigned int n) {
		if (n > GEOLIB_MAX_POLYGON_SIDES) return;
		if (n < 3) return;
		unsigned int base = is[0];
		unsigned int n_triangles = n - 2U;
		for (unsigned int i = 1; i <= n_triangles; i++) {
			add_triangle(base, is[i], is[i + 1]);
		}
	}

	/*	=========================================
	 *	geometry_adapter implementation
		=========================================*/

	geometry_adapter::geometry_adapter(geometry* adaptee) {
		_geo_data = adaptee;
	}
	std::vector<GLfloat> geometry_adapter::request_vertices() {
		std::vector<GLfloat> vertex_data;
		for (const auto& current : _geo_data->get_vdata()) {
			vertex& cpy = *current.get();
			vertex_data.push_back(cpy.position.x);
			vertex_data.push_back(cpy.position.y);
			vertex_data.push_back(cpy.position.z);
			vertex_data.push_back(cpy.smoothNormal.x);
			vertex_data.push_back(cpy.smoothNormal.y);
			vertex_data.push_back(cpy.smoothNormal.z);
			vertex_data.push_back(cpy.texture.x);
			vertex_data.push_back(cpy.texture.y);
		}

		return vertex_data;
	}
	std::vector<GLuint> geometry_adapter::request_faces() {
		std::vector<GLuint> idx_data;
		for (const auto& current : _geo_data->get_fdata()) {
			face& cpy = *current.get();
			idx_data.push_back(cpy.indices.x);
			idx_data.push_back(cpy.indices.y);
			idx_data.push_back(cpy.indices.z);
		}
		return idx_data;
	}
}