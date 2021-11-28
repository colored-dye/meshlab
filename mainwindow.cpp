#include "mainwindow.h"

MainWindow::MainWindow(QMainWindow *parent)
    : QMainWindow(parent)
{
    ui.setupUi(this);
    m_label = new QLabel("BAO Yuntai, ZJU", this);
    ui.statusbar->addPermanentWidget(m_label);
    ui.statusbar->addWidget(ui.centralwidget->getLabel());

    // menu功能
    connect(ui.actionOpen, &QAction::triggered, this, &MainWindow::open);
    connect(ui.actionShow_Axis, &QAction::toggled, this, &MainWindow::showAxis);
    connect(ui.actionShow_Edge, &QAction::toggled, this, &MainWindow::showEdge);
    connect(ui.actionShow_Vert, &QAction::toggled, this, &MainWindow::showVert);
}

void MainWindow::open()
{
    QString fileName = QFileDialog::getOpenFileName(this, "Open the file");
    if(fileName.isEmpty())
        return;
//    QFile file(fileName);
//    m_currentFile = fileName;
//    if (!file.open(QIODevice::ReadOnly | QFile::Text)) {
//        QMessageBox::warning(this, "Warning", "Cannot open file: " + file.errorString());
//        return;
//    }
    setWindowTitle(fileName);
    ui.centralwidget->loadVertexFromFile(fileName);
}

void MainWindow::showAxis()
{
    ui.centralwidget->toggleShowAxis();
    ui.centralwidget->update();
}

void MainWindow::showEdge()
{
    ui.centralwidget->toggleShowEdge();
    ui.centralwidget->update();
}

void MainWindow::showVert()
{
    ui.centralwidget->toggleShowVert();
    ui.centralwidget->update();
}
