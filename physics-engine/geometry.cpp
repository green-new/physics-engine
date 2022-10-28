#include "geometry.hpp"

namespace geolib {

	Geometry3D::Geometry3D() {
		vertices = std::vector<dynamic_vertex>();
		faces = std::vector<dynamic_face>();
	}
	Geometry3D::~Geometry3D() {
		clear();
	}
	void Geometry3D::add_vertex(vertex v) {
		vertices.push_back(dynamic_vertex(new vertex(v)));
	}
	void Geometry3D::add_face(face f) {
		faces.push_back(dynamic_face(new face(f)));
	}
	void Geometry3D::remove_vertex(unsigned int idx) {
		vertices.erase(vertices.begin() + idx);
	}
	void Geometry3D::remove_face(unsigned int idx) {
		faces.erase(faces.begin() + idx);
	}
	const std::vector<dynamic_vertex>& Geometry3D::get_vdata() const {
		return vertices;
	}
	const std::vector<dynamic_face>& Geometry3D::get_fdata() const {
		return faces;
	}
	vertex& Geometry3D::get_vertex(unsigned int idx) const {
		return *vertices.at(idx).get();
	}
	face& Geometry3D::get_face(unsigned int idx) const {
		return *faces.at(idx).get();
	}
	/*
	Determines if this geometry is empty (no vertices and no faces).
	this is a const reference.
	*/
	bool Geometry3D::is_empty() const {
		return vertices.empty() && faces.empty();
	}
	/*
	Gets the face count of this geometry.
	@return unsigned int Number of faces.
	*/
	unsigned int Geometry3D::get_face_count() {
		return (unsigned int)faces.size();
	}
	/*
	Gets the vertex count of this geometry.
	@return unsigned int Number of vertices.
	*/
	unsigned int Geometry3D::get_vertex_count() {
		return (unsigned int)vertices.size();
	}
	/*
	Calculates and updates both the flat and smooth shading of the vertex normals.
	If they are already precomputed (e.g., from file) this function is exited.
	Algorithm from https://computergraphics.stackexchange.com/questions/4031/programmatically-generating-vertex-normals
	*/
	void Geometry3D::calc_normals() {
		// Set all normals to zero if swapping to smooth shading
		for (auto& v : vertices) {
			vertex& vcpy = *v.get();
			vcpy.normal = glm::vec3(0.0f);
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
			glm::vec3 normal = glm::cross(Bp - Ap, Cp - Ap);
			normal = glm::normalize(normal);
			A.normal += normal;
			B.normal += normal;
			C.normal += normal;
		}
		for (auto& v : vertices) {
			vertex& vcpy = *v.get();
			vcpy.normal = glm::normalize(vcpy.normal);
		}
		for (auto& f : faces) {
			face& fcpy = *f.get();
			vertex& A = get_vertex(fcpy.indices[0]);
			vertex& B = get_vertex(fcpy.indices[1]);
			vertex& C = get_vertex(fcpy.indices[2]);
			glm::vec3 Ap = A.position;
			glm::vec3 Bp = B.position;
			glm::vec3 Cp = C.position;
			glm::vec3 normal = glm::cross(Bp - Ap, Cp - Ap);
			normal = glm::normalize(normal);
			fcpy.flatNormal = normal;
		}
	}
	/*
	Calculates the U, V (S, T) coordinates of the given geometry.
	Currently not used, but will look into in the future.
	Specifically, it may be of use to look into triplanar mapping instead,
	so we don't need to generate UV coordinates, and can simply infer them from 
	local vertex coordinates and normals. This would be done in the shader program. 
	*/
	void Geometry3D::calc_textures() {
		for (auto& f : faces) {
			const face& fcpy = *f.get();
			vertex& A = get_vertex(fcpy.indices[0]);
			vertex& B = get_vertex(fcpy.indices[1]);
			vertex& C = get_vertex(fcpy.indices[2]);
			glm::vec2 F;


		}

	}
	void Geometry3D::clear() {
		vertices.clear();
		faces.clear();
	}

