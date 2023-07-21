#pragma once
#include "Reputeless/PerlinNoise.hpp"
#include <glad/glad.h>
#include <string>
#include <iostream>
#include <cstdint>
#include <cmath>
#include "types.hpp"

// Builds a color object with the following r, g, b, a values (clamped to [0.0, 1.0]).
color_t build_colorf(float r, float g, float b, float a);

// Builds a color object with the following r, g, b, a values (clamped to [0, 255].
color_t build_colorb(byte r, byte g, byte b, byte a);

// Generates a xor_texture using OpenGL.
texture_t xor_texture(color_t base, uint16_t width, uint16_t height);

// Generates a solid colored texture with a specific color.
texture_t solid_colored_texture(color_t color);

// Build a texture from a filename using stb_image and OpenGL.
texture_t build_texture(std::string filename);

// Generate a texture using PerlinNoise and OpenGL.
texture_t noise_texture(color_t primary, color_t secondary, uint16_t width, uint16_t height, double frequency, uint32_t octave);

// Generates the skybox texture with OpenGL and stb_image.
texture_t skybox();
