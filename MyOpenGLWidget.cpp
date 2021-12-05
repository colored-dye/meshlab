#include "MyOpenGLWidget.h"
#include <QCoreApplication>
#include <QtMath>

MyOpenGLWidget::MyOpenGLWidget(QWidget* parent)
  : QOpenGLWidget(parent)
  , m_vao(nullptr)
  , m_vbo(nullptr)
  , m_ebo(nullptr)
  , camera(this)
  , m_scaleRatio(1.0f)
{
    m_label = new QLabel("Hello, Meshware5994!", this);
    // 上一帧的鼠标在世界坐标中的位置
    m_lastPos = glm::vec2(0, 0);
    // 显示坐标轴
    m_axisShow = true;
    // 显示顶点
    m_showVert = true;
    // 显示网格边
    m_showEdge = true;

    m_minPoint = glm::vec3(1000, 1000, 1000);
    m_maxPoint = glm::vec3(-1000, -1000, -1000);

    this->grabKeyboard();
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
    m_functions->glUniformMatrix4fv(m_shader->uniformLocation("view"), 1, GL_FALSE, glm::value_ptr(camera.getViewMatrix()));

    // 投影
    m_functions->glUniformMatrix4fv(m_shader->uniformLocation("projection"), 1, GL_FALSE, glm::value_ptr(camera.getProjectionMatrix()));

    // 变换矩阵
    m_shader->setUniformValue("rotation", m_rotation);
    m_shader->setUniformValue("translation", m_trans);
    m_shader->setUniformValue("scale", m_scale);
    m_shader->setUniformValue("move", m_move);

    // 绘制顶点
    if(m_showVert)
        m_functions->glDrawArrays(GL_POINTS, 0, m_Vertices.size());

    // 绘制连线
    if(m_showEdge)
        m_functions->glDrawElements(GL_LINES, m_Indices.size(), GL_UNSIGNED_INT, 0);
    m_vao->release();
    m_shader->release();
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
        m_scaleRatio += 1.5f;
        m_scale.setToIdentity();
        m_scale.scale(m_scaleRatio);
    }else{
        // 缩小
        if(m_scaleRatio > 1.5f + 0.01f){
            m_scaleRatio -= 1.5f;
            m_scale.setToIdentity();
            m_scale.scale(m_scaleRatio);
        }
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
        trans.translate(sub.x / 1000 / m_scaleRatio, - sub.y / 1000 / m_scaleRatio);
        m_move = trans * m_moveUse;
        m_moveSave = m_move;
    }

    this->update();
}

void MyOpenGLWidget::keyPressEvent(QKeyEvent *event)
{
    QMatrix4x4 trans;

    const float step = 0.01f;
    switch(event->key()){
    case Qt::Key_A:
    case Qt::Key_Left:
        trans.translate(- step / m_scaleRatio, 0);
        break;
    case Qt::Key_D:
    case Qt::Key_Right:
        trans.translate(step / m_scaleRatio, 0);
        break;
    case Qt::Key_W:
        trans.translate(0, 0, - step / m_scaleRatio);
        break;
    case Qt::Key_S:
        trans.translate(0, 0 ,step / m_scaleRatio);
        break;
    case Qt::Key_Up:
        trans.translate(0, step / m_scaleRatio);
        break;
    case Qt::Key_Down:
        trans.translate(0, - step / m_scaleRatio);
        break;
    default:
        break;
    }

    m_trans = trans * m_trans;

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
            m_functions->glUniformMatrix4fv(m_shader_axis[i]->uniformLocation("view"), 1, GL_FALSE, glm::value_ptr(camera.getViewMatrix()));

            // 投影
            m_functions->glUniformMatrix4fv(m_shader_axis[i]->uniformLocation("projection"), 1, GL_FALSE, glm::value_ptr(camera.getProjectionMatrix()));

            // 变换矩阵
            m_shader_axis[i]->setUniformValue("rotation", m_rotation);
            m_shader_axis[i]->setUniformValue("translation", m_trans_axis);
            m_shader_axis[i]->setUniformValue("scale", m_scale);

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

    m_Vertices.clear();
    m_Indices.clear();

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
    m_trans_axis = m_trans;
    // 计算模型缩放矩阵
    m_scale.setToIdentity();
    m_scaleRatio = fmin(this->width(), this->height()) / 30.0f;
    m_scale.scale(m_scaleRatio);

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
