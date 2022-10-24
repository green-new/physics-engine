#pragma once
#include <glad/glad.h>
#include <vector>

struct attribute {
	GLuint index;
	GLint size;
	GLenum type;
	GLboolean normalized;
	GLsizei stride;
	const void* pointer;
};

/* Default attributes. */
inline const std::vector<attribute> DEFAULT_ATTRIBUTES = {
	{0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(GLfloat), (void*)0},
	{1, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(GLfloat), (void*)0}
};

/* The general buffer_object.
Interherited by vbo and vao.
Encapsulates its buffer object value, with some gets and binds (changes based on implementation).
destructor is virtual so it is called when a child is destroyed (the same for all children -> glDeleteBuffers(1, &_object)). */
class buffer_object {
public:
	GLuint* get() {
		return &m_object;
	}
	virtual void bind() const = 0;
	virtual void unbind() const = 0;
	~buffer_object() {
		glDeleteBuffers(1, &m_object);
	}
protected:
	GLuint m_object = 0;
};

/* https://registry.khronos.org/OpenGL-Refpages/gl4/html/glBufferData.xhtml */
class vbo : public buffer_object {
public:
	vbo(GLenum usage = GL_STREAM_DRAW, GLenum target = GL_ARRAY_BUFFER) 
		: m_target(target), m_usage(usage) {
		glGenBuffers(1, &m_object);
	}
	virtual void bind() const override {
		glBindBuffer(m_target, m_object);
	}
	virtual void unbind() const override {
		glBindBuffer(m_target, 0);
	}
	void bind_data(GLsizeiptr new_size, const void* new_data) {
		glBindBuffer(m_target, m_object);
		glBufferData(NULL, NULL, NULL, NULL);
		glBufferData(m_target, new_size * sizeof(GLfloat), new_data, m_usage);
	}
private:
	const GLenum m_target;
	const GLenum m_usage;
};

/* https://registry.khronos.org/OpenGL-Refpages/gl4/html/glVertexAttribPointer.xhtml */
class vao : public buffer_object {
public:
	vao(std::vector<attribute> attributes = DEFAULT_ATTRIBUTES) {
		glGenVertexArrays(1, &m_object);
		_attributes = attributes;
	}
	virtual void bind() const override {
		glBindVertexArray(m_object);
	}
	virtual void unbind() const override {
		glBindVertexArray(0);
	}
private:
	std::vector<attribute> _attributes;
};