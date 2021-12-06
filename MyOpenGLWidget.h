#ifndef MYOPENGLWIDGET_H
#define MYOPENGLWIDGET_H
#include <QOpenGLWidget>
#include <QMainWindow>
#include <QOpenGLFunctions>
#include <QOpenGLBuffer>
#include <QOpenGLVertexArrayObject>
#include <QOpenGLShaderProgram>
#include <QWheelEvent>
#include <QLabel>
#include <QFileDialog>
#include <QMessageBox>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtc/quaternion.hpp>
#include <glm/gtx/quaternion.hpp>

#include "Camera.h"
#include "Shader.h"
#include "Mesh.h"


class MyOpenGLWidget : public QOpenGLWidget
{
    Q_OBJECT
public:
    MyOpenGLWidget(QWidget* parent=nullptr);
    ~MyOpenGLWidget();

    QLabel* getLabel() { return m_label; }
    void toggleShowAxis() { m_showAxis = !m_showAxis; }
    bool loadVertexFromFile(QString fileName);
    void toggleShowEdge() { m_showEdge = !m_showEdge; }
    void toggleShowVert() { m_showVert = !m_showVert; }
    void exportPLY(QFile &file);
protected:
    void initializeGL() override;
    void paintGL() override;
    void resizeGL(int w, int h) override;

    void wheelEvent(QWheelEvent* event) override;
    void mousePressEvent(QMouseEvent *event) override;
    void mouseMoveEvent(QMouseEvent *event) override;

private:
    void initAxis();
    void drawAxis();

    glm::vec2 transPoint(glm::vec2 p);
    glm::vec2 getWindowCenter() { return glm::vec2(this->width()/2.0f, this->height()/2.0f); }

private:
    QOpenGLFunctions* m_functions;
    // 模型相关: 顶点数组,顶点buffer,索引buffer,着色器
    std::vector<Mesh*> m_meshes;
    Shader *m_shader;
    // 摄像机
    Camera camera;
    // 鼠标上一帧的位置(世界坐标)
    glm::vec2 m_lastPos;
    // 坐标轴
    bool m_showAxis;
    Mesh *m_mesh_axis[3];
    Shader *m_shader_axis[3];
    QMatrix4x4 m_trans_axis;

    // 模型平移矩阵
    QMatrix4x4 m_trans;
    // 旋转矩阵
    QMatrix4x4 m_rotation, m_rotationUse, m_rotationSave;
    // 缩放矩阵
    QMatrix4x4 m_scale;
    float m_scaleRatio;
    // 鼠标平移矩阵
    QMatrix4x4 m_move, m_moveUse, m_moveSave;

    // 模型最大/最小的坐标维度构成的点
    glm::vec3 m_minPoint;
    glm::vec3 m_maxPoint;
    // 显示连线
    bool m_showEdge;
    // 显示顶点
    bool m_showVert;
    // 左下角状态栏
    QLabel *m_label;
};
#endif // MYOPENGLWIDGET_H
