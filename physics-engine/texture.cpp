#include "texture.hpp"
#include "stb_image/stb_image.h"
#include <iostream>
/*
https://en.wikipedia.org/wiki/Smoothstep
Creates a function where the derivatives at x = 0 and x = 1 is 1 (flat).
Technically this implements what is called smootherstep, and it is not visually different from smoothstep in this application.
It may be vastly different in 3D applications, such as randomly generated terrain in 3D environments.
*/
float smoothstep(float edge0, float edge1, float x) {
	// Scales into [0, 1] float range.
	x = std::clamp((x - edge0) / (edge1 - edge0), 0.0f, 1.0f);

	return x * x * x * (x * (x * 6 - 15) + 10);
}

/*
* Finds the GLenum associated with this number of color channels.
*/
GLenum get_format(int channels) {
	switch (channels) {
	case 0:
		return GL_INVALID_ENUM;
	case 1:
		return GL_R;
	case 2:
		return GL_RG;
	case 3:
		return GL_RGB;
	case 4:
		return GL_RGBA;
	}
	return GL_INVALID_ENUM;
}

/* Consider this paper for implementation:
https://redirect.cs.umbc.edu/~olano/s2002c36/ch06.pdf 
Or just find a simpler article on Perlin noise 
https://github.com/Reputeless/PerlinNoise/blob/master/example.cpp
http://anderoonies.github.io/2020/03/21/perlin-colors.html
*/
texture_t noise_texture(color_t primary, color_t secondary, uint16_t width, uint16_t height, double frequency, uint32_t octave) {
	/* Algorithm:
	1) Initalize map of width, height
	2) Set map value i, j to RGB color(perlin.octave2D_01(x * fx, y * yf, octaves);
	3) Generate, bind texture
	4) Bind texture to perlin data
	5) Return texture ID */
	const double fx = (frequency / width);
	const double fy = (frequency / height);
	uint32_t seed = (uint32_t)time(NULL);
	const siv::PerlinNoise perlin{ seed };
	/* Number of color components (R, G, B, A) */
	const unsigned int Cn = 4;
	float* data = new float[Cn * width * height];
	/* O(n * n * octaves). Kinda slow */
	for (uint16_t i = 0; i < height; i++)
		for (uint16_t j = 0; j < width; j++) {
			/* Octave implementation is inherently O(n) */
			double d = perlin.octave2D_01(j * fx, i * fx, octave);
			/* We use smoothstep instead of linear interpolation for more realistic color blending. */
			float sx = smoothstep(0.0f, 1.0f, (float)d);
			float r = std::lerp(primary.rgba.red / 255.0f, secondary.rgba.red / 255.0f, sx);
			float g = std::lerp(primary.rgba.green / 255.0f, secondary.rgba.green / 255.0f, sx);
			float b = std::lerp(primary.rgba.blue / 255.0f, secondary.rgba.blue / 255.0f, sx);
			data[(i * width + j) * Cn + 0] = r;
			data[(i * width + j) * Cn + 1] = g;
			data[(i * width + j) * Cn + 2] = b;
			/* The alpha value. Perhaps create a function parameter for this in the future? */
			data[(i * width + j) * Cn + 3] = 1.0f;
		}

	texture_t id;
	glGenTextures(1, &id);
	glBindTexture(GL_TEXTURE_2D, id);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	/* Ensure we use a GL_RGBA internal and external color format, for now. */
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_FLOAT, &data[0]);
	glGenerateMipmap(GL_TEXTURE_2D);

	return id;
}

/* 
https://lodev.org/cgtutor/xortexture.html 
The XOR texture is used in some raycasting applications. This is the same technique Wolfenstein 3D used when it was conceived.
It is a testing texture to ensure proper alignment of textures, and is a simple texture to procedurally generate.
*/
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
			/* Algorithm derived from Lode Vandevenne. */
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

/* 
https://stackoverflow.com/questions/63295216/create-a-solid-color-texture-in-opengl 
Creates a solid colored texture that uses only 1 pixel of data.
*/
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
https://github.com/nothings/stb
Build an OpenGL Texture Object from the file provided.
Must be of valid filetype (jpg, png, tga, bmp, psd, gif, hdr, pic, or else it returns 0.
Dynamically determines the number of channels in the image to ensure proper format usage (GL_RGBA or GL_RGB).
Assumes most textures will either have 3 or 4 channels.
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
	GLuint internalformat = get_format(channels);
	GLenum format = get_format(channels);
	if (data) {
		/* Loads the pixel data to the image. Says the image data is RGB and this texture will be RGB with 8-bit unsigned data. */
		glTexImage2D(GL_TEXTURE_2D, 0, internalformat, width, height, 0, format, GL_UNSIGNED_BYTE, data);
		glGenerateMipmap(GL_TEXTURE_2D);
	}
	else {
		throw std::exception("Texture failed to load. Possibly an incorrect filename or file format.");
	}

	stbi_image_free(data);

	return id;
}

/*
Creates the skybox.
Uses the CUBE_MAP texture.
Uses 6 different textures to create a cubemap texture.
*/
texture_t skybox() {
	texture_t id;
	// Must be right, left, top, bottom, front, back.
	std::vector<std::string> skybox_textures = {
		"textures/skybox_right.png",
		"textures/skybox_left.png",
		"textures/skybox_top.png",
		"textures/skybox_bottom.png",
		"textures/skybox_front.png",
		"textures/skybox_back.png",
	};
	glGenTextures(1, &id);
	glBindTexture(GL_TEXTURE_CUBE_MAP, id);
	int width, height, channels;
	unsigned char* data;
	for (unsigned int i = 0; i < skybox_textures.size(); i++) {
		data = stbi_load(skybox_textures[i].c_str(), &width, &height, &channels, 0);
		// Test is the data loaded correctly.
		if (data) {
			glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, get_format(channels), width, height, 0, get_format(channels), GL_UNSIGNED_BYTE, data);
			stbi_image_free(data);
		}
		else {
			throw std::exception("Texture failed to load. Possibly an incorrect filename or file format.");
			stbi_image_free(data);
		}
	}

	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);

	return id;
}

/*
Builds a color_t type with the following byte values.
r, g, b, a [0 .. 255]. Clamped otherwise.
*/
color_t build_colorb(byte r, byte g, byte b, byte a) {
	r = std::clamp(r, (byte)0, (byte)255);
	g = std::clamp(g, (byte)0, (byte)255);
	b = std::clamp(b, (byte)0, (byte)255);
	a = std::clamp(a, (byte)0, (byte)255);
	uint32_t c = a;
	c = (c << 8) + b;
	c = (c << 8) + g;
	c = (c << 8) + r;

	return { c };
}

/*
Builds a color_t type with the following float values.
r, g, b, a [0 .. 1]. Clamped otherwise.
*/
color_t build_colorf(float r, float g, float b, float a) {
	r = std::clamp(r, 0.0f, 1.0f);
	g = std::clamp(g, 0.0f, 1.0f);
	b = std::clamp(b, 0.0f, 1.0f);
	a = std::clamp(a, 0.0f, 1.0f);
	byte _r = (byte)(r * 255.0f);
	byte _g = (byte)(g * 255.0f);
	byte _b = (byte)(b * 255.0f);
	byte _a = (byte)(a * 255.0f);
	
	return build_colorb(_r, _g, _b, _a);
}
