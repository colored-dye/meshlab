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

#define CAMERA_DIST (3.0f)

class MyOpenGLWidget : public QOpenGLWidget
{
    Q_OBJECT
public:
    MyOpenGLWidget(QWidget* parent=nullptr);
    ~MyOpenGLWidget();

    QLabel* getLabel() { return m_label; }
    void toggleShowAxis() { m_axisShow = !m_axisShow; }
    bool loadVertexFromFile(QString fileName);
    void toggleShowEdge() { m_showEdge = !m_showEdge; }
    void toggleShowVert() { m_showVert = !m_showVert; }
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
    QOpenGLBuffer* m_vbo;
    QOpenGLVertexArrayObject* m_vao;
    QOpenGLBuffer* m_ebo;
    QOpenGLShaderProgram* m_shader;
    // 长宽比
    float m_aspectRatio;
    // 几何属性: field of view, yaw, pitch
    float m_fov;
    // 鼠标上一帧的位置(世界坐标)
    glm::vec2 m_lastPos;
    // 摄像机位置和上方向量
    glm::vec3 cameraPos, cameraUp, cameraFront;
    // 坐标轴
    bool m_axisShow;
    QOpenGLShaderProgram* m_shader_axis[3];
    QOpenGLVertexArrayObject* m_vao_axis;
    QOpenGLBuffer* m_vbo_axis;
    GLuint m_projectionPos_axis[3];
    GLuint m_viewPos_axis[3];
    GLuint m_rotationPos_axis[3];

    // 物体
    GLuint m_projectionPos;
    GLuint m_viewPos;
    GLuint m_rotationPos;

    // 模型矩阵, model = rotation * translation * scale
//    glm::mat4 m_model, m_modelUse, m_modelSave;
//    QMatrix4x4 m_model, m_modelUse, m_modelSave;
    // 平移矩阵
//    glm::mat4 m_trans;
    QMatrix4x4 m_trans;
    // 旋转矩阵
    QMatrix4x4 m_rotation, m_rotationUse, m_rotationSave;

    glm::vec3 m_minPoint;
    glm::vec3 m_maxPoint;
    // 顶点(vertex)
    std::vector<glm::vec3> m_Vertices;
    std::vector<unsigned int> m_Indices;
    bool m_showEdge;
    bool m_showVert;
    // 面元(face)
    // 底部状态栏
    QLabel *m_label;
};
#endif // MYOPENGLWIDGET_H
