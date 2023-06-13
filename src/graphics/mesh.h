#ifndef MESH_H
#define MESH_H

#include "vertex_attribute.h"
#include "vertex_array.h"
#include "../core/platform.h"

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <vector>

struct Mesh {
    void generateVertexArray() {
	va.create();
	va.bind();

	vb.create(&vertices[0], vertices.size() * sizeof(float));
	ib.create(&indices[0], indices.size() * sizeof(uint));

	va.addVertexBuffer(vb, attributes);
	va.setIndexBuffer(ib);
    }
    
    void destroy() {
	ib.destroy();
	vb.destroy();
	va.destroy();
    }

    // TODO: store a type of a vertex subdata in here (to identify position/normal..)
    std::vector<VertexAttribute> attributes;
    std::vector<float> vertices;
    std::vector<uint> indices;

    VertexArray va;
    VertexBuffer vb;
    IndexBuffer ib;
};

#endif // MESH_H
