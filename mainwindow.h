#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QWidget>
#include <QMainWindow>
#include "MyOpenGLWidget.h"
#include "./ui_mainwindow.h"

class MainWindow : public QMainWindow {
    Q_OBJECT
public:
    MainWindow(QMainWindow *parent = nullptr);
    ~MainWindow() {}

protected slots:
    void open();
    void save();
    void showAxis();
    void showEdge();
    void showVert();
private:
    Ui::MainWindow ui;
    QLabel *m_label;
    QString m_currentFile;
};

#endif
