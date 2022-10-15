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
	GLuint* get();
	virtual void bind() const = 0;
	virtual void unbind() const = 0;
	virtual ~buffer_object();
protected:
	GLuint _object;
};

/* https://registry.khronos.org/OpenGL-Refpages/gl4/html/glBufferData.xhtml */
class vbo : public buffer_object {
public:
	vbo(GLenum usage = GL_STREAM_DRAW, GLenum target = GL_ARRAY_BUFFER);
	virtual void bind() const override;
	virtual void unbind() const override;
	void bind_data(GLsizeiptr new_size, const void* new_data);
private:
	const GLenum _target;
	const GLenum _usage;
};

/* https://registry.khronos.org/OpenGL-Refpages/gl4/html/glVertexAttribPointer.xhtml */
class vao : public buffer_object {
public:
	vao(std::vector<attribute> attributes = DEFAULT_ATTRIBUTES);
	virtual void bind() const override;
	virtual void unbind() const override;
	void config_attributes();
private:
	std::vector<attribute> _attributes;
};

class mesh {
public:
	mesh(geolib::geometry* g, std::vector<texture_t> textures);
	~mesh();
	void draw(shader& prog);
	void swap();
private:
	geolib::adapter_GLdata _data;
	std::vector<texture_t> _textures;

	vbo* _positionBuffer;
	vbo* _positionBufferSmoothed;
	vbo* _normalBuffer;
	vbo* _normalBufferSmoothed;
	vao* _vertexArray;
	vao* _vertexArraySmoothed;

	GLsizei vertex_count();

	// True for facted, false for smoothed.
	bool flatNormalsOrSmoothedNormals;
};