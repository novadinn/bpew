#ifndef INDEX_BUFFER_H
#define INDEX_BUFFER_H

#include "../core/platform.h"

#include <glad/glad.h>

struct IndexBuffer {
    void create(const uint* inds, uint size);
    void destroy();
	
    void bind();
    void unbind();

    inline uint getIndicesCount() const { return count; }
	
    inline uint getID() const { return id; }
	
private:
    uint id;
    uint count;
};

#endif // INDEX_BUFFER_H
