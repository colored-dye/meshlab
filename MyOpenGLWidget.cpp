#include "MyOpenGLWidget.h"
#include <QCoreApplication>
#include <QtMath>

MyOpenGLWidget::MyOpenGLWidget(QWidget* parent)
  : QOpenGLWidget(parent)
  , cameraFront(0.0f)
  , m_vao(nullptr)
  , m_vbo(nullptr)
  , m_ebo(nullptr)
{
    m_label = new QLabel("Hello, mesh!", this);
    // 计算长宽比
    m_aspectRatio = this->width() / this->height();
    // 视野,默认为45.0
    m_fov = 45.0f;
    // 上一帧的鼠标在世界坐标中的位置
    m_lastPos = glm::vec2(0, 0);
    // 相机位置和方向
    cameraPos = glm::vec3(0.0f, 0.0f, CAMERA_DIST);
    cameraUp = glm::vec3(0.0f, 1.0f, 0.0f);
    // 显示坐标轴
    m_axisShow = true;
    // 显示顶点
    m_showVert = true;
    // 显示网格边
    m_showEdge = true;

    m_minPoint = glm::vec3(1000, 1000, 1000);
    m_maxPoint = glm::vec3(-1000, -1000, -1000);
}

MyOpenGLWidget::~MyOpenGLWidget()
{
    delete m_shader;
    delete m_vao;
    delete m_vbo;
    delete m_ebo;
    delete m_label;
    for(int i=0; i<3; i++)
        delete m_shader_axis[i];
}

void MyOpenGLWidget::initializeGL()
{
    // 获取上下文的OpenGL函数
    m_functions = this->context()->functions();
    // 开启深度检测
    m_functions->glEnable(GL_DEPTH_TEST);
    // 顶点大小可调整
    m_functions->glEnable(GL_VERTEX_PROGRAM_POINT_SIZE);

    // 准备坐标轴
    initAxis();

    // 加载vertex shader和fragment shader
    m_shader = new QOpenGLShaderProgram();
    QString path = QCoreApplication::applicationDirPath();
    m_shader->addShaderFromSourceFile(QOpenGLShader::Vertex, path + "/shaders/vert.txt");
    m_shader->addShaderFromSourceFile(QOpenGLShader::Fragment, path + "/shaders/frag.txt");
    if (m_shader->link()) {
        qDebug("Shaders link success.");
    }
    else {
        qDebug("Shaders link failed!");
    }

    if(!loadVertexFromFile(path + "/bunny.ply")){
        QMessageBox::warning(this, "Warning", "Cannot open file: " + path + "/test.txt");
    }

    // 获取视角(view)和投影(projection)在vertex shader中的位置
    m_viewPos = m_shader->uniformLocation("view");
    m_projectionPos = m_shader->uniformLocation("projection");
    m_rotationPos = m_shader->uniformLocation("rotation");
}

void MyOpenGLWidget::paintGL()
{
    m_functions->glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    m_functions->glClearColor(0.0f, 0.0f, 0.0f, 1.0f); // 背景颜色

    // 绘制坐标轴
    if(m_axisShow)
        drawAxis();

    if(m_Vertices.size() == 0)
        return;
    // 绘制顶点
    m_shader->bind();
    m_vao->bind();

    // 摄像机绕着场景旋转
    glm::mat4 view = glm::lookAt(cameraPos, cameraFront, cameraUp);
    m_functions->glUniformMatrix4fv(m_viewPos, 1, GL_FALSE, &view[0][0]);

    // 投影
    glm::mat4 projection = glm::perspective(glm::radians(m_fov), m_aspectRatio, 0.1f, 100.0f);
    m_functions->glUniformMatrix4fv(m_projectionPos, 1, GL_FALSE, &projection[0][0]);

//    m_functions->glUniformMatrix4fv(m_modelPos, 1, GL_FALSE, glm::value_ptr(m_model));
    m_shader->setUniformValue(m_rotationPos, m_rotation);
    m_shader->setUniformValue("translation", m_trans);
    QMatrix4x4 scale;
    scale.scale(10);
    m_shader->setUniformValue("scale", scale);

    if(m_showVert)
        m_functions->glDrawArrays(GL_POINTS, 0, m_Vertices.size());

    if(m_showEdge)
        m_functions->glDrawElements(GL_LINES, m_Indices.size(), GL_UNSIGNED_INT, 0);
    m_vao->release();
    m_shader->release();
}

