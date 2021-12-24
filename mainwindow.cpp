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
    // 导出模型
    connect(ui.actionSave, &QAction::triggered, this, &MainWindow::save);
    // 显示坐标轴
    connect(ui.actionShow_Axis, &QAction::toggled, this, &MainWindow::showAxis);
    // 显示线框
    connect(ui.actionShow_Edge, &QAction::toggled, this, &MainWindow::showEdge);
    // 显示顶点
    connect(ui.actionShow_Vert, &QAction::toggled, this, &MainWindow::showVert);
    // 显示面片
    connect(ui.actionShow_Face, &QAction::toggled, this, &MainWindow::showFace);
    // 稠密: Butterfly算法
    connect(ui.actionSubdivision_Butterfly, &QAction::triggered, this, &MainWindow::subdivision);
}

void MainWindow::open()
{
    QString fileName;
    QString filter = tr("ALL Files(*.*);;PLY(*.ply);;OBJ(*.obj);;XYZ(*.xyz);;TXT(*.txt);;STL(*.stl)");

    QString fileType;
    QFileDialog::Options options(QFileDialog::DontUseNativeDialog);
    fileName = QFileDialog::getOpenFileName(this, "Import mesh from file", QDir::currentPath(), filter, &fileType, options);
    if(fileName.isEmpty())
        return;

    if(!ui.centralwidget->loadMesh(fileName)){
        QMessageBox::warning(this, "Warning", "Cannot open file: " + fileName + "\nFile occupied or format unsupported");
        return;
    }

    if(fileName.length() > 20){
        fileName = fileName.split('/').back();
    }
    setWindowTitle(fileName);
    ui.centralwidget->update();
}

void MainWindow::save()
{
    QString fileName;
    QString filter = tr("ALL Files(*.*);;PLY(*.ply);;OBJ(*.obj);;XYZ(*.xyz);;TXT(*.txt);;STL(*.stl)");

    QString fileType;
    QFileDialog::Options options(QFileDialog::DontUseNativeDialog);
    fileName = QFileDialog::getSaveFileName(this, "Export mesh to file", QDir::currentPath(), filter, &fileType, options);
    if(fileName.isEmpty())
        return;

    QFileInfo fileInfo(fileName);
    if(fileInfo.suffix().isEmpty()){
        fileName += '.' +  fileType.split(".").back().left(3);
    }

    if(!ui.centralwidget->exportMesh(fileName)){
        QMessageBox::warning(this, "Warning", "Cannot save file: " + fileName + "\nFile occupied or format unsupported");
    }

    setWindowTitle("Meshware5994");
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

void MainWindow::showFace()
{
    ui.centralwidget->toggleShowFace();
    ui.centralwidget->update();
}

void MainWindow::subdivision()
{
    ui.centralwidget->butterflySubdivision();
    ui.centralwidget->update();
}
