#include "event_receiver.h"

void emptyEvent(EditorContext *ctx) {}

void EventReceiver::clearEvents() {
    onCreate = emptyEvent;
    onDestroy = emptyEvent;

    onUpdate = emptyEvent;
    onResize = emptyEvent;

    onRenderBegin = emptyEvent;
    onRender = emptyEvent;
    onRenderPostProcessing = emptyEvent;
    onRenderEnd = emptyEvent;

    onDrawUIBegin = emptyEvent;
    onDrawUI = emptyEvent;
    onDrawUIEnd = emptyEvent;
}