void MyOpenGLWidget::resizeGL(int w, int h)
{
    glViewport(0, 0, w, h);
    m_aspectRatio = this->width() / this->height();
    qDebug() << "Width: " << this->width() << ", Height: " << this->height() << endl;
}

void MyOpenGLWidget::wheelEvent(QWheelEvent *event)
{
    if(event->delta() < 0){
        // 放大
        if(m_fov > 1)
            m_fov--;
    }else{
        // 缩小
        if(m_fov < 100)
            m_fov++;
    }
    qDebug() << "field: " << m_fov << endl;
    m_label->setText("Zoom ratio: " + QString::number(m_fov));
    this->update();
}

void MyOpenGLWidget::mousePressEvent(QMouseEvent *event)
{
    if(event->button() == Qt::LeftButton){
        QPoint pos = event->pos();
        m_lastPos = transPoint(glm::vec2(pos.x(), pos.y()));
        m_rotationUse = m_rotationSave;
    }
}

void MyOpenGLWidget::mouseMoveEvent(QMouseEvent *event)
{
    QPoint pos = event->pos();
    glm::vec2 ppos(pos.x(), pos.y());
    ppos = transPoint(ppos);
    glm::vec2 sub = ppos - m_lastPos;

    float angle = qSqrt(sub.x * sub.x + sub.y * sub.y);
    m_rotation.setToIdentity();
    m_rotation.rotate(angle, sub.y, sub.x, 0);
    m_rotation *= m_rotationUse;
    m_rotationSave = m_rotation;

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
    for(int i=0; i<3; i++){
        m_shader_axis[i] = new QOpenGLShaderProgram();
        m_shader_axis[i]->addShaderFromSourceCode(QOpenGLShader::Fragment, axis_frag_shader[i]);
        m_shader_axis[i]->addShaderFromSourceCode(QOpenGLShader::Vertex, axis_vert_shader);
        if (m_shader_axis[i]->link()) {
            qDebug("Shaders link success.");
        }
        else {
            qDebug("Shaders link failed!");
        }
        m_viewPos_axis[i] = m_shader_axis[i]->uniformLocation("view");
        m_projectionPos_axis[i] = m_shader_axis[i]->uniformLocation("projection");
        m_rotationPos_axis[i] = m_shader_axis[i]->uniformLocation("rotation");
    }

    const GLfloat MAX_AXIS = 100.0f;
    const GLfloat AXIS[] = {
        0, 0, 0,
        MAX_AXIS, 0, 0,
        0, 0, 0,
        0, MAX_AXIS, 0,
        0, 0, 0,
        0, 0, MAX_AXIS,
    };

    m_vao_axis = new QOpenGLVertexArrayObject();
    m_vao_axis->create();
    m_vao_axis->bind();
    m_vbo_axis = new QOpenGLBuffer(QOpenGLBuffer::Type::VertexBuffer);
    m_vbo_axis->create();
    m_vbo_axis->bind();

    m_vbo_axis->allocate(AXIS, sizeof(AXIS));
    m_functions->glEnableVertexAttribArray(0);
    m_functions->glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(GLfloat), 0);

    m_vbo_axis->release();
    m_vao_axis->release();
}

void MyOpenGLWidget::drawAxis()
{
    if(m_axisShow){
        m_vao_axis->bind();

        for(int i=0; i<3; i++){
            m_shader_axis[i]->bind();
            // 摄像机
            glm::mat4 view = glm::lookAt(cameraPos, cameraFront, cameraUp);
            m_functions->glUniformMatrix4fv(m_viewPos_axis[i], 1, GL_FALSE, &view[0][0]);

            // 投影
            glm::mat4 projection = glm::perspective(glm::radians(m_fov), m_aspectRatio, 0.1f, 100.0f);
            m_functions->glUniformMatrix4fv(m_projectionPos_axis[i], 1, GL_FALSE, &projection[0][0]);

            // 模型矩阵
            m_shader_axis[i]->setUniformValue("rotation", m_rotation);
            m_shader_axis[i]->setUniformValue("translation", m_trans);
            QMatrix4x4 scale;
            scale.scale(10);
            m_shader_axis[i]->setUniformValue("scale", scale);

            m_functions->glDrawArrays(GL_LINES, 2*i, 2);
            m_shader_axis[i]->release();
        }

        m_vao_axis->release();
    }
}

