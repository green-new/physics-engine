// ==================================================== //
// Include guards										//
// ==================================================== //
#pragma once

// ==================================================== //
// Included dependencies								//
// ==================================================== //
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <string>
#include <fstream>
#include <sstream>
#include <iostream>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
// ==================================================== //
// Forward declared dependencies						//
// ==================================================== //
typedef GLuint SHADER;
typedef GLuint PROGRAM;

// ==================================================== //
// Macro expressions									//
// ==================================================== //

/*
	Classes
*/

class Shader
{
public:
	PROGRAM id;

	/* Constructor */
	Shader(const char* vertex_path, const char* fragment_path);

	void use();
	void del();
	void set_bool(const std::string& name, bool value) const;
	void set_int(const std::string& name, int value) const;
	void set_float(const std::string& name, float value) const;
	void set_mat4(const std::string& name, glm::mat4 mat);
	void set_vec3(const std::string& name, glm::vec3 vec3);
	void set_vec3(const std::string& name, float x, float y, float z);
};

