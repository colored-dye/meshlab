#include "MyOpenGLWidget.h"
#include <QCoreApplication>


MyOpenGLWidget::MyOpenGLWidget(QWidget* parent)
  : QOpenGLWidget(parent)
{
    m_label = new QLabel("Hello, mesh!", this);
    // 计算长宽比
    m_aspectRatio = this->width() / this->height();
    // 视野,默认为45.0
    m_fov = 45.0f;
    // 欧拉角,只需要仰角(pitch)和水平角(yaw)
    m_yaw = 0.0f;
    m_pitch = 0.0f;
    // 上一帧的鼠标位置
    m_lastX = m_lastY = 0.0f;
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
    m_view = m_shader->uniformLocation("view");
    m_projection = m_shader->uniformLocation("projection");
}

void MyOpenGLWidget::paintGL()
{
    m_functions->glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    m_functions->glClearColor(0.0f, 0.0f, 0.0f, 1.0f); // 背景颜色

    // 绘制坐标轴
    if(m_axisShow)
        drawAxis();

    // 绘制顶点
    m_vao->bind();
    m_shader->bind();

    // 摄像机绕着场景旋转
    glm::mat4 view = glm::lookAt(cameraPos, glm::vec3(0, 0, 0), cameraUp);
    m_functions->glUniformMatrix4fv(m_view, 1, GL_FALSE, &view[0][0]);

    // 投影
    glm::mat4 projection = glm::perspective(glm::radians(m_fov), m_aspectRatio, 0.1f, 100.0f);
    m_functions->glUniformMatrix4fv(m_projection, 1, GL_FALSE, &projection[0][0]);

//    glTranslatef((m_maxPoint.x + m_minPoint.x) / 2, (m_maxPoint.y + m_minPoint.y) / 2, (m_maxPoint.z + m_minPoint.z) / 2);
    QMatrix4x4 trans;
    trans.translate(-(m_maxPoint.x + m_minPoint.x) / 2, -(m_maxPoint.y + m_minPoint.y) / 2, -(m_maxPoint.z + m_minPoint.z) / 2);
    m_shader->setUniformValue("model", trans);

    if(m_showVert)
        m_functions->glDrawArrays(GL_POINTS, 0, m_Vertices.size());

    if(m_showEdge)
        m_functions->glDrawElements(GL_LINES, m_Indices.size(), GL_UNSIGNED_INT, 0);
    m_shader->release();
    m_vao->release();
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
        m_lastX = pos.x();
        m_lastY = pos.y();
    }
}

void MyOpenGLWidget::mouseMoveEvent(QMouseEvent *event)
{
    QPoint pos = event->pos();
    float xoffset = pos.x() - m_lastX;
    float yoffset = pos.y() - m_lastY;
    m_lastX = pos.x();
    m_lastY = pos.y();

    m_yaw += xoffset;
    m_pitch += yoffset;
    qDebug() << "Pitch: " << m_pitch << " Yaw: " << m_yaw << endl;

    if(m_pitch > 89.0f)
        m_pitch = 89.0f;
    if(m_pitch < -89.0f)
        m_pitch = -89.0f;

    glm::vec3 front;
    front.x = cos(glm::radians(m_yaw)) * cos(glm::radians(m_pitch));
    front.y = sin(glm::radians(m_pitch));
    front.z = sin(glm::radians(m_yaw)) * cos(glm::radians(m_pitch));

    // 更新摄像机位置,移动方向与鼠标移动方向相反,且与原点距离始终保持为CAMERA_DIST
    cameraPos += glm::normalize(front);;
    cameraPos = CAMERA_DIST * glm::normalize(cameraPos);

    qDebug() << "Camera position: (" << cameraPos.x << ", " << cameraPos.y << ", " << cameraPos.z << ")\n";
    qDebug() << "Camera direction: (" << front.x << ", " << front.y << ", " << front.z << " )\n";

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
             layout(location = 0) in vec3 posVertex;\n \
             void main()\n \
             {\n \
                gl_Position = projection * view * vec4(posVertex, 1.0f);\n \
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
        m_view_axis[i] = m_shader_axis[i]->uniformLocation("view");
        m_projection_axis[i] = m_shader_axis[i]->uniformLocation("projection");
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
            // 摄像机绕着场景旋转
            glm::mat4 view = glm::lookAt(cameraPos, glm::vec3(0, 0, 0), cameraUp);
            m_functions->glUniformMatrix4fv(m_view_axis[i], 1, GL_FALSE, &view[0][0]);

            // 投影
            glm::mat4 projection = glm::perspective(glm::radians(m_fov), m_aspectRatio, 0.1f, 100.0f);
            m_functions->glUniformMatrix4fv(m_projection_axis[i], 1, GL_FALSE, &projection[0][0]);
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

    m_vao = new QOpenGLVertexArrayObject();
    m_vao->create();
    m_vao->bind();
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

    return true;
}
