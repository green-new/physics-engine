#include "texture.hpp"
#include "stb_image/stb_image.h"
#include <iostream>


/* Consider this paper for implementation:
https://redirect.cs.umbc.edu/~olano/s2002c36/ch06.pdf 
Or just find a simpler article on Perlin noise 
https://github.com/Reputeless/PerlinNoise/blob/master/example.cpp
http://anderoonies.github.io/2020/03/21/perlin-colors.html
*/
texture_t noise_texture(color_t primary, color_t secondary, uint16_t width, uint16_t height, double frequency, uint32_t octave) {
	/* Generate a map of perlin values:
	1) Initalize map of width, height
	2) Set map value i, j to RGB color(perlin.octave2D_01(x * fx, y * yf, octaves);
	3) Generate, bind texture
	4) Bind texture to perlin data
	5) Return texture ID */
	const double fx = (frequency / width);
	const double fy = (frequency / height);
	uint32_t seed = (uint32_t)time(NULL);
	const siv::PerlinNoise perlin{ seed };
	/* Number of components */
	const unsigned int Cn = 4;
	float* data = new float[Cn * width * height];
	for (uint16_t i = 0; i < height; i++)
		for (uint16_t j = 0; j < width; j++) {
			double d = perlin.octave2D_01(j * fx, i * fx, octave);
			float r = std::lerp(primary.rgba.red / 255.0f, secondary.rgba.red / 255.0f, d);
			float g = std::lerp(primary.rgba.green / 255.0f, secondary.rgba.green / 255.0f, d);
			float b = std::lerp(primary.rgba.blue / 255.0f, secondary.rgba.blue / 255.0f, d);
			data[(i * width + j) * Cn + 0] = r;
			data[(i * width + j) * Cn + 1] = g;
			data[(i * width + j) * Cn + 2] = b;
			data[(i * width + j) * Cn + 3] = 1.0f;
		}

	texture_t id;
	glGenTextures(1, &id);
	glBindTexture(GL_TEXTURE_2D, id);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_FLOAT, &data[0]);
	glGenerateMipmap(GL_TEXTURE_2D);

	return id;
}

/* https://lodev.org/cgtutor/xortexture.html */
texture_t xor_texture(color_t base, uint16_t width, uint16_t height) {
	texture_t id;
	glGenTextures(1, &id);
	glBindTexture(GL_TEXTURE_2D, id);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	const unsigned int Cn = 3;
	byte *data = new byte[Cn * width * height];

	for (uint16_t y = 0; y < height; y++) {
		for (uint16_t x = 0; x < width; x++) {
			byte res = (x ^ y);
			base.rgba.red = 255 - res;
			base.rgba.green = res;
			base.rgba.blue = res % 128;
			data[(y * width + x) * Cn] = base.rgba.red;
			data[(y * width + x) * Cn + 1] = base.rgba.green;
			data[(y * width + x) * Cn + 2] = base.rgba.blue;
		} 
	}

	if (data) {
		/* Loads the pixel data to the image. Says the image data is RGB and this texture will be RGB with 8-bit unsigned data. */
		glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
		glGenerateMipmap(GL_TEXTURE_2D);
	}
	else {
		std::cout << "XOR texture failed to load. Something is wrong with byte data." << std::endl;
	}

	return id;
}

// https://stackoverflow.com/questions/63295216/create-a-solid-color-texture-in-opengl
texture_t solid_colored_texture(color_t color) {
	texture_t id;
	glGenTextures(1, &id);
	glBindTexture(GL_TEXTURE_2D, id);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);	
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, 1, 1, 0, GL_RGBA, GL_UNSIGNED_BYTE, new unsigned char[] {color.rgba.red, color.rgba.green, color.rgba.blue, color.rgba.alpha});
	glGenerateMipmap(GL_TEXTURE_2D);

	return id;
}

/*
Build an OpenGL Texture Object from the file provided.
https://github.com/nothings/stb
*/
texture_t build_texture(std::string filename) {
	texture_t id;
	glGenTextures(1, &id);
	glBindTexture(GL_TEXTURE_2D, id);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

	std::string ext = filename.substr(filename.find_last_of(".") + 1);
	/* stb_image.h supported file extensions. */
	if (ext != "jpg" && ext != "png" && ext != "tga" && ext != "bmp" && ext != "psd"
	&& ext != "gif" && ext != "hdr" && ext != "pic") {
		std::cout << "Provided filename is not one of the following supported file formats: .jpg, .png, .tga, .bmp, .psd, .gif, .hdr, .pic." << std::endl;;
		return 0;
	}

	/* Use the stb_image library to load the file. */
	int width, height, channels;
	byte* data = stbi_load(filename.c_str(), &width, &height, &channels, 0);
	GLuint internalformat;
	GLenum format;
	if (channels == 3) {
		internalformat = GL_RGB;
		format = GL_RGB;
	}
	else if (channels == 4) {
		internalformat = GL_RGBA;
		format = GL_RGBA;
	}
	if (data) {
		/* Loads the pixel data to the image. Says the image data is RGB and this texture will be RGB with 8-bit unsigned data. */
		glTexImage2D(GL_TEXTURE_2D, 0, internalformat, width, height, 0, format, GL_UNSIGNED_BYTE, data);
		glGenerateMipmap(GL_TEXTURE_2D);
	}
	else {
		std::cout << "Texture failed to load. Possibly an incorrect filename or file format." << std::endl;
		return 0;
	}

	stbi_image_free(data);

	return id;
}

color_t build_colorb(byte r, byte g, byte b, byte a) {
	uint32_t c = a;
	c = (c << 8) + b;
	c = (c << 8) + g;
	c = (c << 8) + r;

	return { c };
}

color_t build_colorf(float r, float g, float b, float a) {
	byte _r = (byte)(r * 255.0f);
	byte _g = (byte)(g * 255.0f);
	byte _b = (byte)(b * 255.0f);
	byte _a = (byte)(a * 255.0f);
	
	return build_colorb(_r, _g, _b, _a);
}
