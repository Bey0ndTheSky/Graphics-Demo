#include "Camera.h"
#include "Window.h"
#include <algorithm>

void Camera::UpdateCamera(float dt) {

    pitch -= (Window::GetMouse()->GetRelativePosition().y);
    yaw -= (Window::GetMouse()->GetRelativePosition().x);

    pitch = std::min(pitch, 90.0f);
    pitch = std::max(pitch, -90.0f);

    if (yaw < 0) {
        yaw += 360.0f;
    }
    if (yaw > 360.0f) {
        yaw -= 360.0f;
    }

    if (!locked) {
        FreeCamera(dt);
    }
    else {
        FollowPath(dt);
    }
}

void Camera::LockCamera() {
    if (!locked) {
        position = cameraPath[(currentPos + 1) % 5];
    }
    locked = !locked;
}

void Camera::FreeCamera(float dt) {
    Matrix4 rotation = Matrix4::Rotation(yaw, Vector3(0, 1, 0));

    Vector3 forward = rotation * Vector3(0, 0, -1);
    Vector3 right = rotation * Vector3(1, 0, 0);

    float speed = 5000.0f * dt;

    if (Window::GetKeyboard()->KeyDown(KEYBOARD_W)) {
        position += forward * speed;
    }
    if (Window::GetKeyboard()->KeyDown(KEYBOARD_S)) {
        position -= forward * speed;
    }
    if (Window::GetKeyboard()->KeyDown(KEYBOARD_A)) {
        position -= right * speed;
    }
    if (Window::GetKeyboard()->KeyDown(KEYBOARD_D)) {
        position += right * speed;
    }
    if (Window::GetKeyboard()->KeyDown(KEYBOARD_SHIFT)) {
        position.y += speed;
    }
    if (Window::GetKeyboard()->KeyDown(KEYBOARD_SPACE)) {
        position.y -= speed;
    }
}

void Camera::FollowPath(float dt) {
    if (!cameraPath.empty()) {
        Vector3 targetPosition = cameraPath[currentPos];

        Vector3 direction = targetPosition - position;
        float distance = direction.Length();

        if (distance > 50.0f) {
            direction.Normalise();

            float speed = 500.0f * dt;
            position += direction * speed;
        }
        else {
            currentPos = (currentPos + 1) % 5;
        }
    }
}


Matrix4 Camera::BuildViewMatrix() {
    return Matrix4::Rotation(-pitch, Vector3(1, 0, 0)) *
        Matrix4::Rotation(-yaw, Vector3(0, 1, 0)) *
        Matrix4::Translation(-position);
}

