#pragma once

#include "glad/glad.h"

#define MAX_ATTRIBS 4


// Unique, constant attributes for one VAO attribute
using attrib 
= const struct {
	const std::string	A_name;
	const GLint			A_size;
	const GLenum		A_type;
	const GLboolean		A_normalized;
};

attrib A_POSITION{
	"a_position",
	3,
	GL_FLOAT,
	GL_FALSE
};

attrib A_NORMAL{
	"a_normal",
	3,
	GL_FLOAT,
	GL_FALSE
};

attrib A_COLOR{
	"a_color",
	3,
	GL_FLOAT,
	GL_FALSE
};

attrib A_TEXTURE{
	"a_texture",
	2,
	GL_FLOAT,
	GL_FALSE
};

using attrib_offset_pair =
struct {
	const attrib	attrib;
	const void*		offset;
};

using attrib_list =
const std::vector<attrib_offset_pair>;

GLsizei calculate_stride(const attrib_list& list) {
	GLsizei stride = 0;
	for (auto& a : list) {	// Must be a reference type to be "constexpr"
		stride += a.attrib.A_size * sizeof(a.attrib.A_type);
	}
	return stride;
}

const attrib_list A_POS_NORM_COL = {
	{
		A_POSITION,
		(void*)0
	},
	{
		A_NORMAL,
		(void*)(A_POSITION.A_size * sizeof(A_POSITION.A_type))
	},
	{
		A_COLOR,
		(void*)((A_POSITION.A_size * sizeof(A_POSITION.A_type)) + (A_NORMAL.A_size * sizeof(A_NORMAL.A_type)))
	}
};


const attrib_list A_POS_NORM_TEXT = {
	{
		A_POSITION,
		(void*)0
	},
	{
		A_NORMAL,
		(void*)(A_POSITION.A_size * sizeof(A_POSITION.A_type))
	},
	{
		A_TEXTURE,
		(void*)((A_POSITION.A_size * sizeof(A_POSITION.A_type)) + (A_NORMAL.A_size * sizeof(A_NORMAL.A_type)))
	}
};

