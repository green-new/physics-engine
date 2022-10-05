#pragma once
#include <cstdint>
#include <vector>
#include <numbers>
#include "glad/glad.h"
#include "shader.hpp"
#include "texture.hpp"

using PlatonicSolid =
enum {
	TETRAHEDRON,
	CUBE,
	OCTAHEDRON,
	DODECAHEDRON,
	ICOSAHEDRON
};

using vertex_t = 
struct {
	GLfloat vertex[3];
	GLfloat normal[3];
	GLfloat texture[2];
};

class mesh {
public:
	mesh(std::vector<vertex_t> _verts, std::vector<GLuint> _indices, std::vector<texture_t> _textures);
	~mesh();
	void const draw(shader& prog);
private:
	std::vector<vertex_t> verts;
	std::vector<GLuint> indices;
	std::vector<texture_t> textures;
	GLuint vbo;
	GLuint vao;
	GLuint ebo;

	void setup();
};

mesh* platonic_mesh(PlatonicSolid type, std::vector<texture_t> textures);