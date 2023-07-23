#include "resource.hpp"
#include "Reputeless/PerlinNoise.hpp"
#include "texture.hpp"

using namespace Resources;

GeometryResources::GeometryResources() {
	// OBJ file loads.
	std::cout << "[Registry] Generating geometries..." << std::endl;
	//add("tetrahedron", Geometry::AssetGeometryBuilder()
	//	.load("models/tetrahedron.obj")
	//	.build());
	//add("octahedron", Geometry::AssetGeometryBuilder()
	//	.load("models/octahedron.obj")
	//	.build());
	//add("dodecahedron", Geometry::AssetGeometryBuilder()
	//	.load("models/dodecahedron.obj")
	//	.build());
	//add("icosahedron", Geometry::AssetGeometryBuilder()
	//	.load("models/icosahedron.obj")
	//	.build());
	//add("gourd", Geometry::AssetGeometryBuilder()
	//	.load("models/gourd.obj")
	//	.build());
	//add("teapot", Geometry::AssetGeometryBuilder()
	//	.load("models/teapot.obj")
	//	.build());

	// Procedural generation.
	Geometry::vertex v0 = { .position = glm::vec3(-0.5f, -0.5f, -0.5f), .normal = glm::vec3(-0.5f, -0.5f, -0.5f), .texture = glm::vec2(0.0f, 0.0f)};
	Geometry::vertex v1 = { .position = glm::vec3(0.5f, -0.5f, -0.5f), .normal = glm::vec3(-0.5f, -0.5f, -0.5f), .texture = glm::vec2(0.0f, 0.0f) };
	Geometry::vertex v2 = { .position = glm::vec3(0.5f, 0.5f, -0.5f), .normal = glm::vec3(-0.5f, -0.5f, -0.5f), .texture = glm::vec2(0.0f, 0.0f) };
	Geometry::vertex v3 = { .position = glm::vec3(-0.5f, -0.5f, 0.5f), .normal = glm::vec3(-0.5f, -0.5f, -0.5f), .texture = glm::vec2(0.0f, 0.0f) };
	Geometry::vertex v4 = { .position = glm::vec3(-0.5f, 0.5f, -0.5f), .normal = glm::vec3(-0.5f, -0.5f, -0.5f), .texture = glm::vec2(0.0f, 0.0f) };
	Geometry::vertex v5 = { .position = glm::vec3(-0.5f, 0.5f, 0.5f), .normal = glm::vec3(-0.5f, -0.5f, -0.5f), .texture = glm::vec2(0.0f, 0.0f) };
	Geometry::vertex v6 = { .position = glm::vec3(0.5f, 0.5f, 0.5f), .normal = glm::vec3(-0.5f, -0.5f, -0.5f), .texture = glm::vec2(0.0f, 0.0f) };
	Geometry::vertex v7 = { .position = glm::vec3(0.5f, -0.5f, 0.5f), .normal = glm::vec3(-0.5f, -0.5f, -0.5f), .texture = glm::vec2(0.0f, 0.0f) };
	auto cube = Geometry::ProceduralBuilder().addQuad({v0, v2, v3, v1})
		.addQuad({ v1, v0, v4, v7 })
		.addQuad({ v7, v1, v3, v6 })
		.addQuad({ v6, v7, v4, v5 })
		.addQuad({ v5, v6, v3, v2 })
		.addQuad({ v2, v5, v4, v0 })
		.build();

	add("cube", *cube.get());

	// UV sphere algorithm
	// x = x0 + r * sin(A) * cos(P)
	// y = y0 + r * sin(A) * sin(P)
	// z = z0 + r * cos(A)
	// where r > 0, A = theta[0, pi] and P = phi[0, 2pi]
	Geometry::ProceduralBuilder sphereGen;
	unsigned int Us = 64;
	unsigned int Vs = 64;
	float pi = std::numbers::pi_v<float>;
	float x0, y0, z0,
		x1, y1, z1,
		x2, y2, z2,
		x3, y3, z3;
	for (unsigned int u = 0; u < Us; u++) {
		float theta0 = pi * u / Us;
		float theta1 = pi * (u + 1.0f) / Us;
		for (unsigned int v = 0; v < Vs; v++) {
			float phi0 = 2 * pi * v / Vs;
			float phi1 = 2 * pi * (v + 1.0f) / Vs;
			// Lower left
			x0 = sinf(theta0) * cosf(phi0);
			y0 = sinf(theta0) * sinf(phi0);
			z0 = cosf(theta0);

			// Lower right
			x1 = sinf(theta0) * cosf(phi1);
			y1 = sinf(theta0) * sinf(phi1);
			z1 = cosf(theta0);

			// Top left
			x2 = sinf(theta1) * cosf(phi0);
			y2 = sinf(theta1) * sinf(phi0);
			z2 = cosf(theta1);

			// Top right
			x3 = sinf(theta1) * cosf(phi1);
			y3 = sinf(theta1) * sinf(phi1);
			z3 = cosf(theta1);
			Geometry::vertex v0 = Geometry::vertex{ .position = glm::vec3(x0, y0, z0), .normal = glm::vec3(x0, y0, z0), .texture = glm::vec2(theta0, phi0) };
			Geometry::vertex v1 = Geometry::vertex{ .position = glm::vec3(x1, y1, z1), .normal = glm::vec3(x1, y1, z1), .texture = glm::vec2(theta0, phi1) };
			Geometry::vertex v2 = Geometry::vertex{ .position = glm::vec3(x2, y2, z2), .normal = glm::vec3(x2, y2, z2), .texture = glm::vec2(theta1, phi0) };
			Geometry::vertex v3 = Geometry::vertex{ .position = glm::vec3(x3, y3, z3), .normal = glm::vec3(x3, y3, z3), .texture = glm::vec2(theta1, phi1) };

			if (u > 0 && u < Us - 1) {
				sphereGen.addQuad({ v0, v2, v3, v1});
			} else {
				if (u == 0) {
					sphereGen.addTriangle({ v0, v2, v3 });
				} else if (u == Us - 1) {
					sphereGen.addTriangle({ v1, v0, v2 });
				}
			}
		}
	}
	add("sphere", *sphereGen.build().get());

	Geometry::ProceduralBuilder terrain;
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
			glm::vec2 texture = glm::vec2(x / width, y / height);
			glm::vec3 normal = glm::vec3(0.0f, 1.0f, 0.0f);
			Geometry::vertex v0 = { .position = glm::vec3((float)x, (float)d0 * (float)depth, (float)y), .normal = normal, .texture = texture};
			Geometry::vertex v1 = { .position = glm::vec3((float)x, (float)d1 * (float)depth, (float)y + 1.0f), .normal = normal, .texture = texture };
			Geometry::vertex v2 = { .position = glm::vec3((float)x + 1.0f, (float)d2 * (float)depth, (float)y), .normal = normal, .texture = texture };
			Geometry::vertex v3 = { .position = glm::vec3((float)x + 1.0f, (float)d3 * (float)depth, (float)y + 1.0f), .normal = normal, .texture = texture };

			terrain.addQuad({ v0, v2, v3, v1 });
		}
	}

	add("terrain", *terrain.build().get());
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