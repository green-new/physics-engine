#include "mesh.hpp"
// Best practices for VAOs: https://stackoverflow.com/questions/8923174/opengl-vao-best-practices
// Best practives for vbos, vaos https://www.khronos.org/opengl/wiki/Vertex_Specification_Best_Practices
// Inspiration: https://learnopengl.com/Model-Loading/Mesh

GLuint* buffer_object::get() {
	return &_object;
}
buffer_object::~buffer_object() {
	glDeleteBuffers(1, &_object);
}

vbo::vbo(GLenum usage, GLenum target) : _usage(usage), _target(target) {
	glGenBuffers(1, &_object);
}
void vbo::bind() const {
	glBindBuffer(_target, _object);
}
void vbo::unbind() const {
	glBindBuffer(_target, 0);
}
void vbo::bind_data(GLsizeiptr new_size, const void* new_data) {
	glBindBuffer(_target, _object);
	glBufferData(NULL, NULL, NULL, NULL);
	glBufferData(_target, new_size * sizeof(GLfloat), new_data, _usage);
}
vao::vao(std::vector<attribute> attributes) {
	glGenVertexArrays(1, &_object);
	_attributes = attributes;
}
void vao::config_attributes() {
	for (attribute& a : _attributes) {
		glEnableVertexAttribArray(a.index);
		glVertexAttribPointer(a.index, a.size, a.type, a.normalized, a.stride, a.pointer);
	}
}
void vao::bind() const {
	glBindVertexArray(_object);
}
void vao::unbind() const {
	glBindVertexArray(0);
}

mesh::mesh(geolib::geometry* g, std::vector<texture_t> textures) {
	geolib::geometry_adapter adapter = geolib::geometry_adapter(g);
	flatNormalsOrSmoothedNormals = true;
	_data = adapter.request_data();
	_textures = textures;
	_vertexArray = new vao();
	_vertexArraySmoothed = new vao();
	_normalBuffer = new vbo();
	_positionBuffer = new vbo(); 
	_normalBufferSmoothed = new vbo();
	_positionBufferSmoothed = new vbo();

	/* We could either create 2 VBOs and dynamically update them in the same VAO, or use 1 VBO with all vertex information.
	For now, we will create two VAOs and bind whichever one we need when drawing. 
	Because of this method, we must bind a new buffer everytime we call glVertexAttribPointer. 
	This is why the vao.config_attributes() method is uninvoked in this program.
	That method would work if we only had 1 VBO to bind. */
	_vertexArray->bind();
	glEnableVertexAttribArray(0);
	glEnableVertexAttribArray(1);
	_positionBuffer->bind_data(_data.flat_vertices.size(), _data.flat_vertices.data());
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(GLfloat), (void*)0);
	_normalBuffer->bind_data(_data.flat_normals.size(), _data.flat_normals.data());
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(GLfloat), (void*)0);
	_vertexArray->unbind();

	_vertexArraySmoothed->bind();
	glEnableVertexAttribArray(0);
	glEnableVertexAttribArray(1);
	_positionBufferSmoothed->bind_data(_data.smooth_vertices.size(), _data.smooth_vertices.data());
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(GLfloat), (void*)0);
	_normalBufferSmoothed->bind_data(_data.smooth_normals.size(), _data.smooth_normals.data());
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(GLfloat), (void*)0);
	_vertexArraySmoothed->unbind();
}

mesh::~mesh() {
	_data.flat_vertices.clear();
	_data.flat_normals.clear();
	_data.smooth_vertices.clear();
	_data.smooth_normals.clear();

	delete _positionBuffer;
	delete _normalBuffer;
	delete _vertexArray;
}
/* Determines the number of vertices active right now. */
GLsizei mesh::vertex_count() {
	return flatNormalsOrSmoothedNormals ? _data.flat_vertices.size() / 3 : _data.smooth_vertices.size() / 3;
}
/* 
* Draws the mesh.
* https://registry.khronos.org/OpenGL-Refpages/gl4/html/glDrawArrays.xhtml
*/
void mesh::draw(shader& prog) {
	for (uint16_t i = 0; i < _textures.size(); i++) {
		glActiveTexture(GL_TEXTURE0 + i);
		glBindTexture(GL_TEXTURE_2D, _textures[i]);
	}
	if (flatNormalsOrSmoothedNormals)
		_vertexArray->bind();
	else
		_vertexArraySmoothed->bind();

	glDrawArrays(GL_TRIANGLES, 0, vertex_count());
}
void mesh::swap() {
	flatNormalsOrSmoothedNormals = !flatNormalsOrSmoothedNormals;
	if (flatNormalsOrSmoothedNormals) {
		/* Bind to flat normals. */
		_normalBuffer->bind_data(_data.flat_normals.size(), _data.flat_normals.data());
		_positionBuffer->bind_data(_data.flat_vertices.size(), _data.flat_vertices.data());
	}
	else {
		/* Bind to smooth normals. */
		_normalBufferSmoothed->bind_data(_data.smooth_normals.size(), _data.smooth_normals.data());
		_positionBufferSmoothed->bind_data(_data.smooth_vertices.size(), _data.smooth_vertices.data());
	}
}