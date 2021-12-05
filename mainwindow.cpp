#include "mainwindow.h"

MainWindow::MainWindow(QMainWindow *parent)
    : QMainWindow(parent)
{
    ui.setupUi(this);
    this->setWindowTitle("MeshWare5994");
    // 添加右下角固定标签
    m_label = new QLabel("BAO Yuntai, ZJU", this);
    ui.statusbar->addPermanentWidget(m_label);
    // 添加左下角的状态信息标签
    ui.statusbar->addWidget(ui.centralwidget->getLabel());

    // menu功能
    // 打开文件,导入模型
    connect(ui.actionOpen, &QAction::triggered, this, &MainWindow::open);
    // 显示坐标轴
    connect(ui.actionShow_Axis, &QAction::toggled, this, &MainWindow::showAxis);
    // 显示线框
    connect(ui.actionShow_Edge, &QAction::toggled, this, &MainWindow::showEdge);
    // 显示顶点
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
