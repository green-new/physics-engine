#pragma once
#include <cstdint>
#include <vector>
#include <glad/glad.h>
#include "shader.hpp"
#include "texture.hpp"
#include "geometry.hpp"

struct attribute {
	GLuint index;
	GLint size;
	GLenum type;
	GLboolean normalized;
	GLsizei stride;
	const void* pointer;
};

/* Default interleaved position, normal, texture coordinates. */
inline std::vector<attribute> DEFAULT_ATTRIBUTES = {
	{0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(GLfloat), (void*)0},
	{1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(GLfloat), (void*)(3 * sizeof(GLfloat))},
	{2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(GLfloat), (void*)(6 * sizeof(GLfloat))}
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
	mesh(geolib::geometry* g, std::vector<texture_t> textures, int normalConfig);
	~mesh();
	void draw(shader& prog);
private:
	std::vector<GLfloat> _vertices;
	std::vector<GLuint> _indices;
	std::vector<texture_t> _textures;

	vbo* _vertexBuffer;
	vao* _vertexArray;
	ebo* _elementBuffer;

	unsigned int VAO, VBO, EBO;
};