#include "framebuffer.h"

#include "../core/log.h"

#include <glad/glad.h>

#define MAX_FRAMEBUFFER_SIZE 8192

bool Framebuffer::create(FramebufferData new_data) {
	data = new_data;
	
	return invalidate();
}

void Framebuffer::destroy() {
	glDeleteFramebuffers(1, &id);
	glDeleteTextures(1, &color_attachment_id);
	glDeleteTextures(1, &depth_attachment_id);
}

void Framebuffer::bind() {
	glBindFramebuffer(GL_FRAMEBUFFER, id);
	glViewport(0, 0, data.width, data.height);
}

void Framebuffer::unbind() {
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

bool Framebuffer::resize(uint width, uint height) {
	if(width == 0 || height == 0 || width > MAX_FRAMEBUFFER_SIZE || height > MAX_FRAMEBUFFER_SIZE) {
	    LOG_ERROR("Can not resize framebuffer more that %d. Attempted width: %d, height: %d\n",
		    MAX_FRAMEBUFFER_SIZE, width, height);
	    return false;
	}
	
	data.width = width;
	data.height = height;

	return invalidate();
}
	
bool Framebuffer::invalidate() {
	if(id) {
	    glDeleteFramebuffers(1, &id);
	    glDeleteTextures(1, &color_attachment_id);
		glDeleteTextures(1, &depth_attachment_id);
	}

	glCreateFramebuffers(1, &id);
	glBindFramebuffer(GL_FRAMEBUFFER, id);

	glCreateTextures(GL_TEXTURE_2D, 1, &color_attachment_id);
	glBindTexture(GL_TEXTURE_2D, color_attachment_id);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, data.width, data.height, 0, GL_RGBA,
				 GL_UNSIGNED_BYTE, nullptr);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D,
						   color_attachment_id, 0);

	glCreateTextures(GL_TEXTURE_2D, 1, &depth_attachment_id);
	glBindTexture(GL_TEXTURE_2D, depth_attachment_id);
	glTexStorage2D(GL_TEXTURE_2D, 1, GL_DEPTH24_STENCIL8, data.width, data.height);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_TEXTURE_2D,
						   depth_attachment_id, 0);

	if(glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE) {
	    LOG_ERROR("Can not create framebuffer\n");
		
		return false;
	}

	glBindFramebuffer(GL_FRAMEBUFFER, 0);

	return true;
}
