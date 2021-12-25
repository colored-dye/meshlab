#ifndef SHADER_H
#define SHADER_H

#include <QOpenGLExtraFunctions>
#include <QOpenGLShaderProgram>
#include <QOpenGLWidget>
#include <QLoggingCategory>

#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>

class Shader: public QOpenGLShaderProgram
{
//    Q_OBJECT
private:
    QOpenGLFunctions *func;
//    QOpenGLShaderProgram *m_shader;
public:
    Shader(QString vertexPath, QString fragmentPath, QOpenGLWidget *w)
        : QOpenGLShaderProgram(w)
    {
        func = w->context()->functions();
//        m_shader = new QOpenGLShaderProgram(w);
        addShaderFromSourceFile(QOpenGLShader::Vertex, vertexPath);
        addShaderFromSourceFile(QOpenGLShader::Fragment, fragmentPath);
    }
    Shader(const char *vertexProgram, const char *fragmentProgram, QOpenGLWidget *w)
        : QOpenGLShaderProgram(w)
    {
        func = w->context()->functions();
//        m_shader = new QOpenGLShaderProgram(w);
        addShaderFromSourceCode(QOpenGLShader::Vertex, vertexProgram);
        addShaderFromSourceCode(QOpenGLShader::Fragment, fragmentProgram);
    }
    ~Shader()
    {
    }
    bool link()
    {
        return QOpenGLShaderProgram::link();
    }
    void setMat4(const char *name, QMatrix4x4 mat)
    {
        setUniformValue(name, mat);
    }
    void setMat4(const char *name, glm::mat4 mat)
    {
        func->glUniformMatrix4fv(uniformLocation(name), 1, GL_FALSE, glm::value_ptr(mat));
    }
    void setVec3(const char *name, QVector3D vec)
    {
        setUniformValue(name, vec);
    }
    void setVec3(const char *name, glm::vec3 vec)
    {
        func->glUniform3fv(uniformLocation(name), 1, glm::value_ptr(vec));
    }
    void setVec3(const char *name, float x, float y, float z)
    {
        setUniformValue(name, QVector3D(x, y, z));
    }
    void setFloat(const char *name, float f)
    {
        setUniformValue(name, f);
    }
};

#endif // SHADER_H
