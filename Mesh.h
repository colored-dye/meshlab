#ifndef MESH_H
#define MESH_H

#include <QOpenGLVertexArrayObject>
#include <QOpenGLBuffer>
#include <QOpenGLWidget>
#include <QOpenGLExtraFunctions>
#include <QDir>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <vector>

#include "Shader.h"
#include <QMessageBox>

struct Vertex {
    // position
    glm::vec3 Position;
    // normal
    glm::vec3 Normal;
    // texCoords
//    glm::vec2 TexCoords;
    // tangent
//    glm::vec3 Tangent;
    // bitangent
//    glm::vec3 Bitangent;
    Vertex(float x=0, float y=0, float z=0, float nx=0, float ny=0, float nz=0)
    {
        Position = glm::vec3(x, y, z);
        Normal = glm::vec3(nx, ny, nz);
    }
};

struct Face {
    // Vertex index reference
    unsigned int V[3];
    // Face normal
//    glm::vec3 Normal;

    Face(unsigned int V0=-1, unsigned int V1=-1, unsigned int V2=-1, float x=0, float y=0, float z=0)
    {
        V[0] = V0, V[1] = V1, V[2] = V2;
//        Normal = glm::vec3(x, y, z);
    }
};

class Mesh {
public:
    // mesh Data
    std::vector<Vertex>       vertices;
    std::vector<unsigned int> indices;
    std::vector<Face>         faces;

    QOpenGLVertexArrayObject *VAO;
    QOpenGLBuffer *VBO, *EBO, *FBO;

    Shader *shader_vertex, *shader_edge, *shader_face;

    std::string mesh_file;
    glm::vec3 maxPoint;
    glm::vec3 minPoint;
    glm::vec3 centerPoint;
    const float MINMAXBOUND = 1e6f;

    // OpenGL context
    QOpenGLWidget *context;
    QOpenGLFunctions *func;

    // constructor
    Mesh(QOpenGLWidget *w)
    {
        context = w;
        func = context->context()->functions();

        context->makeCurrent();

        maxPoint = glm::vec3(-MINMAXBOUND);
        minPoint = glm::vec3(MINMAXBOUND);
        centerPoint = glm::vec3(0.0f);

        QString curDir = QDir::currentPath();
        // Vertex Shader
        shader_vertex = new Shader(curDir + "/debug/shaders/vert.txt", curDir + "/debug/shaders/fragVert.txt", context);
        if (shader_vertex->link()) {
            qDebug("Shaders link success.");
        }
        else {
            qDebug("Shaders link failed!");
        }

        // Edge Shader
        shader_edge = new Shader(curDir + "/debug/shaders/vert.txt", curDir + "/debug/shaders/fragEdge.txt", context);
        if(shader_edge->link()){
            qDebug("Shaders link success.");
        }
        else {
            qDebug("Shaders link failed!");
        }

        // Face Shader
        shader_face = new Shader(curDir + "/debug/shaders/vert.txt", curDir + "/debug/shaders/fragFace.txt", context);
        if(shader_edge->link()){
            qDebug("Shaders link success.");
        }
        else {
            qDebug("Shaders link failed!");
        }

        context->doneCurrent();
    }
    ~Mesh(){
        if(VBO) VBO->destroy(), delete VBO;
        if(EBO) EBO->destroy(), delete EBO;
        if(FBO) FBO->destroy(), delete FBO;
        if(VAO){
            context->makeCurrent();
            VAO->destroy();
            context->doneCurrent();
            delete VAO;
        }
        if(shader_vertex) delete shader_vertex;
        if(shader_edge) delete shader_edge;
        if(shader_face) delete shader_face;
    }

    decltype (vertices.size()) getVerticesNum() {
        return vertices.size();
    }

    void pushVertex(Vertex v){
        vertices.push_back(v);
    }

    void pushVertex(float x, float y, float z){
        vertices.push_back(Vertex(x, y, z));
    }

    // initializes all the buffer objects/arrays
    void setupMesh()
    {
        // create buffers/arrays
        context->makeCurrent();

        VAO = new QOpenGLVertexArrayObject(context);
        QOpenGLVertexArrayObject::Binder{VAO};

        VBO = new QOpenGLBuffer(QOpenGLBuffer::VertexBuffer);
        VBO->create();
        VBO->bind();
        VBO->allocate(&vertices[0], vertices.size() * sizeof(Vertex));
        shader_vertex->setAttributeBuffer(0, GL_FLOAT, 0, 3, sizeof(Vertex));
        shader_vertex->enableAttributeArray(0);
        shader_vertex->setAttributeBuffer(0, GL_FLOAT, sizeof(glm::vec3), 3, sizeof(Vertex));
        shader_vertex->enableAttributeArray(1);

//        VBO->bind();
        shader_face->setAttributeBuffer(0, GL_FLOAT, 0, 3, sizeof(Vertex));
        shader_face->enableAttributeArray(0);
        shader_face->setAttributeBuffer(1, GL_FLOAT, sizeof(glm::vec3), 3, sizeof(Vertex));
        shader_face->enableAttributeArray(1);

        if(func->glGetError() != GL_NO_ERROR){
            QMessageBox::warning(context, "Warning", "Error in shader face");
        }

        EBO = new QOpenGLBuffer(QOpenGLBuffer::IndexBuffer);
        EBO->create();
        EBO->bind();
        EBO->allocate(&indices[0], indices.size() * sizeof(unsigned int));

        FBO = new QOpenGLBuffer(QOpenGLBuffer::IndexBuffer);
        FBO->create();
        FBO->bind();
        FBO->allocate(&faces[0], faces.size() * sizeof(Face));

        if(func->glGetError() != GL_NO_ERROR){
            QMessageBox::warning(context, "Warning", "Error: shader lighting bind buffer");
        }

        context->doneCurrent();
    }

    // render the mesh
    void DrawVertex()
    {
        QOpenGLVertexArrayObject::Binder{VAO};
        func->glDrawArrays(GL_POINTS, 0, vertices.size());
    }

    void DrawEdge()
    {
        QOpenGLVertexArrayObject::Binder{VAO};
        EBO->bind();
        func->glDrawElements(GL_LINES, indices.size(), GL_UNSIGNED_INT, 0);
    }

    void DrawFace()
    {
        QOpenGLVertexArrayObject::Binder{VAO};
        FBO->bind();
        func->glDrawElements(GL_TRIANGLES, 3 * faces.size(), GL_UNSIGNED_INT, 0);
    }
};

#endif // MESH_H
