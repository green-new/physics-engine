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
	typedef std::shared_ptr<vertex> dynamic_vertex;
	struct face {
		glm::ivec3 indices;
		glm::vec3 flatNormal;
	};
	class Simplex3D {
		bool contains_origin();
	private:
		struct simple_vertex {
			float x, y, z;
		};
		std::array<simple_vertex, 4> vertices;
	};
	typedef std::shared_ptr<face> dynamic_face;
	/* The geometry class is responsible for loading 3D data and representing them as triangle meshes,
	keeping a list of vertices and face information. 
	It encapsulates each vertex object into a shared_ptr, so that each vertex is guaranteed to be destroyed once its 
	parent geometry object's destructor is called. 
	This is so we don't have to loop through each vector of vertices and faces and delete them, which could be memory unsafe. 
	https://stackoverflow.com/questions/6072192/deleting-dynamically-allocated-memory-from-a-map */
	class Geometry3D {
	public:
		Geometry3D();
		~Geometry3D();

		unsigned int get_face_count();
		unsigned int get_vertex_count();

		void add_vertex(vertex v);
		void add_face(face f);

		void remove_vertex(unsigned int idx);
		void remove_face(unsigned int idx);

		const std::vector<dynamic_vertex>& get_vdata() const;
		const std::vector<dynamic_face>& get_fdata() const;

		vertex& get_vertex(unsigned int idx) const;
		face& get_face(unsigned int idx) const;

		void clear();
		void calc_normals();
		void calc_textures();

		bool is_empty() const;
	protected:
		std::vector<dynamic_vertex> vertices;
		std::vector<dynamic_face> faces;
	};

	/* Simple implementation of the Builder strategy in C++. 
	Can be inherited and each implementation will build from the child geometry _g object. */
	class GeometryBuilder {
	public:
		/* Return by value.
		_g is not allocated dynamically. */
		Geometry3D build();
	protected:
		bool normalsWerePredefined = false;
		bool texturesWerePredefined = false;
		/* Not allocated dynamically. 
		Will be deleted in destructor automatically. */
		Geometry3D _g;
	};

	/* The objreader_strategy interface. */
	template<class T>
	class objreader_strategy {
	public:
		virtual T execute(std::stringstream& stream) = 0;
	};

	/* The vec strategy interface for use with reading .obj files. 
	It will read from the stream L elements of type S and return it as a glm::vec. */
	template<int L, class S>
	class vec_strategy : objreader_strategy<glm::vec<L, S>>{
	private:
		std::array<S, L> _data;
	public:
		vec_strategy();
		~vec_strategy() = default;

		virtual glm::vec<L, S> execute(std::stringstream& stream);
	};

	class geometry_obj : public GeometryBuilder {
	private:
		std::string filename;
		std::fstream file;
		std::stringstream stream;
		enum entry { VERTEX, VERTEX_TEXTURES, VERTEX_NORMALS, FACE };
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

	/* Generate geometry procedurally.
	This is currently used to create the plane, cube, and sphere meshes. */
	class geometry_procedural : public GeometryBuilder{
	public:
		geometry_procedural();
		void set_smooth_normal(unsigned int idx, float v0, float v1, float v2);
		void set_flat_normal(unsigned int idx, float v0, float v1, float v2);
		void set_normal(unsigned int idx, float v0, float v1, float v2);
		unsigned int add_vertex(float v0, float v1, float v2);
		void add_texture(unsigned int idx, float u, float v);
		void add_triangle(unsigned int i0, unsigned int i1, unsigned int i2);
		void add_quad(unsigned int i0, unsigned int i1, unsigned int i2, unsigned int i3);
		void add_polygon(unsigned int is[GEOLIB_MAX_POLYGON_SIDES], unsigned int n);
	};
	/* For use with the adapter strategy. */
	struct GLData {
		std::vector<GLfloat> smooth_vertices;
		std::vector<GLfloat> flat_vertices;
		std::vector<GLfloat> flat_normals;
		std::vector<GLfloat> smooth_normals;
		std::vector<GLfloat> textureData;
	};
	/* Adapter class for retrieving GL appropriate data. 
	Adapter design pattern. Takes geometry data, which is difficult to read properly from a GL perspective.
	Therefore, we retrieve the data in terms of GL so it can be seamlessly read by glBufferData, etc. 
	That way, we can keep the geometry class specific to 3D geometry data and disregard anything to do with a GL context. 
	Therefore, loading of geometric information could be ran on a different thread without a GL context, but this is probably not necessary. */
	class GLDataAdapter {
	public:
		GLDataAdapter(const Geometry3D& adaptee);
		GLData requestData();
		const Geometry3D& mGeoData;
	};
}
