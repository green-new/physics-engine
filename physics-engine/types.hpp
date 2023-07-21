#pragma once
#include <glad/glad.h>
#include <cstdint>
#include <bitset>

/*
* 
* OpenGL-related types and constants.
* 
*/
// Represents a vertex buffer object in OpenGL. Expands to GLuint.
using VBO = GLuint;

// Represents a vertex array object in OpenGL. Expands to GLuint.
using VAO = GLuint;

/*
* 
* Entity-component-system related types and constants.
* 
*/
// Represents an entity as an integer. Expands to uint32_t.
using Entity = uint32_t;

// Represents a particular component. Expands to uint8_t.
using ComponentType = uint8_t;

// The maximum entities for the program. Determined at compile-time.
constexpr Entity MAX_ENTITIES = 10000;

// The maximum number of components for the program. Determined at compile-time.
constexpr ComponentType MAX_COMPONENTS = 32;

// The bitset for a particular system. Helps determine what components a system uses.
using Signature = std::bitset<MAX_COMPONENTS>;

/*
* 
* Texture and color related types and constants.
* 
*/
// Represents a texture object in OpenGL. Expands to GLuint.
using texture_t = GLuint;

// Expands to unsigned char.
using byte = unsigned char;

// The color data type. Integer format is as follows: 0xAARRGGBB due to endianness.
using color_t =
union {
	uint32_t c;
	struct rgba_t {
		byte red;
		byte green;
		byte blue;
		byte alpha;
	} rgba;
};

// Red color as 0xFF0000FF.
const color_t RED = { 0xFF0000FF }; // First byte is alpha (FF, or 255), next two are blue and green, so they are 0 since this is the color red. Last byte is red, so its (0xFF, or 255).

// Green color as 0xFF00FF00.
const color_t GREEN = { 0xFF00FF00 };

// Blue color as 0xFFFF0000.
const color_t BLUE = { 0xFFFF0000 };

// Yellow color as RED bitwise OR GREEN.
const color_t YELLOW = { RED.c | GREEN.c };

// CYAN color as GREEN bitwise OR BLUE.
const color_t CYAN = { GREEN.c | BLUE.c };

// MAGENTA color as RED bitwise OR BLUE.
const color_t MAGENTA = { RED.c | BLUE.c };

// Total presence of all color..
const color_t WHITE = { 0xFFFFFFFF };

// Absence of color.
const color_t BLACK = { 0xFF000000 };
