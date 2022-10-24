#include "geometry.hpp"
#include "texture.hpp"
#include "shader.hpp"
#include "Reputeless/PerlinNoise.hpp"
#include "mesh.hpp"
#include <unordered_map>
#include <memory>
#include <glad/glad.h>

// Memory management namespace.
namespace registry {
	/* Template for a registrar.
	Has its map of resources of shared_ptrs of type T, and a way to get and add specific resources. 
	We use shared_ptrs again like in the geometry class because it is not safe to delete dynamically allocated information
	in a list when that dynamically allocated information may or may not have been allocated in the constructor (it doesnt, it does in void add().
	Therefore, we need to follow RAII principles by encapsulating their creation in a shared_ptr object. 
	https://stackoverflow.com/questions/6072192/deleting-dynamically-allocated-memory-from-a-map*/
	template<class T>
	class Resource {
	public:
		T& get(const std::string& uri) const {
			if (_map.contains(uri))
				return *_map.at(uri).get();
			else
				throw std::invalid_argument("Registry: invalid argument for key " + uri + " in registry map");
		}
		const std::unordered_map<std::string, std::shared_ptr<T>>& get_all() {
			return _map;
		}
		virtual void add(const std::string uri, T object) {
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

	class GeometryResources : public Resource<geolib::Geometry3D> {
	public:
		GeometryResources() {
			// OBJ file loads.
			std::cout << "[Registry] Generating geometries..." << std::endl;
			add("tetrahedron", geolib::geometry_obj("models/tetrahedron.obj").build());
			add("octahedron", geolib::geometry_obj("models/octahedron.obj").build());
			add("dodecahedron", geolib::geometry_obj("models/dodecahedron.obj").build());
			add("icosahedron", geolib::geometry_obj("models/icosahedron.obj").build());
			add("gourd", geolib::geometry_obj("models/gourd.obj").build());
			add("teapot", geolib::geometry_obj("models/teapot.obj").build());

			// Procedural generation.
			geolib::geometry_procedural cubeGen;
			unsigned int c0 = cubeGen.add_vertex(-0.5f, -0.5f, -0.5f);
			unsigned int c1 = cubeGen.add_vertex(0.5f, -0.5f, -0.5f);
			unsigned int c2 = cubeGen.add_vertex(-0.5f, 0.5f, -0.5f);
			unsigned int c3 = cubeGen.add_vertex(0.5f, 0.5f, -0.5f);
			unsigned int c4 = cubeGen.add_vertex(-0.5f, -0.5f, 0.5f);
			unsigned int c5 = cubeGen.add_vertex(-0.5f, 0.5f, 0.5f);
			unsigned int c6 = cubeGen.add_vertex(0.5f, 0.5f, 0.5f);
			unsigned int c7 = cubeGen.add_vertex(0.5f, -0.5f, 0.5f);
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
			unsigned int Us = 64;
			unsigned int Vs = 64;
			float pi = std::numbers::pi_v<float>;
			float x0, y0, z0;
			for (unsigned int u = 0; u < Us; u++) {
				float theta0 = pi * u / Us;
				float theta1 = pi * (u + 1.0f) / Us;
				for (unsigned int v = 0; v < Vs; v++) {
					float phi0 = 2 * pi * v / Vs;
					float phi1 = 2 * pi * (v + 1.0f) / Vs;
					x0 = sinf(theta0) * cosf(phi0);
					y0 = sinf(theta0) * sinf(phi0);
					z0 = cosf(theta0);
					unsigned int i0 = sphereGen.add_vertex(x0, y0, z0);
					x0 = sinf(theta0) * cosf(phi1);
					y0 = sinf(theta0) * sinf(phi1);
					z0 = cosf(theta0);
					unsigned int i1 = sphereGen.add_vertex(x0, y0, z0);
					x0 = sinf(theta1) * cosf(phi0);
					y0 = sinf(theta1) * sinf(phi0);
					z0 = cosf(theta1);
					unsigned int i2 = sphereGen.add_vertex(x0, y0, z0);
					x0 = sinf(theta1) * cosf(phi1);
					y0 = sinf(theta1) * sinf(phi1);
					z0 = cosf(theta1);
					unsigned int i3 = sphereGen.add_vertex(x0, y0, z0);
					if (u > 0 && u < Us - 1)
						// Add a normal quad.
						sphereGen.add_quad(i0, i2, i3, i1);
					else
						// Add a triangle at the top and bottom ends.
						if (u == 0)
							sphereGen.add_triangle(i0, i2, i3);
						else if (u == Us - 1)
							sphereGen.add_triangle(i0, i2, i1);
				}
			}
			add("uvsphere", sphereGen.build());

			geolib::geometry_procedural plane;

			unsigned int i0 = plane.add_vertex(-1.0f, 0.0f, -1.0f);
			unsigned int i1 = plane.add_vertex(1.0f, 0.0f, -1.0f);
			unsigned int i2 = plane.add_vertex(1.0f, 0.0f, 1.0f);
			unsigned int i3 = plane.add_vertex(-1.0f, 0.0f, 1.0f);
			unsigned int faces[4] = { i0, i1, i2, i3 };
			plane.add_polygon(faces, 4);

			add("plane", plane.build());

			geolib::geometry_procedural terrain;
			uint32_t seed = (uint32_t)time(NULL);
			const siv::PerlinNoise perlin{ seed };
			int width = 256, height = 256;
			int depth = 100;
			uint32_t oct0 = 4;
			const double fx = (1.0f / width);
			const double fy = (1.0f / height);
			for (int x = -width; x < width; x++) {
				for (int y = -height; y < height; y++) {
					double d0 = perlin.octave2D_01(fx * x, fy * y, oct0);
					double d1 = perlin.octave2D_01(fx * x, fy * (y + 1), oct0);
					double d2 = perlin.octave2D_01(fx * (x + 1), fy * y, oct0);
					double d3 = perlin.octave2D_01(fx * (x + 1), fy * (y + 1), oct0);
					unsigned int i0 = terrain.add_vertex(x, d0 * depth, y);
					unsigned int i1 = terrain.add_vertex(x, d1 * depth, y + 1);
					unsigned int i2 = terrain.add_vertex(x + 1, d2 * depth, y);
					unsigned int i3 = terrain.add_vertex(x + 1, d3 * depth, y + 1);

					terrain.add_quad(i0, i1, i3, i2);
				}
			}

			add("terrain", terrain.build());
		}
	};

	class ShaderResources : public Resource<shader> {
	public:
		ShaderResources() {
			std::cout << "[Registry] Generating shaders..." << std::endl;
			shader VS_transform = shader("shaders/VS_transform.glsl", "shaders/FS_transform.glsl");
			add("VS_transform", VS_transform);
			shader skybox = shader("shaders/skybox.vert", "shaders/skybox.frag");
			add("skybox", skybox);
		}
		~ShaderResources() {
			// Shared pointers are released automatically with std::shared_ptr.
			
		}
	};

	class TextureResources : public Resource<texture_t> {
	public:
		TextureResources() {
			std::cout << "[Registry] Generating textures..." << std::endl;
			// Procedural textures.
			float freq = 8.0f;
			uint32_t octs = 5;
			uint16_t size = 256;
			add("cloud", noise_texture(BLUE, WHITE, size, size, freq, octs));
			add("fire", noise_texture(RED, ORANGE, size, size, freq, octs));
			add("lava", noise_texture(YELLOW, RED, size, size, freq, octs));
			add("magenta_white", noise_texture(MAGENTA, WHITE, size, size, freq, octs));
			add("black_white", noise_texture(BLACK, WHITE, size, size, freq, octs));
			add("better_cloud", noise_texture(CYAN, WHITE, size, size, freq, octs));
			add("water", noise_texture(BLUE, build_colorb(0, 0, 128, 255), size, size, freq, octs));
			add("grass", noise_texture(build_colorb(0, 128, 0, 255), build_colorb(0, 64, 0, 255), size, size, freq, octs));
			add("rock", noise_texture(build_colorb(128, 128, 128, 255), build_colorb(64, 64, 64, 255), size, size, freq, octs));
			add("red", solid_colored_texture(RED));
			add("green", solid_colored_texture(GREEN));
			add("blue", solid_colored_texture(BLUE));
			add("yellow", solid_colored_texture(YELLOW));
			add("orange", solid_colored_texture(ORANGE));
			add("xor_red", xor_texture(RED, 256, 256));
			add("xor_green", xor_texture(GREEN, 256, 256));
			add("xor_blue", xor_texture(BLUE, 256, 256));

			// Cubemaps.
			add("skybox", skybox());

			// Texutres from file!
			add("castle_walls_short.png", build_texture("textures/castle_walls_short.png"));
		}
		~TextureResources() {
			// We need to delete the GL textures associated in this map.
			// We need to loop through them all which is O(n) time.
			// OpenGL cleans up after itself after all contexts referring
			// to the texture object is released, however - its still good practice
			// to eliminate things created in the constructor.
			for (const auto& it : _map) {
				texture_t* t = it.second.get();
				glDeleteTextures(0.5f, t);
			}
		}
	};

	// This is the one point of access to all resources for this application.
	// glfw and opengl must be defined before initalizing this class due to texture loading.
	// Perhaps in the future we split this into two, GameRegistry and GLResources.
	// GameRegistry would handle things like geometry data, and maybe sounds for example in the future and run on its own thread.
	// GLResources would handle all GL objects (textures, buffer objects, wayyy more perhaps) and run in a GL thread/context.
	// Also, optimally, this class should be static (it could be singleton, but singleton is way overrused and incorrectly!)
	// However, static isn't thread safe without mutexes.
	class ResourceManager final {
	public:
		void init() {
			Textures = std::make_unique<TextureResources>();
			Geometries = std::make_unique<GeometryResources>();
			Shaders = std::make_unique<ShaderResources>();
		}
		texture_t& getTexture(std::string name) {
			return Textures->get(name);
		}
		geolib::Geometry3D& getGeometry(std::string name) {
			return Geometries->get(name);
		}
		Shader& getShader(std::string name) {
			return Shaders->get(name);
		}
		~ResourceManager() = default;
	private:
		std::unique_ptr<TextureResources> Textures;
		std::unique_ptr<GeometryResources> Geometries;
		std::unique_ptr<ShaderResources> Shaders;
	};
};