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
#define GEOLIB_FLATNORMALS			0x01
#define GEOLIB_SMOOTHNORMALS		0x02

namespace geolib {
	struct vertex {
		glm::vec3 position;
		glm::vec2 texture;
		glm::vec3 flatNormal;
		glm::vec3 smoothNormal;
	};
	typedef std::shared_ptr<vertex> dynamic_vertex;
	struct face {
		glm::ivec3 indices;
	};
	typedef std::shared_ptr<face> dynamic_face;
	class geometry {
	public:
		geometry();
		~geometry();

		unsigned int get_face_count();
		unsigned int get_vertex_count();

		void add_vertex(vertex v);
		void add_face(face f);

		void remove_vertex(unsigned int idx);
		void remove_face(unsigned int idx);

		const std::vector<dynamic_vertex>& get_vdata();
		const std::vector<dynamic_face>& get_fdata();

		vertex& get_vertex(unsigned int idx);
		face& get_face(unsigned int idx);

		void clear();
		void calc_normals();

		bool is_empty() const;
	protected:
		std::vector<dynamic_vertex> vertices;
		std::vector<dynamic_face> faces;
	};

	class geometry_creator {
	public:
		/* Return by value.
		_g is not allocated dynamically. */
		geometry build();
	protected:
		bool normalsWerePredefined;
		bool texturesWerePredefined;
		/* Not allocated dynamically. 
		Will be deleted in destructor automatically. */
		geometry _g;
	};

	template<class T>
	class objreader_strategy {
	public:
		virtual T execute(std::stringstream& stream) = 0;
	};

	template<int L, class S>
	class vec_strategy : objreader_strategy<glm::vec<L, S>>{
	private:
		std::array<S, L> _data;
	public:
		vec_strategy();
		~vec_strategy() = default;

		virtual glm::vec<L, S> execute(std::stringstream& stream);
	};

	class geometry_obj : public geometry_creator {
	private:
		std::string filename;
		std::fstream file;
		std::stringstream stream;
		enum entry {VERTEX, VERTEX_TEXTURES, VERTEX_NORMALS, FACE };
		inline static const std::unordered_map<std::string, entry> map = {
			{"v", VERTEX},
			{"vt", VERTEX_TEXTURES},
			{"vn", VERTEX_NORMALS},
			{"f", FACE}
		};
		vec_strategy<3, float> vec3f_strategy;
		vec_strategy<2, float> vec2f_strategy;
		vec_strategy<3, unsigned int> vec3i_strategy;
	protected:
		unsigned int vIndex, vnIndex, vtIndex, fIndex;
	public:
		geometry_obj(std::string filename);
		~geometry_obj();
	};

	class geometry_procedural : public geometry_creator {
	public:
		geometry_procedural();
		unsigned int add_vertex(float v0, float v1, float v2);
		void set_smooth_normal(unsigned int idx, float v0, float v1, float v2);
		void set_flat_normal(unsigned int idx, float v0, float v1, float v2);
		void set_normal(unsigned int idx, float v0, float v1, float v2);
		void add_texture(unsigned int idx, float u, float v);
		void add_triangle(unsigned int i0, unsigned int i1, unsigned int i2);
		void add_quad(unsigned int i0, unsigned int i1, unsigned int i2, unsigned int i3);
		void add_polygon(unsigned int is[GEOLIB_MAX_POLYGON_SIDES], unsigned int n);
	};

	/* Adapter class for retrieving GL appropriate data. */
	class geometry_adapter {
	public:
		geometry_adapter(geometry* adaptee, unsigned int normalConfig);
		std::vector<GLfloat> request_vertices();
		std::vector<GLuint> request_faces();
		geometry* _geo_data; 
		unsigned int _normalConfig;
	};
}
