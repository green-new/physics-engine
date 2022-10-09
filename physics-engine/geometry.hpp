#pragma once
#include <string>
#include <map>
#include <sstream>
#include <iostream>
#include <fstream>
#include <vector>
#include <glad/glad.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

namespace geolib {
	struct vertex {
		glm::vec3 position;
		glm::vec3 normal;
		struct vertex* next;
		struct vertex* prev;
	};
	struct face {
		glm::ivec3 indices;
		glm::vec3 normal;
		struct face* next;
		struct face* prev;
	};
	class geometry {
	public:
		geometry();
		~geometry();

		unsigned int get_face_count();
		unsigned int get_vertex_count();

		void add_vertex(vertex* v);
		void add_face(face* f);

		void remove_vertex(vertex* v);
		void remove_face(face* face);

		void build_face_triangle(vertex v0, vertex v1, vertex v2);
		void build_face_quad(vertex v0, vertex v1, vertex v2, vertex v3);
		void build_face_polygon(vertex* vxs, unsigned int vs);

		void update_face_normals();
		void update_vertex_normals();

		std::vector<GLfloat> get_vertex_data();
		std::vector<GLuint> get_face_data();

		bool is_empty() const;

	private:
		vertex* vertex_data;
		face* face_data;

		unsigned int n_vertices;
		unsigned int n_faces;
	};

	void get_geometry_from_obj(std::string filename, geometry& g);
}