bool MyOpenGLWidget::loadVertexFromFile(QString fileName)
{
    QFile file(fileName);
    if (!file.open(QIODevice::ReadOnly | QFile::Text)) {
        QMessageBox::warning(this, "Warning", "Cannot open file: " + file.errorString());
        return false;
    }

    char buf[128];
    glm::vec3 v;

    bool isPLY = false;
    while(file.readLine(buf, sizeof(buf)) != -1){
        // PLY格式文件需要特殊处理
        // 如果不是PLY文件,默认认为只有点的坐标
        if(strstr(buf, "ply")){
            isPLY = true;
            break;
        }
        sscanf_s(buf, "%f %f %f", &v.x, &v.y, &v.z);
        m_Vertices.push_back(v);
    }
    // 解析PLY文件
    if(isPLY){
        while((file.readLine(buf, sizeof(buf)) != -1) && !strstr(buf, "element vertex"))
            ;
        unsigned num_vertex, num_face;
        num_vertex = num_face = 0;
        if(file.atEnd()){
            QMessageBox::warning(this, "Warning", "Invaid PLY format! File: " + file.errorString());
            return false;
        }
        sscanf_s(buf, "element vertex %lu", &num_vertex);
        while((file.readLine(buf, sizeof(buf)) != -1) && !strstr(buf, "element face"))
            ;
        if(file.atEnd()){
            QMessageBox::warning(this, "Warning", "Invaid PLY format! File: " + file.errorString());
            return false;
        }
        sscanf_s(buf, "element face %lu", &num_face);
        while((file.readLine(buf, sizeof(buf)) != -1) && !strstr(buf, "end_header"))
            ;
        for(unsigned int i=0; i<num_vertex; i++){
            if(file.readLine(buf, sizeof(buf)) != -1){
                sscanf_s(buf, "%f %f %f", &v.x, &v.y, &v.z);
                m_Vertices.push_back(v);
                m_minPoint.x = fmin(m_minPoint.x, v.x);
                m_minPoint.y = fmin(m_minPoint.y, v.y);
                m_minPoint.z = fmin(m_minPoint.z, v.z);
                m_maxPoint.x = fmax(m_maxPoint.x, v.x);
                m_maxPoint.y = fmax(m_maxPoint.y, v.y);
                m_maxPoint.z = fmax(m_maxPoint.z, v.z);
            }else{
                QMessageBox::warning(this, "Warning", "Vertex number does not fit! File: " + file.errorString());
                return false;
            }
        }
        for(unsigned int i=0; i<num_face; i++){
            if(file.readLine(buf, sizeof(buf)) != -1){
                unsigned int facen; // face的顶点数,一般为3
                std::stringstream ss(buf);
                ss >> facen;
                unsigned int vert0, vert1;
                ss >> vert0;
                m_Indices.push_back(vert0);
                for(unsigned int i=1; i<facen; i++){
                    ss >> vert1;
                    m_Indices.push_back(vert1);
                    m_Indices.push_back(vert1);
                }
                m_Indices.push_back(vert0);
            }else{
                QMessageBox::warning(this, "Warning", "Face number does not fit! File: " + file.errorString());
                return false;
            }
        }
    }
    file.close();

    // 计算模型平移矩阵
    m_trans.setToIdentity();
    m_trans.translate(-(m_maxPoint.x + m_minPoint.x) / 2, -(m_maxPoint.y + m_minPoint.y) / 2, -(m_maxPoint.z + m_minPoint.z) / 2);

    if(m_vao == nullptr)
        m_vao = new QOpenGLVertexArrayObject();
    m_vao->create();
    m_vao->bind();
    if(m_vbo == nullptr)
        m_vbo = new QOpenGLBuffer(QOpenGLBuffer::Type::VertexBuffer);
    m_vbo->create();
    m_vbo->bind();
    m_ebo = new QOpenGLBuffer(QOpenGLBuffer::Type::IndexBuffer);
    m_ebo->create();
    m_ebo->bind();

    m_vbo->allocate(&m_Vertices[0], sizeof(decltype(m_Vertices[0])) * m_Vertices.size());
    m_ebo->allocate(&m_Indices[0], sizeof(decltype(m_Indices[0])) * m_Indices.size());

    m_functions->glEnableVertexAttribArray(0);
    m_functions->glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(GLfloat), 0);

//    m_ebo->release();
    m_vbo->release();
    m_vao->release();

    m_label->setText("Number of vertices: " + QString::number(m_Vertices.size()));

    this->update();

    return true;
}

glm::vec2 MyOpenGLWidget::transPoint(glm::vec2 p)
{
    return (p - (getWindowCenter()));
}
