#ifndef FRAMEBUFFER_H
#define FRAMEBUFFER_H

#include "../core/platform.h"

struct FrambufferData {
	uint width, height;
};

struct Framebuffer {
	bool create(FrambufferData data);
	void destroy();

	void bind();
	void unbind();

	bool resize(uint width, uint height);

	inline FrambufferData getData() const { return data; }
	inline uint getID() const { return id; }
	inline uint getColorAttachmentID() const { return color_attachment_id; }
	inline uint getDepthAttachmentID() const { return depth_attachment_id; }
	
private:
	bool invalidate();

	FrambufferData data;
	uint id, color_attachment_id, depth_attachment_id;
};

#endif // FRAMEBUFFER_H
