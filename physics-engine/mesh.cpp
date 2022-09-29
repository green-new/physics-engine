#include "mesh.hpp"
// Best practices for VAOs: https://stackoverflow.com/questions/8923174/opengl-vao-best-practices
// Best practives for vbos, vaos https://www.khronos.org/opengl/wiki/Vertex_Specification_Best_Practices
// Inspiration: https://learnopengl.com/Model-Loading/Mesh

mesh::mesh(std::vector<vertex_t> _verts, std::vector<GLuint> _indices, std::vector<texture_t> _textures) {
	verts = _verts;
	indices = _indices;
	textures = _textures;
	vbo = 0;
	vao = 0;
	ebo = 0;

	setup();
}

mesh::~mesh() {
	glDeleteBuffers(1, &vbo);
	glDeleteBuffers(1, &vao);
	glDeleteBuffers(1, &ebo);
}

void mesh::setup() {
	glGenVertexArrays(1, &vao);
	glGenBuffers(1, &ebo);
	glGenBuffers(1, &vbo);

	glBindVertexArray(vao);
	glBindBuffer(GL_ARRAY_BUFFER, vbo);
	glBufferData(GL_ARRAY_BUFFER, verts.size() * sizeof(vertex_t), (const void*)&verts[0], GL_STATIC_DRAW);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(GLuint), &indices[0], GL_STATIC_DRAW);

	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(vertex_t), (const void*)offsetof(vertex_t, vertex));

	glEnableVertexAttribArray(1);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(vertex_t), (const void*)offsetof(vertex_t, normal));

	glEnableVertexAttribArray(2);
	glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(vertex_t), (const void*)offsetof(vertex_t, texture));

	glBindVertexArray(0);
}

/* Draws the mesh.
https://registry.khronos.org/OpenGL-Refpages/gl4/html/glDrawElements.
*/
void const mesh::draw(shader& prog) {
	for (uint16_t i = 0; i < textures.size(); i++) {
		glActiveTexture(GL_TEXTURE0 + i);
		glBindTexture(GL_TEXTURE_2D, textures[i]);
	}
	glBindVertexArray(vao);
	glDrawElements(GL_TRIANGLES, (GLsizei)indices.size(), GL_UNSIGNED_INT, 0);
}

/*
Generates a platonic mesh with the given textures.
https://www.danielsieger.com/blog/2021/01/03/generating-platonic-solids.html
https://en.wikipedia.org/wiki/Platonic_solid
*/
mesh* platonic_mesh(PlatonicSolid type, std::vector<texture_t> textures) {
	std::vector<vertex_t> verts;
	std::vector<GLuint> indices;
	switch (type) {
	case TETRAHEDRON:

		break;
	case CUBE:
		verts = {
			// X, Y, Z,			NormX, NormY, NormZ,	U, V
			// Front
			{ 1.0f,  1.0f,  1.0f,	0.0f, 0.0f, 1.0f,	1.0f, 1.0f}, // Front-top-right
			{-1.0f,  1.0f,  1.0f,	0.0f, 0.0f, 1.0f,	0.0f, 1.0f}, // Front-top-left
			{ 1.0f, -1.0f,  1.0f,	0.0f, 0.0f, 1.0f,	1.0f, 0.0f}, // Front-bottom-right
			{-1.0f, -1.0f,  1.0f,	0.0f, 0.0f, 1.0f,	0.0f, 0.0f}, // Front-bottom-left

			// Back
			{ 1.0f,  1.0f, -1.0f,	0.0f, 0.0f, -1.0f,	1.0f, 1.0f}, // Back-top-right
			{-1.0f,  1.0f, -1.0f,	0.0f, 0.0f, -1.0f,	0.0f, 1.0f}, // Back-top-left
			{ 1.0f, -1.0f, -1.0f,	0.0f, 0.0f, -1.0f,	1.0f, 0.0f}, // Back-bottom-right
			{-1.0f, -1.0f, -1.0f,	0.0f, 0.0f, -1.0f,	0.0f, 0.0f}, // Back-bottom-left

			// Right
			{ 1.0f,  1.0f,  1.0f,	1.0f, 0.0f, 0.0f,	1.0f, 1.0f}, // Front-top-right
			{ 1.0f,  1.0f, -1.0f,	1.0f, 0.0f, 0.0f,	0.0f, 1.0f}, // Back-top-right
			{ 1.0f, -1.0f, -1.0f,	1.0f, 0.0f, 0.0f,	0.0f, 0.0f}, // Back-bottom-right
			{ 1.0f, -1.0f,  1.0f,	1.0f, 0.0f, 0.0f,	1.0f, 0.0f}, // Front-bottom-right

			// Left
			{-1.0f,  1.0f,  1.0f,	-1.0f, 0.0f, 0.0f,	1.0f, 1.0f}, // Front-top-left
			{-1.0f,  1.0f, -1.0f,	-1.0f, 0.0f, 0.0f,	0.0f, 1.0f}, // Back-top-left
			{-1.0f, -1.0f, -1.0f,	-1.0f, 0.0f, 0.0f,	0.0f, 0.0f}, // Back-bottom-left
			{-1.0f, -1.0f,  1.0f,	-1.0f, 0.0f, 0.0f,	1.0f, 0.0f}, // Front-bottom-left

			// Top
			{ 1.0f,  1.0f,  1.0f,	0.0f, 1.0f, 0.0f,	1.0f, 1.0f}, // Front-top-right
			{-1.0f,  1.0f,  1.0f,	0.0f, 1.0f, 0.0f,	0.0f, 1.0f}, // Front-top-left
			{ 1.0f,  1.0f, -1.0f,	0.0f, 1.0f, 0.0f,	1.0f, 0.0f}, // Back-top-right
			{-1.0f,  1.0f, -1.0f,	0.0f, 1.0f, 0.0f,	0.0f, 0.0f}, // Back-top-left

			// Bottom
			{ 1.0f, -1.0f,  1.0f,	0.0f, 0.0f, -1.0f,	1.0f, 1.0f}, // Front-bottom-right
			{-1.0f, -1.0f,  1.0f,	0.0f, 0.0f, -1.0f,	0.0f, 1.0f}, // Front-bottom-left
			{ 1.0f, -1.0f, -1.0f,	0.0f, 0.0f, -1.0f,	1.0f, 0.0f}, // Back-bottom-right
			{-1.0f, -1.0f, -1.0f,	0.0f, 0.0f, -1.0f,	0.0f, 0.0f}, // Back-bottom-left
		}; // https://github.com/mikeglaz/solids-animation
		indices = {
			// Front
			0, 1, 3,
			3, 2, 0,
			// Back
			4, 5, 7,
			7, 6, 4,
			// Right
			8, 9, 10,
			10, 11, 8,
			// Left
			12, 13, 14,
			14, 15, 12,
			// Top
			16, 17, 18,
			18, 19, 17,
			// Bottom
			20, 21, 22,
			22, 23, 21
		};
		break;
	case OCTAHEDRON:

		break;
	case DODECAHEDRON:

		break;
	case ICOSAHEDRON: {
		GLfloat golden_ratio = std::numbers::phi;
		verts = {
			{0, 1.0f, golden_ratio, 0.0f}
		};
		indices = {

		};
		break;
	}
	default:
		break;
	}

	return new mesh(verts, indices, textures);
}