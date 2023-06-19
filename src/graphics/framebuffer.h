#ifndef FRAMEBUFFER_H
#define FRAMEBUFFER_H

#include "../core/platform.h"

#include <glm/glm.hpp>
#include <glad/glad.h>
#include <vector>

struct FramebufferData {
    std::vector<GLenum> formats;
    uint width, height;
    uint samples = 1;
};

struct Framebuffer {
    bool create(FramebufferData data);
    void destroy();

    void bind();
    void unbind();

    void bindReadAttachment(uint index);
    void bindDrawAttachment(uint index);

    void blitTo(Framebuffer other);
    void blitFrom(Framebuffer other);
	
    bool resize(uint width, uint height);
    
    int readR32IPixel(int x, int y);
    void clearR32IColorAttachment(uint index, int value);
    glm::vec4 readRGBA8Pixel(int x, int y);
    void clearRGBA8ColorAttachment(uint index, glm::vec4 value);

    inline FramebufferData getFramebufferData() const { return data; }
    inline uint getID() const { return id; }
    inline uint getColorAttachmentID(uint index) const { return color_attachment_ids[index]; }
    inline uint getDepthAttachmentID() const { return depth_attachment_id; }
	
private:
    bool invalidate();

    void attachColorTexture(GLenum internal_format, GLenum format, int index);
    void attachRenderbuffer(GLenum format, GLenum attachment_type);

    std::vector<GLenum> getColorAttachmentFormats() const;
    GLenum getRenderbufferAttachmentFormat() const;
	
    uint id;
    FramebufferData data;
    std::vector<uint> color_attachment_ids;
    uint depth_attachment_id = GL_NONE;
};

#endif // FRAMEBUFFER_H
