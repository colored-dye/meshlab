#ifndef SHADER_H
#define SHADER_H

#include <QOpenGLFunctions>
#include <QOpenGLShaderProgram>
#include <QLoggingCategory>

#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>

class Shader {
private:
    QOpenGLFunctions *func;
    QOpenGLShaderProgram *m_shader;
public:
    Shader(QString vertexPath, QString fragmentPath, QOpenGLFunctions *f, QObject *parent)
    {
        func = f;
        m_shader = new QOpenGLShaderProgram(parent);
        m_shader->addShaderFromSourceFile(QOpenGLShader::Vertex, vertexPath);
        m_shader->addShaderFromSourceFile(QOpenGLShader::Fragment, fragmentPath);
    }
    Shader(const char *vertexProgram, const char *fragmentProgram, QOpenGLFunctions *f, QObject *parent)
    {
        func = f;
        m_shader = new QOpenGLShaderProgram(parent);
        m_shader->addShaderFromSourceCode(QOpenGLShader::Vertex, vertexProgram);
        m_shader->addShaderFromSourceCode(QOpenGLShader::Fragment, fragmentProgram);
    }
    ~Shader()
    {
        delete m_shader;
    }
    bool bind()
    {
        return m_shader->bind();
    }
    bool link()
    {
        return m_shader->link();
    }
    void release()
    {
        m_shader->release();
    }
    void setMat4(const char *name, QMatrix4x4 &mat)
    {
        m_shader->setUniformValue(name, mat);
    }
    void setMat4(const char *name, glm::mat4 mat4)
    {
        func->glUniformMatrix4fv(m_shader->uniformLocation(name), 1, GL_FALSE, glm::value_ptr(mat4));
    }
};

#endif // SHADER_H
