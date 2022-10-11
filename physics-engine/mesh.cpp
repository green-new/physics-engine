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

ebo::ebo(GLsizeiptr size, const void* data, GLenum target = GL_ELEMENT_ARRAY_BUFFER, GLenum usage = GL_STATIC_DRAW) : vbo(size, data, target, usage) { }
ebo::~ebo() {}

mesh::mesh() {
	
}

mesh::~mesh() {
	
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
	glBindVertexArray(vertexArray);
	glDrawElements(GL_TRIANGLES, (GLsizei)idx_data.size(), GL_UNSIGNED_INT, 0);
}