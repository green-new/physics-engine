#pragma once
#include <cstdint>
#include <vector>
#include "glad/glad.h"
#include "shader.hpp"
#include "texture.hpp"

typedef const struct attribute {
	GLuint index;
	GLint size;
	GLenum type;
	GLboolean normalized;
	GLsizei stride;
	const void* pointer;
} attribute;

/* Default interleaved position, normal, texture coordinates. */
static std::vector<attribute> DEFAULT_ATTRIBUTES = {
	{0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(GLfloat), (const void*)0},
	{1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(GLfloat), (const void*)(3 * sizeof(GLfloat))},
	{2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(GLfloat), (const void*)(6 * sizeof(GLfloat))}
};

class attribute_list {
public:
	attribute_list(std::vector<attribute> );
	~attribute_list() = default;
	void add_attribute(const attribute& a);
private:
	std::vector<attribute> m_attrib_list;
};

class buffer_object {
public:
	GLuint get() const;
	virtual void bind() const = 0;
	virtual void unbind() const = 0;
protected:
	GLuint _object;
};

/* https://registry.khronos.org/OpenGL-Refpages/gl4/html/glBufferData.xhtml */
class vbo : public buffer_object {
public:
	vbo(GLsizeiptr size, const void* data, GLenum target, GLenum usage);
	~vbo();
	virtual void bind() const = 0;
	virtual void unbind() const = 0;
private:
	GLenum _target;
};

/* https://registry.khronos.org/OpenGL-Refpages/gl4/html/glVertexAttribPointer.xhtml */
class vao : public buffer_object {
	vao(std::vector<attribute> attributes = DEFAULT_ATTRIBUTES);
	~vao();
	virtual void bind() const = 0;
	virtual void unbind() const = 0;
};

class ebo : public buffer_object {

};

class mesh {
public:
	mesh(std::vector<texture_t> _textures);
	~mesh();
	void const draw(shader& prog);
private:
	std::vector<GLfloat> vertex_data;
	std::vector<GLuint> idx_data;
	std::vector<texture_t> textures;


	void setup();
};