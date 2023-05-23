#include "framebuffer.h"

#include "../core/log.h"

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
	
	std::vector<GLenum> color_attachment_formats;
	GLenum depth_attachment_format;
	for(int i = 0; i < data.formats.size(); ++i) {
		// TODO: 
		switch(data.formats[i]) {
		case GL_RGBA8: {
			color_attachment_formats.push_back(GL_RGBA8);
		} break;
		case GL_DEPTH24_STENCIL8: {
			depth_attachment_format = GL_DEPTH24_STENCIL8;
		} break;
		}
	}
	
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
				if(multisampled) {
					glTexImage2DMultisample(GL_TEXTURE_2D_MULTISAMPLE, data.samples,
											GL_RGBA8, data.width, data.height, GL_TRUE);
				} else {
					glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, data.width, data.height, 0, GL_RGBA,
								 GL_UNSIGNED_BYTE, nullptr);
					
					glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
					glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
					glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
					glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
					glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
				}

				glFramebufferTexture2D(GL_FRAMEBUFFER,
									   GL_COLOR_ATTACHMENT0 + i,
									   texture_target, color_attachment_ids[i], 0);
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
			if(multisampled) {
				glRenderbufferStorageMultisample(GL_RENDERBUFFER, data.samples,
												 GL_DEPTH24_STENCIL8,
												 data.width, data.height);
			} else {
				glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH24_STENCIL8,
									  data.width, data.height);
			}

			glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT,
									  GL_RENDERBUFFER, depth_attachment_id);
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
