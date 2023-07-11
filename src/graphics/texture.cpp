#include "texture.h"

#define STB_IMAGE_STATIC
#define STB_IMAGE_IMPLEMENTATION
#include "stb/stb_image.h"

#include <glad/glad.h>
#include <cstdio>

void Texture2D::createFromFile(const char* p) {
	glGenTextures(1, &id);

	glBindTexture(GL_TEXTURE_2D, id);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

	int w, h, chan_num;
	stbi_set_flip_vertically_on_load(true);
	unsigned char *data = stbi_load(p, &w, &h, &chan_num, 0);
	
	if (data) {
	    width = (uint)w;
	    height = (uint)h;

	    GLenum internal_fmt;
	    GLenum data_fmt;
	    if(chan_num == 4) {
			internal_fmt = GL_RGBA8;
			data_fmt = GL_RGBA;
	    } else if(chan_num == 3) {
			internal_fmt = GL_RGB8;
			data_fmt = GL_RGB;
	    }

	    glTexImage2D(GL_TEXTURE_2D, 0, internal_fmt, width, height,
					 0, data_fmt, GL_UNSIGNED_BYTE, data);
	} else {
	    printf("Failed to load texture at path %s\n", p);
		return;
	}

	stbi_image_free(data);
}

void Texture2D::destroy() {
	glDeleteTextures(1, &id);
}

void Texture2D::bind() {
	glBindTexture(GL_TEXTURE_2D, id);
}
    
void Texture2D::unbind() {
	glBindTexture(GL_TEXTURE_2D, 0);
}
