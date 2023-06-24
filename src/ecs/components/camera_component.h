#ifndef CAMERA_COMPONENT_H
#define CAMERA_COMPONENT_H

#include "../../graphics/camera.h"

struct CameraComponent {
    Camera camera;
    bool main = false;
};

#endif // CAMERA_COMPONENT_H
