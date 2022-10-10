#include "geometry.hpp"

namespace geolib {
	geometry::geometry() : vertex_data(nullptr), face_data(nullptr), n_vertices(0), n_faces(0) { 
		
	}
	geometry::geometry(std::string filename) {
		get_geometry_from_obj(filename);
	}
	geometry::~geometry() { }

	void geometry::add_vertex(vertex* v) {
		if (v) {
			if (!vertex_data) {
				v->prev = nullptr;
				v->next = nullptr;
				vertex_data = v;
				n_vertices++;
				return;
			}
			vertex* tmp = vertex_data;
			while (tmp->next)
				tmp = tmp->next;
			tmp->next = v;
			v->prev = tmp;
			v->next = nullptr;
			n_vertices++;
		}
	}
	void geometry::add_face(face* f) {
		if (f) {
			if (!face_data) {
				f->prev = nullptr;
				f->next = nullptr;
				face_data = f;
				n_faces++;
				return;
			}
			face* tmp = face_data;
			while (tmp->next)
				tmp = tmp->next;
			tmp->next = f;
			f->prev = tmp;
			f->next = nullptr;
			n_faces++;
		}
	}
	void geometry::remove_vertex(vertex* v) {
		if (v) {
			// Remove process
			if (vertex_data == v) {
				// If at the head and not null
				// Head = head.next
				// head.prev = nullptr
				vertex_data = vertex_data->next;
				if (v->prev) {
					v->prev->next = v->next;
				}
			}
			else {
				if (v->prev) {
					v->prev->next = v->next;
				}
				if (v->next) { // Not null check 
					v->next->prev = v->prev;
				}
			}
			n_vertices--;
			delete v;
		}
	}
	void geometry::remove_face(face* f) {
		if (f) {
			if (face_data == f) {
				face_data = face_data->next;
				if (face_data->prev) {
					f->prev->next = f->next;
				}
			}
			else {
				if (f->prev) {
					f->prev->next = f->next;
				}
				if (f->next) {
					f->next->prev = f->prev;
				}
			}
			n_faces--;
			delete f;
		}
	}
	bool geometry::is_empty() const {
		return !vertex_data && !face_data;
	}
	unsigned int geometry::get_face_count() {
		return n_faces;
	}
	unsigned int geometry::get_vertex_count() {
		return n_vertices;
	}

	/*
	* https://people.cs.clemson.edu/~dhouse/courses/405/docs/brief-obj-file-format.html
	*/
	void geometry::get_geometry_from_obj(std::string filename) {
		using namespace std;
		enum Entry {
			VERTEX,
			VERTEX_TEXTURES,
			VERTEX_NORMALS,
			FACE
		};
		static const std::map<std::string, Entry> map = {
			{"v", VERTEX},
			{"vt", VERTEX_TEXTURES},
			{"vn", VERTEX_NORMALS},
			{"f", FACE}
		};
		ifstream f(filename);
		unsigned int vIndex = 0;
		unsigned int vtIndex = 0;
		unsigned int vnIndex = 0;
		unsigned int fIndex = 0;

		vector<vertex*> tmpVertices;

		/* If not .obj or not opened, get out! */
		if (filename.substr(filename.find_last_of(".") + 1) != "obj") throw std::logic_error("Error reading OBJ file: file was not .OBJ file type");
		if (!f.is_open()) throw std::runtime_error("Error reading OBJ file: file could not be opened");

		string line = "";
		while (getline(f, line)) {
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
					// Expecting a list of vertices
					float x0 = 0.0f, y0 = 0.0f, z0 = 0.0f;
					try {
						stream >> element;
						x0 = stof(element);
						stream >> element;
						y0 = stof(element);
						stream >> element;
						z0 = stof(element);
					}
					catch (std::invalid_argument) {
						throw std::invalid_argument("Error converting OBJ file: invalid argument, datatype, element, or value for [" + element + "].\n");
					}
					catch (std::out_of_range) {
						throw std::out_of_range("Error converting OBJ file: out of range number values for [" + element + "].\n");
					}
					vertex* v = new vertex;
					if (v) { /* nullptr check */
						v->position = glm::vec3(x0, y0, z0);
						v->normal = glm::vec3(0.0f, 0.0f, 0.0f);
						this->add_vertex(v);
						tmpVertices.push_back(v);
						vIndex++;
					}
					continue;
				}
				case VERTEX_TEXTURES: {
					// Expecting a list of vertex textures
					// Can ignore the 3rd element, it is almost always 0
					float x0 = 0.0f, y0 = 0.0f;
					this->vtPreComputed = true;
					try {
						stream >> element;
						x0 = stof(element);
						stream >> element;
						y0 = stof(element);
						stream >> element;
					}
					catch (std::invalid_argument) {
						throw std::invalid_argument("Error converting OBJ file : invalid argument, datatype, element, or value for [" + element + "].\n");
					}
					catch (std::out_of_range) {
						throw std::out_of_range("Error converting OBJ file: out of range number values for [" + element + "].\n");
					}
					glm::vec2 texture = glm::vec2(x0, y0);
					if (vtIndex <= vIndex && tmpVertices[vtIndex]) {
						tmpVertices[vtIndex]->texture = texture;
						vtIndex++;
					}
					continue;
				}
				case VERTEX_NORMALS: {
					// Expecting a list of vertex normals
					float x0 = 0.0f, y0 = 0.0f, z0 = 0.0f;
					this->vnPreComputed = true;
					try {
						stream >> element;
						x0 = stof(element);
						stream >> element;
						y0 = stof(element);
						stream >> element;
						z0 = stof(element);
					}
					catch (std::invalid_argument) {
						throw std::invalid_argument("Error converting OBJ file : invalid argument, datatype, element, or value for [" + element + "].\n");
					}
					catch (std::out_of_range) {
						throw std::out_of_range("Error converting OBJ file: out of range number values for [" + element + "].\n");
					}
					glm::vec3 normal = glm::vec3(x0, y0, z0);
					if (vnIndex <= vIndex && tmpVertices[vnIndex]) {
						tmpVertices[vnIndex]->normal = normal;
						vnIndex++;
					}
					continue;
				}
				case FACE: {
					unsigned int i[3] = {0};
					try {
						stream >> element;
						i[0] = stoi(element);
						stream >> element;
						i[1] = stoi(element);
						stream >> element;
						i[2] = stoi(element);
					}
					catch (std::invalid_argument) {
						throw std::invalid_argument("Error converting OBJ file: invalid argument, datatype, element, or value for [" + element + "].\n");
					}
					catch (std::out_of_range) {
						throw std::out_of_range("Error converting OBJ file: out of range number values for [" + element + "].\n");
					}
					face* f = new face;
					if (f) {
						f->indices = glm::ivec3(i[0], i[1], i[2]);
						fIndex++;
						this->add_face(f);
					}
					continue;
				}
				default:
					continue;
				}
			}
		}
	}
}