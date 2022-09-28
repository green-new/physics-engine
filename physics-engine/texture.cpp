#include "texture.hpp"

#define STB_IMAGE_IMPLEMENTATION
#include "stb_image/stb_image.h"

/* Consider this paper for implementation:
https://redirect.cs.umbc.edu/~olano/s2002c36/ch06.pdf 
Or just find a simpler article on Perlin noise 
https://github.com/Reputeless/PerlinNoise/blob/master/example.cpp
http://anderoonies.github.io/2020/03/21/perlin-colors.html
*/
GLuint noise_texture(color_t base, int width, int height, double frequency, uint32_t octave, uint32_t seed) {
	/* Generate a map of perlin values:
	1) Initalize map of width, height
	2) Set map value i, j to RGB color(perlin.octave2D_01(x * fx, y * yf, octaves);
	3) Generate, bind texture
	4) Bind texture to perlin data
	5) Return texture ID */
	const double fx = (frequency / width);
	const double fy = (frequency / height);
	const siv::PerlinNoise perlin{ seed };
	byte* data = new byte[width * height * 4];
	for (uint16_t i = 0; i < height; i++)
		for (uint16_t j = 0; j < width; j++) {
			double d = perlin.octave2D_01(j * fx, i * fx, octave);
			byte red = (byte)(base.rgba.red * d);
			byte green = (byte)(base.rgba.green * d);
			byte blue = (byte)(base.rgba.blue * d);
			unsigned int c = red;
			c = (c << 8) + green;
			c = (c << 8) + blue;
			data[(i * height) + j + 0] = (byte)(red * d);
			data[(i * height) + j + 1] = (byte)(green * d);
			data[(i * height) + j + 2] = (byte)(blue * d);
			data[(i * height) + j + 3] = base.rgba.alpha;
		}

	GLuint id;
	glGenTextures(1, &id);
	glBindTexture(GL_TEXTURE_2D, id);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, (const void*)data);
	glGenerateMipmap(GL_TEXTURE_2D);

	return id;
}

/* https://lodev.org/cgtutor/xortexture.html */
GLuint xor_texture(color_t base, int width, int height) {
	GLuint id;
	glGenTextures(1, &id);
	glBindTexture(GL_TEXTURE_2D, id);
	byte *data = new byte[width * height * 4];

	for (uint16_t y = 0; y < height; y++) {
		for (uint16_t x = 0; x < width; x++) {
			byte res = (x ^ y);
			base.rgba.red = 255 - res;
			base.rgba.green = res;
			base.rgba.blue = res % 128;
			data[y * height + x + 0] = base.rgba.red;
			data[y * height + x + 1] = base.rgba.green;
			data[y * height + x + 2] = base.rgba.blue;
			data[y * height + x + 3] = base.rgba.alpha;
		}
	}

	if (data) {
		/* Loads the pixel data to the image. Says the image data is RGB and this texture will be RGB with 8-bit unsigned data. */
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, (const void*)data);
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
	glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);	
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

	byte *data = new byte[4 * sizeof(byte)];
	data[0] = color.rgba.red;
	data[1] = color.rgba.green;
	data[2] = color.rgba.blue;
	data[3] = color.rgba.alpha;

	if (data) {
		/* Loads the pixel data to the image. Says the image data is RGB and this texture will be RGB with 8-bit unsigned data. */
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, 1, 1, 0, GL_RGBA, GL_UNSIGNED_BYTE, new unsigned char[] {color.rgba.red, color.rgba.blue, color.rgba.green, color.rgba.green});
		std::cout << data << std::endl;
		glGenerateMipmap(GL_TEXTURE_2D);
	}
	else {
		std::cout << "Texture failed to load. Void color data detected." << std::endl;
	}
	glBindTexture(GL_TEXTURE_2D, 0);
	delete [] data;

	return id;
}

/*
Build an OpenGL Texture Object from the file provided.
https://github.com/nothings/stb
*/
texture_t build_texture(std::string filename) {
	GLuint id;
	glGenTextures(1, &id);
	glBindTexture(GL_TEXTURE_2D, id);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

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
	
	for (uint16_t i = 0; i < height; i++) {
		for (uint16_t j = 0; j < width; j++) {
			std::cout << data[(i * height) + j];
		}
		std::cout << std::endl;
	}

	if (data) {
		/* Loads the pixel data to the image. Says the image data is RGB and this texture will be RGB with 8-bit unsigned data. */
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, data);
		glGenerateMipmap(GL_TEXTURE_2D);
	}
	else {
		std::cout << "Texture failed to load. Possibly an incorrect filename or file format." << std::endl;
		return 0;
	}

	stbi_image_free(data);

	return id;
}