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

#include "mesh.hpp"

/* Triangulation is a relatively expensive operation.
For procedurally generated geometry, a face can only have 8 sides. */
#define GEOLIB_MAX_POLYGON_SIDES 8

namespace geolib {

	struct vertex {
		glm::vec3 position;
		glm::vec2 texture;
		glm::vec3 normal;
		struct vertex* next;
		struct vertex* prev;
	};

	struct face {
		glm::ivec3 indices;
		struct face* next;
		struct face* prev;
	};

	class geometry {
	public:
		geometry();
		geometry(std::string filename);
		~geometry();

		unsigned int get_face_count();
		unsigned int get_vertex_count();

		void add_vertex(vertex* v);
		void add_face(face* f);

		void remove_vertex(vertex* v);
		void remove_face(face* face);

		bool is_empty() const;

		void get_geometry_from_obj(std::string filename);

	private:
		vertex* vertex_data;
		face* face_data;

		unsigned int n_vertices;
		unsigned int n_faces;
	};

	template<typename Vtype = float, typename Ftype = unsigned int>
	class geometry_creator {
	private:
		std::vector<vertex*> tmpVertices;
		std::vector<face*> tmpFaces;

		void clear();
		void calculate_normals();
	public:
		virtual geometry build_geometry();
	};

	template<typename Vtype = float, typename Ftype = unsigned int>
	class geometry_obj : geometry_creator<> {
	private:
		std::string filename;
		
	public:
		geometry_obj(std::string filename);
		virtual geometry build_geometry();
	};

	template<typename Vtype = float, typename Ftype = unsigned int>
	class geometry_procedural : geometry_creator<> {
	public:
		geometry_procedural();
		void add_vertex(Vtype v0, Vtype v1, Vtype v2);
		void add_triangle(Ftype i0, Ftype i1, Ftype i2);
		void add_quad(Ftype i0, Ftype i1, Ftype i2, Ftype i3);
		void add_polygon(Ftype is[GEOLIB_MAX_POLYGON_SIDES], size_t n);
		virtual geometry build_geometry();
	};

	/* Adapter class for retrieving GL appropriate data.
	* Use as little VBOs as possible because our data is mostly static.
	* Sequential vbo format.
	*/
	class geometry_adapter : mesh {
		friend class geometry;
	public:
		geometry_adapter();
		std::vector<GLfloat> get_vertex_data();
		std::vector<GLuint> get_face_data();
	private:
		geometry* _geo_data;
	};
}
