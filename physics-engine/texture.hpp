#pragma once
#include "Reputeless/PerlinNoise.hpp"
#include "glad/glad.h"
#include <string>
#include <iostream>
#include <cstdint>
#include <cmath>

using texture_t = GLuint;
using byte = unsigned char;

// https://www.flipcode.com/archives/Using_a_Union_for_Packed_Color_Values.shtml
/* Cool use for unions I found.
If unfamiliar with unions, they basically share memory information among all the elements.
Therefore, color_t.c shares the same memory location as red, green, blue, and alpha.
Because it is an int32, and each component is a byte (8 bits), it lines up well to create a way to 
reference the total color value, or each component individually at the same time. */
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

color_t build_colorf(float r, float g, float b, float a);
color_t build_colorb(byte r, byte g, byte b, byte a);

/* It may seem weird, but because of endianness alpha is actually first, then blue, green, red. 
So, color_t.c's memory layout is really 0xAABBGGRR by value. 
We account for this flip in the build_color functions. 
However, when creating them by value, we need to make sure we account for this! */
const color_t RED = { 0xFF0000FF }; // First byte is alpha (FF, or 255), next two are blue and green, so they are 0 since this is the color red. Last byte is red, so its (0xFF, or 255).
const color_t GREEN = { 0xFF00FF00 };
const color_t BLUE = { 0xFFFF0000 };
const color_t YELLOW = { RED.c | GREEN.c };
const color_t CYAN = { GREEN.c | BLUE.c };
const color_t MAGENTA = { RED.c | BLUE.c };
const color_t ORANGE = { build_colorb(255, 110, 0, 255).c };
const color_t WHITE = { 0xFFFFFFFF };
const color_t BLACK = { 0xFF000000 };

texture_t xor_texture(color_t base, uint16_t width, uint16_t height);
texture_t solid_colored_texture(color_t color);
// This name is really bad.
texture_t build_texture(std::string filename);
texture_t noise_texture(color_t primary, color_t secondary, uint16_t width, uint16_t height, double frequency, uint32_t octave);
texture_t skybox();