	template<int L, class S>
	vec_strategy<L, S>::vec_strategy() {
		/* Will not compile properly if at any point this class is instantiated with any type
		other than int, unsigned int, or short. */
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
				/* We must use the stof or stoi functions to get numerical information from the string.
				Therefore, we must determine the type of S to use the respective string-to-type function. */
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
	/* Builds the current geometry context for this geometry creator.
	Simply calculates normals and textures if they haven't been defined yet. */
	Geometry3D GeometryBuilder::build() {
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
						_g.get_vertex(vnIndex).normal = normal;
						normalsWerePredefined = true;
						vnIndex++;
					}
					continue;
				}
				case FACE: {
					glm::uvec3 indices = vec3i_strategy.execute(stream);
					/* .obj files have non-zero indices, which doesn't bode well for computer programs.
					We have to subtract one so we get a range of [0 .. n - 1] indices, which will work for our program.*/
					indices.x -= 1;
					indices.y -= 1;
					indices.z -= 1;
					/* If we get some number that doesn't account for any vertex, we should just set it to zero. 
					This functionality will not work if faces are detailed first, but this normally does not happen. */
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
	/* RAII. Close what we have created in the constructor.
	This will always be true. */
	geometry_obj::~geometry_obj() {
		if (file)
			file.close();
		if (stream)
			stream.clear();
	}

	geometry_procedural::geometry_procedural() { 
		normalsWerePredefined = false;
		texturesWerePredefined = false;
	}
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
		_g.get_vertex(idx).normal = smoothNormal;
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
		add_triangle(i2, i3, i0);
	}
	/*
	Fan triangulation.
	Can only be a convex polygon or weird meshes will result.
	Could create a is_convex() function, but its not worth the computational effort (O(n), would make this naive triangulation O(2n)).
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

	GLDataAdapter::GLDataAdapter(const Geometry3D& adaptee) : mGeoData(adaptee) {
	}
	/* Gets the GL appropriate data from the current geometry context.
	We have to loop through the faces, not the vertices, because the faces 
	contain the correct order for which the vertices are drawn.
	This is important because we have no EBO (index buffer) implementation due to its complexity with 
	changing vertex size and normal size when swapping from flat and smooth normals. */
	GLData GLDataAdapter::requestData() {
		GLData g;
		for (const auto& current : mGeoData.get_fdata()) {
			face& cpy = *current.get();
			vertex& A = mGeoData.get_vertex(cpy.indices.x);
			vertex& B = mGeoData.get_vertex(cpy.indices.y);
			vertex& C = mGeoData.get_vertex(cpy.indices.z);
			g.smooth_vertices.push_back(A.position.x);
			g.smooth_vertices.push_back(A.position.y);
			g.smooth_vertices.push_back(A.position.z);
			g.smooth_vertices.push_back(B.position.x);
			g.smooth_vertices.push_back(B.position.y);
			g.smooth_vertices.push_back(B.position.z);
			g.smooth_vertices.push_back(C.position.x);
			g.smooth_vertices.push_back(C.position.y);
			g.smooth_vertices.push_back(C.position.z);
			g.smooth_normals.push_back(A.normal.x); 
			g.smooth_normals.push_back(A.normal.y);
			g.smooth_normals.push_back(A.normal.z);
			g.smooth_normals.push_back(B.normal.x);
			g.smooth_normals.push_back(B.normal.y);
			g.smooth_normals.push_back(B.normal.z);
			g.smooth_normals.push_back(C.normal.x);
			g.smooth_normals.push_back(C.normal.y);
			g.smooth_normals.push_back(C.normal.z);
		}
		for (const auto& current : mGeoData.get_fdata()) {
			face& cpy = *current.get();
			vertex& A = mGeoData.get_vertex(cpy.indices.x);
			vertex& B = mGeoData.get_vertex(cpy.indices.y);
			vertex& C = mGeoData.get_vertex(cpy.indices.z);
			g.flat_normals.push_back(cpy.flatNormal.x);
			g.flat_normals.push_back(cpy.flatNormal.y);
			g.flat_normals.push_back(cpy.flatNormal.z);
			g.flat_normals.push_back(cpy.flatNormal.x);
			g.flat_normals.push_back(cpy.flatNormal.y);
			g.flat_normals.push_back(cpy.flatNormal.z);
			g.flat_normals.push_back(cpy.flatNormal.x);
			g.flat_normals.push_back(cpy.flatNormal.y);
			g.flat_normals.push_back(cpy.flatNormal.z);
			g.flat_vertices.push_back(A.position.x);
			g.flat_vertices.push_back(A.position.y);
			g.flat_vertices.push_back(A.position.z);
			g.flat_vertices.push_back(B.position.x);
			g.flat_vertices.push_back(B.position.y);
			g.flat_vertices.push_back(B.position.z);
			g.flat_vertices.push_back(C.position.x);
			g.flat_vertices.push_back(C.position.y);
			g.flat_vertices.push_back(C.position.z);
		}

		return g;
	}
}