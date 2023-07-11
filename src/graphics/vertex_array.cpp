#include "vertex_array.h"

#include <glad/glad.h>
#include <cstdio>

void VertexArray::create() {
	glCreateVertexArrays(1, &id);
}

void VertexArray::destroy() {
	glDeleteVertexArrays(1, &id);
}

void VertexArray::addVertexBuffer(VertexBuffer vb, const std::vector<VertexAttribute>& attribs) {
	if(attribs.size() == 0) {
	    printf("Empty attribute list passed for function addVertexBuffer\n");
	    return;
	}
	
	int i = 0;
	size_t total = 0;

	// Calculate the stride
	for(i = 0; i < attribs.size(); ++i) {
	    total += attribs[i].getSize() * attribs[i].getCount();
	}

	bind();
	vb.bind();
	
	size_t off = 0;

	// Set shader's attributes and calculate offset
	for(i = 0; i < attribs.size(); ++i) {
	    glVertexAttribPointer(i, attribs[i].getCount(), GL_FLOAT,
							  attribs[i].isNormalized(),
							  total, (void*)(off*attribs[i].getSize()));
	    glEnableVertexAttribArray(i);
	    off += attribs[i].getCount();
	}
}
    
void VertexArray::setIndexBuffer(IndexBuffer i) {
	bind();
	i.bind();
}

void VertexArray::bind() {
	glBindVertexArray(id);
}
    
void VertexArray::unbind() {
	glBindVertexArray(0);
}
