#ifndef EVENT_RECEIVER_H
#define EVENT_RECEIVER_H

#include "editor_context.h"

struct EventReceiver {
  void clearEvents();

  void (*onCreate)(EditorContext *ctx);
  void (*onDestroy)(EditorContext *ctx);

  void (*onUpdate)(EditorContext *ctx);
  void (*onResize)(EditorContext *ctx);

  void (*onRender)(EditorContext *ctx);
  // TODO: add functionaloty to submit more passes
  void (*onRenderPostProcessing)(EditorContext *ctx);

  void (*onDrawUI)(EditorContext *ctx);
};

#endif // EVENT_RECEIVER_H
