
#include "GLWidget.hpp"

GLWidget::GLWidget(QWidget* parent) {

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
