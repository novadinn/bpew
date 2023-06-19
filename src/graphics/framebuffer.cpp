#include "framebuffer.h"

#include "../core/log.h"

#include <glm/gtc/type_ptr.hpp>

#define MAX_FRAMEBUFFER_SIZE 8192

bool Framebuffer::create(FramebufferData new_data) {
    data = new_data;
	
    return invalidate();
}

void Framebuffer::destroy() {
    glDeleteFramebuffers(1, &id);
    glDeleteTextures(color_attachment_ids.size(), &color_attachment_ids[0]);
    glDeleteRenderbuffers(1, &depth_attachment_id);
}

void Framebuffer::bind() {
    glBindFramebuffer(GL_FRAMEBUFFER, id);
    glViewport(0, 0, data.width, data.height);
}

void Framebuffer::unbind() {
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void Framebuffer::bindReadAttachment(uint index) {
    if(index >= color_attachment_ids.size()) {
	LOG_ERROR("Color attachment index exeeds number of color attachments\n");
	return;
    }
	
    glReadBuffer(GL_COLOR_ATTACHMENT0 + index);
}

void Framebuffer::bindDrawAttachment(uint index) {
    if(index >= color_attachment_ids.size()) {
	LOG_ERROR("Color attachment index exeeds number of color attachments\n");
	return;
    }
	
    glDrawBuffer(GL_COLOR_ATTACHMENT0 + index);
}

void Framebuffer::blitTo(Framebuffer other) {
    glBindFramebuffer(GL_READ_FRAMEBUFFER, id);
    glBindFramebuffer(GL_DRAW_FRAMEBUFFER, other.getID());
    glBlitFramebuffer(0, 0, data.width, data.height, 0, 0, data.width, data.height, GL_COLOR_BUFFER_BIT, GL_NEAREST);
}

void Framebuffer::blitFrom(Framebuffer other) {
    glBindFramebuffer(GL_READ_FRAMEBUFFER, other.getID());
    glBindFramebuffer(GL_DRAW_FRAMEBUFFER, id);
    glBlitFramebuffer(0, 0, data.width, data.height, 0, 0, data.width, data.height, GL_COLOR_BUFFER_BIT, GL_NEAREST);
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

int Framebuffer::readR32IPixel(int x, int y) {
    int pixel_data;
    glReadPixels(x, y, 1, 1, GL_RED_INTEGER, GL_INT, &pixel_data);

    return pixel_data;
}

void Framebuffer::clearR32IColorAttachment(uint index, int value) {
    if(index >= color_attachment_ids.size()) {
	LOG_ERROR("Color attachment index exeeds number of color attachments\n");
	return;
    }

    glClearTexImage(color_attachment_ids[index], 0, GL_RED_INTEGER, GL_INT, &value);
}

glm::vec4 Framebuffer::readRGBA8Pixel(int x, int y) {
    glm::vec4 pixel_data;
    glReadPixels(x, y, 1, 1, GL_RGBA, GL_FLOAT, glm::value_ptr(pixel_data));

    return pixel_data;
}

void Framebuffer::clearRGBA8ColorAttachment(uint index, glm::vec4 value) {
    if(index >= color_attachment_ids.size()) {
	LOG_ERROR("Color attachment index exeeds number of color attachments\n");
	return;
    }

    glClearTexImage(color_attachment_ids[index], 0, GL_RGBA, GL_FLOAT, glm::value_ptr(value));
}

bool Framebuffer::invalidate() {
    if(id) {
	glDeleteFramebuffers(1, &id);
	glDeleteTextures(color_attachment_ids.size(), &color_attachment_ids[0]);
	glDeleteRenderbuffers(1, &depth_attachment_id);

	color_attachment_ids.clear();
	depth_attachment_id = 0;
    }

    bool multisampled = data.samples > 1;
    GLenum texture_target = multisampled ? GL_TEXTURE_2D_MULTISAMPLE : GL_TEXTURE_2D;
	
    std::vector<GLenum> color_attachment_formats = getColorAttachmentFormats();
    GLenum depth_attachment_format = getRenderbufferAttachmentFormat();
	
    glCreateFramebuffers(1, &id);
    glBindFramebuffer(GL_FRAMEBUFFER, id);
	
    if(color_attachment_formats.size()) {
	color_attachment_ids.resize(color_attachment_formats.size());
	glCreateTextures(texture_target, color_attachment_ids.size(), &color_attachment_ids[0]);

	for(int i = 0; i < color_attachment_ids.size(); ++i) {
	    glBindTexture(texture_target, color_attachment_ids[i]);
	    // TODO:
	    switch(color_attachment_formats[i]) {
	    case GL_RGBA8: {
		attachColorTexture(GL_RGBA8, GL_RGBA, i);
	    } break;
	    case GL_RGBA32F: {
		attachColorTexture(GL_RGBA32F, GL_RGBA, i);
	    } break;
	    case GL_R32I: {
		attachColorTexture(GL_R32I, GL_RED_INTEGER, i);
	    } break;
	    }
	}
    }

    if(depth_attachment_format != GL_NONE) {
	glCreateRenderbuffers(1, &depth_attachment_id);
	glBindRenderbuffer(GL_RENDERBUFFER, depth_attachment_id);
		
	switch(depth_attachment_format) {
	    // TODO:
	case GL_DEPTH24_STENCIL8: {
	    attachRenderbuffer(GL_DEPTH24_STENCIL8, GL_DEPTH_STENCIL_ATTACHMENT);
	} break;
	}
    }

    if(color_attachment_ids.size() > 1) {
	if(color_attachment_ids.size() > 4) {
	    LOG_ERROR("Number of frambuffer color attachments exceeded maximum of %d\n", 4);
	}
		
	GLenum buffers[4] = {GL_COLOR_ATTACHMENT0, GL_COLOR_ATTACHMENT1,
	    GL_COLOR_ATTACHMENT2, GL_COLOR_ATTACHMENT3};
	glDrawBuffers(color_attachment_ids.size(), buffers);
    } else if(color_attachment_ids.size() == 0) {
	glDrawBuffer(GL_NONE);
    }
	
    glBindFramebuffer(GL_FRAMEBUFFER, 0);

    return true;
}

void Framebuffer::attachColorTexture(GLenum internal_format, GLenum format, int index) {
    bool multisampled = data.samples > 1;
    GLenum texture_target = multisampled ? GL_TEXTURE_2D_MULTISAMPLE : GL_TEXTURE_2D;
	
    if(multisampled) {
	glTexImage2DMultisample(GL_TEXTURE_2D_MULTISAMPLE, data.samples,
				GL_RGBA8, data.width, data.height, GL_TRUE);
    } else {
	glTexImage2D(GL_TEXTURE_2D, 0, internal_format, data.width, data.height, 0, format,
		     GL_UNSIGNED_BYTE, nullptr);
					
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    }

    glFramebufferTexture2D(GL_FRAMEBUFFER,
			   GL_COLOR_ATTACHMENT0 + index,
			   texture_target, color_attachment_ids[index], 0);
}

void Framebuffer::attachRenderbuffer(GLenum format, GLenum attachment_type) {
    bool multisampled = data.samples > 1;
	
    if(multisampled) {
	glRenderbufferStorageMultisample(GL_RENDERBUFFER, data.samples,
					 format,
					 data.width, data.height);
    } else {
	glRenderbufferStorage(GL_RENDERBUFFER, format,
			      data.width, data.height);
    }

    glFramebufferRenderbuffer(GL_FRAMEBUFFER, attachment_type,
			      GL_RENDERBUFFER, depth_attachment_id);
}

std::vector<GLenum> Framebuffer::getColorAttachmentFormats() const {
    std::vector<GLenum> color_attachment_formats;
    for(int i = 0; i < data.formats.size(); ++i) {
	// TODO: 
	switch(data.formats[i]) {
	case GL_RGBA8: {
	    color_attachment_formats.push_back(GL_RGBA8);
	} break;
	case GL_RGBA32F: {
	    color_attachment_formats.push_back(GL_RGBA32F);
	} break;
	case GL_R32I: {
	    color_attachment_formats.push_back(GL_R32I);
	} break;
	}
    }

    return color_attachment_formats;
}

GLenum Framebuffer::getRenderbufferAttachmentFormat() const {
    GLenum renderbuffer_attachment_format = GL_NONE;
    for(int i = 0; i < data.formats.size(); ++i) {
	// TODO: 
	switch(data.formats[i]) {
	case GL_DEPTH24_STENCIL8: {
	    if(renderbuffer_attachment_format != GL_NONE) {
		LOG_ERROR("Number of renderbuffer attachments is greater than 1\n");
	    }
	    renderbuffer_attachment_format = GL_DEPTH24_STENCIL8;
	} break;
	}
    }

    return renderbuffer_attachment_format;
}
