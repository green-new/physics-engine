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

const color_t RED =		{	0xFF0000FF	};
const color_t GREEN =	{	0xFF00FF00	};
const color_t BLUE =	{	0xFFFF0000	};

color_t build_color(float r, float g, float b, float a);
color_t build_color(byte r, byte g, byte b, byte a);

texture_t xor_texture(color_t base, uint16_t width, uint16_t height);
texture_t solid_colored_texture(color_t color);
texture_t build_texture(std::string filename);
texture_t noise_texture(color_t base, uint16_t width, uint16_t height, double frequency, uint32_t octave, uint32_t seed);