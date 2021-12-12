#include "MyOpenGLWidget.h"
#include <QCoreApplication>
#include <QtMath>

MyOpenGLWidget::MyOpenGLWidget(QWidget* parent)
  : QOpenGLWidget(parent)
  , camera(this)
  , m_scaleRatio(1.0f)
{
    m_label = new QLabel("Hello, Meshware5994!", this);
    // 上一帧的鼠标在世界坐标中的位置
    m_lastPos = glm::vec2(0, 0);
    // 显示坐标轴
    m_showAxis = true;
    // 显示顶点
    m_showVert = true;
    // 显示网格边
    m_showEdge = true;
    // 显示面片
    m_showFace = true;

    m_meshes = nullptr;
//    this->grabKeyboard();
}

MyOpenGLWidget::~MyOpenGLWidget()
{
    delete m_shaderVert;
    delete m_shaderEdge;
    delete m_shaderFace;
    delete m_label;
    for(int i=0; i<3; i++)
        delete m_shader_axis[i];
//    for(auto i : m_meshes)
//        delete i;
    delete m_meshes;
}

void MyOpenGLWidget::initializeGL()
{
    this->initializeOpenGLFunctions();
    // 获取上下文的OpenGL函数
    m_functions = this->context()->functions();
    // 开启深度检测
    m_functions->glEnable(GL_DEPTH_TEST);
    glDepthFunc(GL_LEQUAL);
    // 顶点大小可调整
    m_functions->glEnable(GL_VERTEX_PROGRAM_POINT_SIZE);
    glMatrixMode(GL_MODELVIEW);

    // 准备坐标轴
    initAxis();
}

void MyOpenGLWidget::paintGL()
{
    m_functions->glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    m_functions->glClearColor(0.0f, 0.0f, 0.0f, 1.0f); // 背景颜色

    // 绘制坐标轴
    drawAxis();

    if(!m_meshes)
        return;

    // 绘制顶点
    if(m_showVert){
        m_meshes->shader_vertex->bind();
        m_meshes->shader_vertex->setMat4("view", camera.getViewMatrix());
        // 投影
        m_meshes->shader_vertex->setMat4("projection", camera.getProjectionMatrix());
        // 变换矩阵
        m_meshes->shader_vertex->setMat4("model", m_scale * m_move * m_rotation * m_trans);

        if(m_functions->glGetError() != GL_NO_ERROR){
            QMessageBox::warning(this, "Warning", "Show vertex");
        }

        m_meshes->DrawVertex();
    }

    // 绘制线框
    if(m_showEdge){
        m_meshes->shader_edge->bind();
        m_meshes->shader_edge->setMat4("view", camera.getViewMatrix());
        // 投影
        m_meshes->shader_edge->setMat4("projection", camera.getProjectionMatrix());
        // 变换矩阵
        m_meshes->shader_edge->setMat4("model", m_scale * m_move * m_rotation * m_trans);
        if(m_functions->glGetError() != GL_NO_ERROR){
            QMessageBox::warning(this, "Warning", "Show Edge");
        }
        m_meshes->DrawEdge();
    }

    // 绘制面片
   if(m_showFace){
       m_meshes->shader_face->bind();
       m_meshes->shader_face->setMat4("view", camera.getViewMatrix());
       m_meshes->shader_face->setMat4("projection", camera.getProjectionMatrix());
       m_meshes->shader_face->setMat4("model", m_scale * m_move * m_rotation * m_trans);
       if(m_functions->glGetError() != GL_NO_ERROR){
           QMessageBox::warning(this, "Warning", "Show Face");
       }
       m_meshes->DrawFace();
   }
}

void MyOpenGLWidget::resizeGL(int w, int h)
{
    glViewport(0, 0, w, h);
    camera.setAspectRatio(1.0f * this->width() / this->height());
    qDebug() << "Width: " << this->width() << ", Height: " << this->height() << endl;
}

