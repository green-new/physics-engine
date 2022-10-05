#pragma once
#include <string>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

namespace geolib {

	class vertex {
	public:
		vertex();
		~vertex();

		glm::vec3 position;
		glm::vec3 normal;
		halfedge* leaving;

		friend class geometry;
	};

	class face {
	public:
		face();
		~face();

		void update_normal();
		void update_vertex_normals() const;
		int get_edge_count();

		glm::vec3 normal;
		halfedge* inner;

		friend class geometry;
	};

	class halfedge {
	public:
		halfedge();
		~halfedge();

		vertex* origin;
		face* incident;
		halfedge* twin;
		halfedge* next;

		halfedge* get_prev();

		friend class geometry;
	};

	class geometry {
	public:
		geometry();
		~geometry();

		void create_from_off(std::string filename);
		void create_from_obj(std::string filename);

		void create_face(vertex* v, unsigned int vs);

		void add(vertex* v);
		void add(halfedge* edge);
		void add(face* f);

		void remove(vertex* v);
		void remove(halfedge* edge);
		void remove(face* f);

		void update_normals();
		void update_vertex_normals();

		bool is_empty();
		bool is_manifold();
		void clear();
	private:
		/* Triangulation creates new faces and new edges. */
		void triangulate(face* face);

		vertex* vertexlist;
		halfedge* edgelist;
		face* facelist;

		unsigned int n_vertices;
		unsigned int n_edges;
		unsigned int n_faces;

		/* If this value is bound, it is the boundary of the plane. */
		inline const static face* infinity = nullptr;
	};
}