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

#include "GLWidget.hpp"

GLWidget::GLWidget(QWidget* parent) : QOpenGLWidget(parent) {
    setFormat(QSurfaceFormat::defaultFormat());
}

GLWidget::~GLWidget() {

}

QSize GLWidget::minimumSizeHint() const {
    return {800, 450};
}

QSize GLWidget::sizeHint() const {
    return minimumSizeHint();
}

void GLWidget::initializeGL() {
    initializeOpenGLFunctions();

    glEnable(GL_MULTISAMPLE);
    glEnable(GL_BLEND);
    glEnable(GL_DEPTH_TEST);

    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
}

void GLWidget::paintGL() {
    glClearColor(1.0f, 1.0f, 1.0f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT);
}

void GLWidget::resizeGL(int w, int h) {
    QOpenGLWidget::resizeGL(w, h);
}
