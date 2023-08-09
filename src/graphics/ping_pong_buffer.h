#ifndef PING_PONG_BUFFER_H
#define PING_PONG_BUFFER_H

#include "framebuffer.h"

struct PingPongBuffer {

  void create(FramebufferData data1, FramebufferData data2) {
    first = new Framebuffer();
    second = new Framebuffer();

    first->create(data1);
    second->create(data2);

    current = first;
    previous = second;
  }

  void destroy() {
    first->destroy();
    second->destroy();

    delete first;
    delete second;

    current = nullptr;
    previous = nullptr;
  }

  void resize(uint width, uint height) {
    first->resize(width, height);
    second->resize(width, height);
  }

  void swap() {
    if (current->getID() == first->getID()) {
      current = second;
      previous = first;
    } else {
      current = first;
      previous = second;
    }
  }

  Framebuffer *current;
  Framebuffer *previous;

private:
  /* pointers for safety in case of copying */

  Framebuffer *first;
  Framebuffer *second;
};

#endif // PING_PONG_BUFFER_H