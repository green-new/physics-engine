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
	glGenBuffers(1, &vbo);
	glBindBuffer(GL_ARRAY_BUFFER, vbo);
	glBufferData(GL_ARRAY_BUFFER, verts.size() * sizeof(vertex_t), (const void*)verts.data(), GL_STATIC_DRAW);

	glGenVertexArrays(1, &vao);
	glBindVertexArray(vao);

	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(vertex_t), (const void*)offsetof(vertex_t, vertex));

	glEnableVertexAttribArray(1);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(vertex_t), (const void*)offsetof(vertex_t, normal));

	glEnableVertexAttribArray(2);
	glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(vertex_t), (const void*)offsetof(vertex_t, texture));

	glGenBuffers(1, &ebo);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(GLuint), &indices[0], GL_STATIC_DRAW);

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

	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, textures[0]);
	glBindVertexArray(vao);
	glDrawElements(GL_TRIANGLES, (GLsizei)indices.size(), GL_UNSIGNED_INT, 0);
}

/*
Generates a platonic mesh with the given textures.
*/
const mesh platonic_mesh(PlatonicSolid type, std::vector<texture_t> textures) {
	std::vector<vertex_t> verts;
	std::vector<GLuint> indices;
	texture_t tx = 0;
	switch (type) {
	case TETRAHEDRON:

		break;
	case CUBE:

		break;
	case OCTAHEDRON:

		break;
	case DODECAHEDRON:

		break;
	case ICOSAHEDRON:

		break;
	default:
		break;
	}

	return mesh(verts, indices, { tx });
}