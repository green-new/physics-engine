#include <iostream>
#include <fstream>
#include <string>
#include "shader.hpp"
// Inspiration from https://learnopengl.com/Getting-started/Shaders.
// Shaders are very boilerplatey. We have to do quite a bit before they are functional.
// A little deviation using glm's own datatypes.
// Therefore, in order to use this library, glm must be included. This isnt optimal for other programs.
/*
	Functions
*/

void gfx_runtime_error(const char* msg, int code)
{
	std::cout << msg << std::endl;
	glfwTerminate();
	std::exit(code);
}

GLint get_shader_compile_status(unsigned int shader)
{
	GLint success;
	glGetShaderiv(shader, GL_COMPILE_STATUS, &success);

	return success;
}

GLint get_shader_link_status(unsigned int shader)
{
	GLint success;
	glGetShaderiv(shader, GL_LINK_STATUS, &success);

	return success;
}

Shader::Shader(const char* vertex_path, const char* fragment_path)
{
	// 1. Retrieve the vertex/fragment source code from filepath
	std::string svertex;
	std::string sfragment;
	std::ifstream fvertex;
	std::ifstream ffragment;
	// Ensure ifstream objects can throw exceptions:
	fvertex.exceptions(std::ifstream::failbit | std::ifstream::badbit);
	ffragment.exceptions(std::ifstream::failbit | std::ifstream::badbit);
	try
	{
		// open files
		fvertex.open(vertex_path);
		ffragment.open(fragment_path);
		std::stringstream ssvertex, ssfragment;
		// read file's buffer contents into streams
		ssvertex << fvertex.rdbuf();
		ssfragment << ffragment.rdbuf();
		// close file handlers
		fvertex.close();
		ffragment.close();
		// convert stream into string
		svertex = ssvertex.str();
		sfragment = ssfragment.str();
	}
	catch (std::ifstream::failure e)
	{
		gfx_runtime_error("shader file was not successfuly read", -5);
	}

	// C-style string of the vertex and fragment code
	const char* cstr_vertex = svertex.c_str();
	const char* cstr_fragment = sfragment.c_str();

	SHADER vertex, fragment;

	vertex = glCreateShader(GL_VERTEX_SHADER);
	fragment = glCreateShader(GL_FRAGMENT_SHADER);
	glShaderSource(vertex, 1, &cstr_vertex, NULL);
	glShaderSource(fragment, 1, &cstr_fragment, NULL);
	glCompileShader(vertex);
	glCompileShader(fragment);
	int vs_status = get_shader_compile_status(vertex);
	int fs_status = get_shader_compile_status(fragment);
	if (!vs_status)
	{
		char info_log[512];
		glGetShaderInfoLog(vertex, 512, NULL, info_log);
		gfx_runtime_error(info_log, -2);
	}
	if (!fs_status)
	{
		char info_log[512];
		glGetShaderInfoLog(fragment, 512, NULL, info_log);
		gfx_runtime_error(info_log, -3);
	}

	id = glCreateProgram();
	glAttachShader(id, vertex);
	glAttachShader(id, fragment);
	glLinkProgram(id);

	int link_status = get_shader_link_status(id);
	if (!link_status)
	{
		char info_log[512];
		glGetProgramInfoLog(id, 512, NULL, info_log);
		gfx_runtime_error(info_log, -4);
	}
	glDeleteShader(vertex);
	glDeleteShader(fragment);
}

void Shader::del() const
{
	glDeleteProgram(id);
}

void Shader::use() const
{
	glUseProgram(id);
}

void Shader::set_bool(const std::string& name, bool value) const
{
	glUniform1i(glGetUniformLocation(id, name.c_str()), (int)value);
}
void Shader::set_int(const std::string& name, int value) const
{
	glUniform1i(glGetUniformLocation(id, name.c_str()), value);
}
void Shader::set_float(const std::string& name, float value) const
{
	glUniform1f(glGetUniformLocation(id, name.c_str()), value);
}

void Shader::set_mat4(const std::string& name, glm::mat4 mat) const
{
	glUniformMatrix4fv(glGetUniformLocation(id, name.c_str()), 1, GL_FALSE, glm::value_ptr(mat));
}

void Shader::set_vec3(const std::string& name, glm::vec3 vec) const
{
	glUniform3fv(glGetUniformLocation(id, name.c_str()), 1, glm::value_ptr(vec));
}

void Shader::set_vec3(const std::string& name, float x, float y, float z) const
{
	glm::vec3 vec(x, y, z);
	glUniform3fv(glGetUniformLocation(id, name.c_str()), 1, glm::value_ptr(vec));
}