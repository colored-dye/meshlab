#include "MyOpenGLWidget.h"
#include "mainwindow.h"
#include <QtWidgets/QApplication>


int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    MainWindow w;
    w.resize(640, 480);
    w.show();

    return a.exec();
}
