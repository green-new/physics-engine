#pragma once

#include "Reputeless/PerlinNoise.hpp"
#include "glad/glad.h"
#include <string>
#include <iostream>
#include <cstdint>

using texture_t = GLuint;
using byte = unsigned char;

// https://www.flipcode.com/archives/Using_a_Union_for_Packed_Color_Values.shtml
using color_t = 
union {
	uint32_t c;
	struct rgba_t {
		unsigned char red;
		unsigned char green;
		unsigned char blue;
		unsigned char alpha;
	} rgba;
};

const color_t RED = { 0xFF0000FF };
const color_t GREEN = { 0x00FF00FF };
const color_t BLUE = { 0x0000FFFF };

texture_t xor_texture(color_t base, int width, int height);
texture_t solid_colored_texture(color_t color);
texture_t build_texture(std::string filename);
texture_t noise_texture(color_t base, int width, int height, double frequency, uint32_t octave, uint32_t seed);