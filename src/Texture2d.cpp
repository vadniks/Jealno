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

#include "Texture2d.hpp"

Texture2d::Texture2d(const glm::vec2& size, const unsigned char* data) {
    mSize = size;
    mId = 0;

    QOpenGLFunctions_3_3_Core::initializeOpenGLFunctions();

    glGenTextures(1, &mId);
    glBindTexture(GL_TEXTURE_2D, mId);
    glTexImage2D(
        GL_TEXTURE_2D,
        0,
        GL_RGBA,
        (int) size[0],
        (int) size[1],
        0,
        GL_RGBA,
        GL_UNSIGNED_BYTE,
        data
    );
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glBindTexture(GL_TEXTURE_2D, 0);
}

Texture2d::~Texture2d() {
    glDeleteTextures(1, &mId);
}

void Texture2d::bind() {
    glBindTexture(GL_TEXTURE_2D, mId);
}
