#ifndef VERTEX_ATTRIBUTE_H
#define VERTEX_ATTRIBUTE_H

#include <cstdio>
#include <glad/glad.h>

struct VertexAttribute {

    VertexAttribute(size_t size, uint count, GLenum normalized) :
	    size(size), count(count), normalized(normalized) {}

	inline size_t getSize() const { return size; }
	inline uint getCount() const { return count; }
	inline GLenum isNormalized() const { return normalized; }
	
private:
	size_t size;
	uint count;
	GLenum normalized;
};

#endif // VERTEX_ATTRIBUTE_H
