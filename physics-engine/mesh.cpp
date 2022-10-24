#include "mesh.hpp"
// Best practices for VAOs: https://stackoverflow.com/questions/8923174/opengl-vao-best-practices
// Best practives for vbos, vaos https://www.khronos.org/opengl/wiki/Vertex_Specification_Best_Practices
// Inspiration: https://learnopengl.com/Model-Loading/Mesh

Mesh3D::Mesh3D(geolib::Geometry3D* g) {
	geolib::geometry_adapter adapter = geolib::geometry_adapter(g);
	m_data = adapter.request_data();
	m_vertexArray = new vao();
	m_normalBuffer = new vbo();
	m_positionBuffer = new vbo(); 

	/* We could either create 2 VBOs and dynamically update them in the same VAO, or use 1 VBO with all vertex information.
	For now, we will create two VAOs and bind whichever one we need when drawing. 
	Because of this method, we must bind a new buffer everytime we call glVertexAttribPointer. 
	This is why the vao.config_attributes() method is uninvoked in this program.
	That method would work if we only had 1 VBO to bind. */
	m_vertexArray->bind();
	glEnableVertexAttribArray(0);
	glEnableVertexAttribArray(1);
	m_positionBuffer->bind_data(m_data.flat_vertices.size(), m_data.smooth_vertices.data());
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(GLfloat), (void*)0);
	m_normalBuffer->bind_data(m_data.flat_normals.size(), m_data.smooth_normals.data());
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(GLfloat), (void*)0);
	m_vertexArray->unbind();
}

Mesh3D::~Mesh3D() {
	m_data.flat_vertices.clear();
	m_data.flat_normals.clear();
	m_data.smooth_vertices.clear();
	m_data.smooth_normals.clear();

	delete m_positionBuffer;
	delete m_normalBuffer;
	delete m_vertexArray;
}
/* Determines the number of vertices active right now. */
GLsizei Mesh3D::vertex_count() {
	return m_data.smooth_vertices.size();
}
/* 
* Draws the mesh.
* https://registry.khronos.org/OpenGL-Refpages/gl4/html/glDrawArrays.xhtml
*/
void Mesh3D::draw(Shader& prog) {
	m_vertexArray->bind();
	glDrawArrays(GL_TRIANGLES, 0, vertex_count());
}