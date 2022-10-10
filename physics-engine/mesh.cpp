#include "mesh.hpp"
// Best practices for VAOs: https://stackoverflow.com/questions/8923174/opengl-vao-best-practices
// Best practives for vbos, vaos https://www.khronos.org/opengl/wiki/Vertex_Specification_Best_Practices
// Inspiration: https://learnopengl.com/Model-Loading/Mesh

GLuint buffer_object::get() const {
	return _object;
}

vbo::vbo(GLsizeiptr size, const void* data, GLenum target = GL_ARRAY_BUFFER, GLenum usage = GL_STATIC_DRAW) {
	_target = target;
	glGenBuffers(1, &_object);
	glBindBuffer(target, _object);
	glBufferData(target, size, data, usage);
}
vbo::~vbo() {
	glDeleteBuffers(1, &_object);
}
void vbo::bind() const {
	glBindBuffer(_target, _object);
}
void vbo::unbind() const {
	glBindBuffer(_target, 0);
}

vao::vao(std::vector<attribute> attributes) {
	glGenVertexArrays(1, &_object);
	// Less expensive
	for (const auto& a : attributes) {
		glEnableVertexAttribArray(a.index);
		glVertexAttribPointer(a.index, a.size, a.type, a.normalized, a.stride, a.pointer);
	}
}
vao::~vao() {
	glDeleteBuffers(1, &_object);
}
void vao::bind() const {
	glBindVertexArray(_object);
}
void vao::unbind() const {
	glBindVertexArray(0);
}

mesh::mesh(std::vector<texture_t> _textures) {
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
	glBufferData(GL_ARRAY_BUFFER, vertex_data.size() * sizeof(GLfloat), vertex_data.data(), GL_STATIC_DRAW);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, idx_data.size() * sizeof(GLuint), idx_data.data(), GL_STATIC_DRAW);

	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(GLfloat), (const void*)0);

	glEnableVertexAttribArray(1);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(GLfloat), (const void*)(3 * sizeof(GLfloat)));

	glEnableVertexAttribArray(2);
	glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(GLfloat), (const void*)(6 * sizeof(GLfloat)));

	glBindVertexArray(0);
}

/* 
* Draws the mesh.
* https://registry.khronos.org/OpenGL-Refpages/gl4/html/glDrawElements.
*/
void const mesh::draw(shader& prog) {
	for (uint16_t i = 0; i < textures.size(); i++) {
		glActiveTexture(GL_TEXTURE0 + i);
		glBindTexture(GL_TEXTURE_2D, textures[i]);
	}
	glBindVertexArray(vao);
	glDrawElements(GL_TRIANGLES, (GLsizei)idx_data.size(), GL_UNSIGNED_INT, 0);
}