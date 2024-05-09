
#include "MainWindow.hpp"

MainWindow::MainWindow() : mGLWidget(new GLWidget(this)) {
    setCentralWidget(mGLWidget);
}

MainWindow::~MainWindow() {
    delete mGLWidget;
}
