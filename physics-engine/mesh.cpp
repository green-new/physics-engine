#include "mesh.hpp"
// Best practices for VAOs: https://stackoverflow.com/questions/8923174/opengl-vao-best-practices
// Best practives for vbos, vaos https://www.khronos.org/opengl/wiki/Vertex_Specification_Best_Practices
// Inspiration: https://learnopengl.com/Model-Loading/Mesh

vao* _defaultVao = new vao();

GLuint* buffer_object::get() {
	return &_object;
}

vbo::vbo(GLsizeiptr size, const void* data, GLenum target, GLenum usage) {
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

ebo::ebo(GLsizeiptr size, const void* data, GLenum target, GLenum usage) : vbo(size, data, target, usage) { }
ebo::~ebo() {}

mesh::mesh(geolib::geometry* meshStructure, std::vector<texture_t> textures, unsigned int normalConfig) {
	_meshStructure = meshStructure;
	_textures = textures;
	geolib::geometry_adapter adapter = geolib::geometry_adapter(meshStructure, normalConfig);
	_vertices = adapter.request_vertices();
	_indices = adapter.request_faces();
	_vertexBuffer = new vbo(_vertices.size(), _vertices.data());
	_vertexArray = new vao();
	_elementBuffer = new ebo(_indices.size(), _indices.data());
}
mesh::mesh(geolib::geometry_adapter adapter, std::vector<texture_t> textures) {
	_meshStructure = adapter._geo_data;
	_textures = textures;
	_vertices = adapter.request_vertices();
	_indices = adapter.request_faces();
	_vertexBuffer = new vbo(_vertices.size(), _vertices.data());
	_vertexArray = new vao();
	_elementBuffer = new ebo(_indices.size(), _indices.data());
}

mesh::~mesh() {
	delete _meshStructure;
	_vertices.clear();
	_indices.clear();
	delete _vertexBuffer;
	delete _vertexArray;
	delete _elementBuffer;
}

/* 
* Draws the mesh.
* https://registry.khronos.org/OpenGL-Refpages/gl4/html/glDrawElements.
*/
void const mesh::draw(shader& prog) {
	for (uint16_t i = 0; i < _textures.size(); i++) {
		glActiveTexture(GL_TEXTURE0 + i);
		glBindTexture(GL_TEXTURE_2D, _textures[i]);
	}
	_vertexArray->bind();
	glDrawElements(GL_TRIANGLES, (GLsizei)_indices.size(), GL_UNSIGNED_INT, 0);
}