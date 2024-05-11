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

#pragma once

#include <glm/glm.hpp>

class Camera {
public:
    enum Direction {
        FORWARD,
        BACKWARD,
        LEFT,
        RIGHT,
        UP,
        DOWN
    };
private:
    glm::vec3 mPosition, mFront, mUp, mRight, mWorldUp;
    float mYaw = -90.0f, mPitch = 0.0f;
    const float mSpeed = 0.1f, mSensitivity = 0.1f;
    float mZoom = 45.0f;
public:
    explicit Camera(
        const glm::vec3& position = glm::vec3(0.0f, 0.0f, 0.0f),
        const glm::vec3& up = glm::vec3(0.0f, 1.0f, 0.0f),
        float yaw = -90.0f,
        float pitch = 0.0f
    );
    glm::mat4 viewMatrix();
    void processKeyboard(Direction direction);
    void processMouseMovement(float xOffset, float yOffset);
    void processMouseScroll(float yOffset);
    float zoom();
    glm::vec3 position();
private:
    void updateCameraVectors();
};
