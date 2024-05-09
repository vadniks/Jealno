
#pragma once

#include "GLWidget.hpp"
#include <QMainWindow>

class MainWindow : public QMainWindow {
    Q_OBJECT
private:
    GLWidget* mGLWidget;
public:
    MainWindow();
    ~MainWindow();
};
