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
    float m_yaw;
    float m_pitch;
    float m_lastX, m_lastY;
    // 摄像机位置和上方向量
    glm::vec3 cameraPos, cameraUp;
    // 坐标轴
    bool m_axisShow;
    QOpenGLShaderProgram* m_shader_axis[3];
    QOpenGLVertexArrayObject* m_vao_axis;
    QOpenGLBuffer* m_vbo_axis;
    GLuint m_projection_axis[3];
    GLuint m_view_axis[3];

    // 物体
    GLuint m_projection;
    GLuint m_view;

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