void MyOpenGLWidget::wheelEvent(QWheelEvent *event)
{
    if(event->delta() < 0){
        // 放大
        if(m_scaleRatio < 0.00011f)
            m_scaleRatio = 0.001f;
        else if(m_scaleRatio < 0.05f){
            m_scaleRatio += 0.005f;
        }else if(m_scaleRatio < 0.1f){
            m_scaleRatio += 0.05f;
        }else if(m_scaleRatio < 1.0f){
            m_scaleRatio += 0.1f;
        }else if(m_scaleRatio < 2.0f){
            m_scaleRatio += 0.3f;
        }else if(m_scaleRatio < 5.0f){
            m_scaleRatio += 0.5f;
        }else if(m_scaleRatio < 100.0f){
            m_scaleRatio += 1.5f;
        }else{
            m_scaleRatio += 5.0f;
        }
        m_scale.setToIdentity();
        m_scale.scale(m_scaleRatio);
    }else{
        // 缩小
        if(m_scaleRatio < 0.0011f)
            m_scaleRatio = 0.001f;
        else if(m_scaleRatio < 0.05f){
            m_scaleRatio -= 0.005f;
        }else if(m_scaleRatio < 0.1f){
            m_scaleRatio -= 0.05f;
        }else if(m_scaleRatio < 1.0f){
            m_scaleRatio -= 0.1f;
        }else if(m_scaleRatio < 2.0f){
            m_scaleRatio -= 0.3f;
        }else if(m_scaleRatio < 5.0f){
            m_scaleRatio -= 0.5f;
        }else if(m_scaleRatio < 100.0f){
            m_scaleRatio -= 1.5f;
        }else{
            m_scaleRatio -= 5.0f;
        }
        if(m_scaleRatio < 0)
            m_scaleRatio = 0.001f;

        m_scale.setToIdentity();
        m_scale.scale(m_scaleRatio);
    }

    m_label->setText("Zoom ratio: " + QString::number(m_scaleRatio));
    this->update();
}

void MyOpenGLWidget::mousePressEvent(QMouseEvent *event)
{
    if(event->buttons() & Qt::LeftButton || event->buttons() & Qt::MiddleButton){
        QPoint pos = event->pos();
        m_lastPos = transPoint(glm::vec2(pos.x(), pos.y()));
        m_rotationUse = m_rotationSave;
        m_moveUse = m_moveSave;
    }
}

void MyOpenGLWidget::mouseMoveEvent(QMouseEvent *event)
{
    QPoint pos = event->pos();
    glm::vec2 ppos(pos.x(), pos.y());
    ppos = transPoint(ppos);
    glm::vec2 sub = ppos - m_lastPos;

    // 旋转: 鼠标左键按住并拖动
    if(event->buttons() & Qt::LeftButton){
        float angle = qSqrt(sub.x * sub.x + sub.y * sub.y);

        m_rotation.setToIdentity();
        m_rotation.rotate(angle, sub.y, sub.x, 0);
        m_rotation *= m_rotationUse;
        m_rotationSave = m_rotation;
    }

    // 平移: 鼠标中键按住并拖动
    if(event->buttons() & Qt::MiddleButton){
        QMatrix4x4 trans;
        trans.translate(sub.x / 500 / m_scaleRatio, - sub.y / 500 / m_scaleRatio);
        m_move = trans * m_moveUse;
        m_moveSave = m_move;
    }

    this->update();
}

