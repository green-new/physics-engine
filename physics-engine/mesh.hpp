#pragma once
#include <cstdint>
#include <vector>
#include "glad/glad.h"
#include "shader.hpp"
#include "texture.hpp"
#include "geometry.hpp"

typedef struct attribute {
	GLuint index;
	GLint size;
	GLenum type;
	GLboolean normalized;
	GLsizei stride;
	const void* pointer;

	attribute* next;
} attribute;

/* Default interleaved position, normal, texture coordinates. */
static std::vector<attribute> DEFAULT_ATTRIBUTES = {
	{0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(GLfloat), (const void*)0},
	{1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(GLfloat), (const void*)(3 * sizeof(GLfloat))},
	{2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(GLfloat), (const void*)(6 * sizeof(GLfloat))}
};

class attribute_list {
public:
	attribute_list(attribute* a);
	~attribute_list() = default;
	void add_attribute(attribute* a);
	void del_attribute(attribute* a);
private:
	attribute* head;
};

class buffer_object {
public:
	GLuint* get();
	virtual void bind() const = 0;
	virtual void unbind() const = 0;

	virtual ~buffer_object() = default;
protected:
	GLuint _object;
};

/* https://registry.khronos.org/OpenGL-Refpages/gl4/html/glBufferData.xhtml */
class vbo : public buffer_object {
public:
	vbo(GLsizeiptr size, const void* data, GLenum target = GL_ARRAY_BUFFER, GLenum usage = GL_STATIC_DRAW);
	// Declaring destructor virtual runs derived destructor -> base destructor (here).
	virtual ~vbo();
	virtual void bind() const;
	virtual void unbind() const;
private:
	GLenum _target;
};

/* https://registry.khronos.org/OpenGL-Refpages/gl4/html/glVertexAttribPointer.xhtml */
class vao : public buffer_object {
public:
	vao(std::vector<attribute> attributes = DEFAULT_ATTRIBUTES);
	virtual ~vao();
	virtual void bind() const;
	virtual void unbind() const;
};

class ebo : public vbo {
public:
	ebo(GLsizeiptr size, const void* data, GLenum target = GL_ELEMENT_ARRAY_BUFFER, GLenum usage = GL_STATIC_DRAW);
	virtual ~ebo();
};

class mesh {
public:
	mesh(geolib::geometry* meshStructure, std::vector<texture_t> textures, unsigned int normalConfig);
	mesh(geolib::geometry_adapter adapter, std::vector<texture_t> textures);
	~mesh();
	void const draw(shader& prog); 
private:
	geolib::geometry* _meshStructure;
	std::vector<GLfloat> _vertices;
	std::vector<GLuint> _indices;
	std::vector<texture_t> _textures;

	vbo* _vertexBuffer;
	vao* _vertexArray;
	ebo* _elementBuffer;
};