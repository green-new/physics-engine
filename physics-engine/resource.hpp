#include "geometry.hpp"
#include "texture.hpp"
#include "shader.hpp"
#include <unordered_map>
#include <memory>
#include <glad/glad.h>

// Memory management namespace.
namespace registry {
	template<class T>
	class registrar {
	public:
		T& get(const std::string& uri) const {
			return *_map.at(uri).get();
		}
		void add(const std::string uri, T object) {
			_map.insert( { uri, std::shared_ptr<T>(new T(object)) } );
		}
	protected:
		registrar() {
			_map = std::unordered_map<std::string, std::shared_ptr<T>>();
			_map.clear();
		}
		virtual ~registrar() {
			// Map consists of shared ptr.
			// Elements will be automatically deallocated on program exit.
			_map.clear();
		}
		std::unordered_map<std::string, std::shared_ptr<T>> _map;
	};

	class geometry_registrar : public registrar<geolib::geometry> {
	public:
		geometry_registrar() {
			// OBJ file loads.
			add("tetrahedron", geolib::geometry_obj("models/tetrahedron.obj").build());
			add("octahedron", geolib::geometry_obj("models/octahedron.obj").build());
			add("dodecahedron", geolib::geometry_obj("models/dodecahedron.obj").build());
			add("icosahedron", geolib::geometry_obj("models/icosahedron.obj").build());
			add("gourd", geolib::geometry_obj("models/gourd.obj").build());
			add("pyramid", geolib::geometry_obj("models/pyramid.obj").build());
			add("teapot", geolib::geometry_obj("models/teapot.obj").build());

			// Procedural generation.
			geolib::geometry_procedural cubeGen;
			unsigned int c0 = cubeGen.add_vertex(-1, -1, -1);
			unsigned int c1 = cubeGen.add_vertex(1, -1, -1);
			unsigned int c2 = cubeGen.add_vertex(-1, 1, -1);
			unsigned int c3 = cubeGen.add_vertex(1, 1, -1);
			unsigned int c4 = cubeGen.add_vertex(-1, -1, 1);
			unsigned int c5 = cubeGen.add_vertex(-1, 1, 1);
			unsigned int c6 = cubeGen.add_vertex(1, 1, 1);
			unsigned int c7 = cubeGen.add_vertex(1, -1, 1);
			cubeGen.add_quad(c0, c2, c3, c1);
			cubeGen.add_quad(c1, c0, c4, c7);
			cubeGen.add_quad(c7, c1, c3, c6);
			cubeGen.add_quad(c6, c7, c4, c5);
			cubeGen.add_quad(c5, c6, c3, c2);
			cubeGen.add_quad(c2, c5, c4, c0);

			add("cube", cubeGen.build());

			// UV sphere algorithm
			// x = x0 + r * sin(A) * cos(P)
			// y = y0 + r * sin(A) * sin(P)
			// z = z0 + r * cos(A)
			// where r > 0, A = theta[0, pi] and P = phi[0, 2pi]
			geolib::geometry_procedural sphereGen;
			unsigned int Us = 5;
			unsigned int Vs = 12;
			float pi = std::numbers::pi_v<float>;
			float x0, y0, z0;
			for (unsigned int u = 0; u < Us; u++) {
				float theta0 = pi * u / Us;
				float theta1 = pi * (u + 1) / Us;
				for (unsigned int v = 0; v < Vs; v++) {
					float phi0 = 2 * pi * v / Vs;
					float phi1 = 2 * pi * (v + 1) / Vs;
					x0 = sinf(theta0) * cosf(phi0);
					y0 = sinf(theta0) * sinf(phi0);
					z0 = cosf(theta0);
					unsigned int i0 = sphereGen.add_vertex(x0, y0, z0);
					x0 = sinf(theta1) * cosf(phi1);
					y0 = sinf(theta1) * sinf(phi1);
					z0 = cosf(theta1);
					unsigned int i1 = sphereGen.add_vertex(x0, y0, z0);
					x0 = sinf(theta0) * cosf(phi0);
					y0 = sinf(theta0) * sinf(phi0);
					z0 = cosf(theta1);
					unsigned int i2 = sphereGen.add_vertex(x0, y0, z0);
					sphereGen.add_triangle(i0, i1, i2);
				}
			}
			add("uvsphere", sphereGen.build());
		}
	};

	class shader_registrar : public registrar<shader> {
	public:
		shader_registrar() {
			shader VS_transform = shader("VS_transform.glsl", "FS_transform.glsl");
			add("VS_transform", VS_transform);
		}
		~shader_registrar() {
			// Shared pointers are released automatically with std::shared_ptr.
			
		}
	};

	class texture_registrar : public registrar<texture_t> {
	public:
		texture_registrar() {
			uint32_t seed = 81293754;
			// Procedural textures.
			add("noise_red", noise_texture(RED, 64, 64, 1.0f, 8, seed));
			add("noise_green", noise_texture(GREEN, 64, 64, 1.0f, 8, seed));
			add("noise_blue", noise_texture(BLUE, 64, 64, 1.0f, 8, seed));
			add("red", solid_colored_texture(RED));
			add("green", solid_colored_texture(GREEN));
			add("blue", solid_colored_texture(BLUE));
			add("xor_red", xor_texture(RED, 256, 256));
			add("xor_green", xor_texture(GREEN, 256, 256));
			add("xor_blue", xor_texture(BLUE, 256, 256));

			// Textures from file.
			std::string flat_1 = "flat_1.png";
			add(flat_1, build_texture(flat_1));
		}
		~texture_registrar() {
			// We need to delete the GL textures associated in this map.
			// We need to loop through them all which is O(n) time.
			// OpenGL cleans up after itself after all contexts referring
			// to the texture object is released, however - its still good practice
			// to eliminate things created in the constructor.
			for (const auto& it : _map) {
				texture_t* t = it.second.get();
				glDeleteTextures(1, t);
			}
		}
	};

	// This is the one point of access to all resources for this application.
	// glfw and opengl must be defined before initalizing this class.
	class registry final {
	public:
		texture_t& get_texture(std::string name) {
			return texture_registry.get(name);
		}
		geolib::geometry& get_geometry(std::string name) {
			return geometry_registry.get(name);
		}
		shader& get_shader(std::string name) {
			return shader_registry.get(name);
		}
		registry() { 
			texture_registry = texture_registrar();
			geometry_registry = geometry_registrar();
			shader_registry = shader_registrar();
		}
		~registry() {
			
		}
	private:
		texture_registrar texture_registry;
		geometry_registrar geometry_registry;
		shader_registrar shader_registry;
	};
};