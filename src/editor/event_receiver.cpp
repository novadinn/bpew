#include "event_receiver.h"

void emptyEvent(EditorContext *ctx) {}

void EventReceiver::clearEvents() {
  onCreate = emptyEvent;
  onDestroy = emptyEvent;

  onUpdate = emptyEvent;
  onResize = emptyEvent;

  onRender = emptyEvent;
  onRenderPostProcessing = emptyEvent;

  onDrawUI = emptyEvent;
}
