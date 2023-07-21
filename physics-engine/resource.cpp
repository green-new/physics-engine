#include "resource.hpp"
#include "Reputeless/PerlinNoise.hpp"
#include "texture.hpp"

using namespace Resources;

GeometryResources::GeometryResources() {
	// OBJ file loads.
	std::cout << "[Registry] Generating geometries..." << std::endl;
	add("tetrahedron", Geometry::geometry_obj("models/tetrahedron.obj").build());
	add("octahedron", Geometry::geometry_obj("models/octahedron.obj").build());
	add("dodecahedron", Geometry::geometry_obj("models/dodecahedron.obj").build());
	add("icosahedron", Geometry::geometry_obj("models/icosahedron.obj").build());
	add("gourd", Geometry::geometry_obj("models/gourd.obj").build());
	add("teapot", Geometry::geometry_obj("models/teapot.obj").build());

	// Procedural generation.
	Geometry::geometry_procedural cubeGen;
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
	Geometry::geometry_procedural sphereGen;
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

	Geometry::geometry_procedural plane;

	unsigned int i0 = plane.add_vertex(-1.0f, 0.0f, -1.0f);
	unsigned int i1 = plane.add_vertex(1.0f, 0.0f, -1.0f);
	unsigned int i2 = plane.add_vertex(1.0f, 0.0f, 1.0f);
	unsigned int i3 = plane.add_vertex(-1.0f, 0.0f, 1.0f);
	unsigned int faces[4] = { i0, i1, i2, i3 };
	plane.add_polygon(faces, 4);

	add("plane", plane.build());

	Geometry::geometry_procedural terrain;
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
			unsigned int i0 = terrain.add_vertex((float)x, (float)d0 * (float)depth, (float)y);
			unsigned int i1 = terrain.add_vertex((float)x, (float)d1 * (float)depth, (float)y + 1.0f);
			unsigned int i2 = terrain.add_vertex((float)x + 1.0f, (float)d2 * (float)depth, (float)y);
			unsigned int i3 = terrain.add_vertex((float)x + 1.0f, (float)d3 * (float)depth, (float)y + 1.0f);

			terrain.add_quad(i0, i1, i3, i2);
		}
	}

	add("terrain", terrain.build());
}

ShaderResources::ShaderResources() {
	std::cout << "[Registry] Generating shaders..." << std::endl;
	Shader VS_transform = Shader("shaders/VS_transform.glsl", "shaders/FS_transform.glsl");
	add("transform", VS_transform);
	Shader skybox = Shader("shaders/skybox.vert", "shaders/skybox.frag");
	add("skybox", skybox);
}

TextureResources::TextureResources() {
	std::cout << "[Registry] Generating textures..." << std::endl;
	// Procedural textures.
	float freq = 8.0f;
	uint32_t octs = 5;
	uint16_t size = 256;
	add("cloud", noise_texture(BLUE, WHITE, size, size, freq, octs));
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
	add("xor_red", xor_texture(RED, 256, 256));
	add("xor_green", xor_texture(GREEN, 256, 256));
	add("xor_blue", xor_texture(BLUE, 256, 256));

	// Cubemaps.
	add("skybox", skybox());

	// Texutres from file!
	add("castle_walls_short.png", build_texture("textures/castle_walls_short.png"));
}
TextureResources:: ~TextureResources() {
	for (const auto& it : _map) {
		texture_t* t = it.second.get();
		glDeleteTextures(1, t);
	}
}

void ResourceManager::init() {
	Textures = std::make_unique<TextureResources>();
	Geometries = std::make_unique<GeometryResources>();
	Shaders = std::make_unique<ShaderResources>();
}
texture_t& ResourceManager::getTexture(std::string name) {
	return Textures->get(name);
}
Geometry::Geometry3D& ResourceManager::getGeometry(std::string name) {
	return Geometries->get(name);
}
Shader& ResourceManager::getShader(std::string name) {
	return Shaders->get(name);
}