void MyOpenGLWidget::initAxis()
{
    const char* axis_frag_shader[3] = {
        "#version 330\nout vec4 fragColor;\nvoid main()\n{\nfragColor = vec4(1.0f, 0.0f, 0.0f, 1.0f);}\n",
        "#version 330\nout vec4 fragColor;\nvoid main()\n{\nfragColor = vec4(0.0f, 1.0f, 0.0f, 1.0f);}\n",
        "#version 330\nout vec4 fragColor;\nvoid main()\n{\nfragColor = vec4(0.0f, 0.0f, 1.0f, 1.0f);}\n"
    };
    const char* axis_vert_shader =
            "#version 330\n \
             uniform mat4 view;\n \
             uniform mat4 projection;\n \
             uniform mat4 rotation;\n \
             uniform mat4 translation;\n \
             uniform mat4 scale;\n \
             layout(location = 0) in vec3 posVertex;\n \
             void main()\n \
             {\n \
                gl_Position = projection * view * scale * rotation * translation * vec4(posVertex, 1.0f);\n \
             }\n";

//    const GLfloat MAX_AXIS = 100.0f;
//    const GLfloat AXIS[] = {
//        0, 0, 0,
//        MAX_AXIS, 0, 0,
//        0, MAX_AXIS, 0,
//        0, 0, MAX_AXIS,
//    };

    for(int i=0; i<3; i++){
        m_shader_axis[i] = new Shader(axis_vert_shader, axis_frag_shader[i], this);
        if (m_shader_axis[i]->link()) {
            qDebug("Shaders link success.");
        }
        else {
            qDebug("Shaders link failed!");
        }
//        m_mesh_axis[i] = new Mesh(this);
//        m_mesh_axis[i]->pushVertex(0, 0, 0);
//        m_mesh_axis[i]->pushVertex(AXIS[3*i+3], AXIS[3*i+4], AXIS[3*i+5]);
//        m_mesh_axis[i]->setupMesh();
    }
}

void MyOpenGLWidget::drawAxis()
{
    if(m_showAxis){
        const GLfloat MAX_AXIS = 100.0f;
        const GLfloat AXIS[] = {
            0, 0, 0,
            MAX_AXIS, 0, 0,
            0, MAX_AXIS, 0,
            0, 0, MAX_AXIS,
        };

        for(int i=0; i<3; i++){
            m_shader_axis[i]->bind();
            // 摄像机
            m_shader_axis[i]->setMat4("view", camera.getViewMatrix());
            // 投影
            m_shader_axis[i]->setMat4("projection", camera.getProjectionMatrix());
            // 变换矩阵
            m_shader_axis[i]->setMat4("rotation", m_rotation);
            m_shader_axis[i]->setMat4("translation", m_trans_axis);
            m_shader_axis[i]->setMat4("scale", m_scale);

            glBegin(GL_LINES);
            glVertex3f(AXIS[0], AXIS[1], AXIS[2]);
            glVertex3f(AXIS[3+3*i], AXIS[3+3*i+1], AXIS[3+3*i+2]);
            glEnd();

            m_shader_axis[i]->release();
        }
    }
}

bool MyOpenGLWidget::loadMesh(QString fileName)
{
    Mesh *mesh = new Mesh(this);

    if(!import_all_types(fileName.toStdString().c_str(), mesh)){
        m_label->setText("Failed to import mesh: " + fileName);
        return false;
    }

    mesh->mesh_file = fileName.split("/").back().toStdString();

    // 计算模型平移矩阵
    m_trans.setToIdentity();
    m_trans.translate(-mesh->centerPoint.x, -mesh->centerPoint.y, -mesh->centerPoint.z);
    m_trans_axis = m_trans;

    // 计算模型缩放矩阵
    m_scale.setToIdentity();
    m_scaleRatio = fmax(fmax((mesh->maxPoint.x - mesh->minPoint.x), \
                             (mesh->maxPoint.y - mesh->minPoint.y)), \
                        (mesh->maxPoint.z - mesh->minPoint.z)) \
            / fmin(this->width(), this->height()) * 400.0f;
    m_scale.scale(m_scaleRatio);

//    m_meshes.push_back(mesh);
//    m_meshes.back()->setupMesh();
    if(m_meshes)
        delete m_meshes;
    m_meshes = mesh;
    m_meshes->setupMesh();

    m_label->setText("Number of vertices: " + QString::number(mesh->getVerticesNum()));

    return true;
}

bool MyOpenGLWidget::exportMesh(QString file)
{
    if(!export_all_types(file.toStdString().c_str(), m_meshes)){
        m_label->setText("Failed to export mesh: " + file);
        return false;
    }

    m_label->setText("File \"" + file + "\" saved!");
    return true;
}
