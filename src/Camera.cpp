/*
 * Jealno - an OpenGL 3D game.
 * Copyright (C) 2024 Vadim Nikolaev (https://github.com/vadniks).
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <https://www.gnu.org/licenses/>.
 */

#include "Camera.hpp"
#include <cmath>
#include <glm/ext/matrix_transform.hpp>

Camera::Camera(const glm::vec3& position, const glm::vec3& up, float yaw, float pitch) :
    mPosition(position),
    mFront(glm::vec3(0.0f, 0.0f, -1.0f)),
    mUp(),
    mRight(),
    mWorldUp(up),
    mYaw(yaw),
    mPitch(pitch)
{
    updateCameraVectors();
}

glm::mat4 Camera::viewMatrix() {
    return glm::lookAt(mPosition, mPosition + mFront, mUp);
}

void Camera::processKeyboard(Direction direction) {
    switch (direction) {
        case FORWARD:
            mPosition += mFront * mSpeed;
            break;
        case BACKWARD:
            mPosition -= mFront * mSpeed;
            break;
        case LEFT:
            mPosition -= mRight * mSpeed;
            break;
        case RIGHT:
            mPosition += mRight * mSpeed;
            break;
        case UP:
            mPosition += mUp * mSpeed;
            break;
        case DOWN:
            mPosition -= mUp * mSpeed;
            break;
    }
}

void Camera::processMouseMovement(float xOffset, float yOffset) {
    xOffset *= mSensitivity;
    yOffset *= mSensitivity;

    mYaw += xOffset;
    mPitch += yOffset;

    if (mPitch > 89.0f)
        mPitch = 89.0f;
    if (mPitch < -89.0f)
        mPitch = -89.0f;

    updateCameraVectors();
}

void Camera::processMouseScroll(float yOffset) {
    mZoom -= yOffset;
    if (mZoom < 1.0f)
        mZoom = 1.0f;
    if (mZoom > 45.0f)
        mZoom = 45.0f;
}

void Camera::updateCameraVectors() {
    glm::vec3 front;
    front.x = cosf(glm::radians(mYaw)) * cosf(glm::radians(mPitch));
    front.y = sinf(glm::radians(mPitch));
    front.z = sinf(glm::radians(mYaw)) * cosf(glm::radians(mPitch));
    mFront = glm::normalize(front);

    mRight = glm::normalize(glm::cross(mFront, mWorldUp));
    mUp = glm::normalize(glm::cross(mRight, mFront));
}

float Camera::zoom() {
    return mZoom;
}

glm::vec3 Camera::position() {
    return mPosition;
}

glm::vec3 Camera::front() {
    return mFront;
}

float Camera::yaw() {
    return mYaw;
}

float Camera::pitch() {
    return mPitch;
}
