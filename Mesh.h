#ifndef MESH_H
#define MESH_H

#include <QOpenGLVertexArrayObject>
#include <QOpenGLBuffer>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <vector>

#include "Shader.h"

struct Vertex {
    // position
    glm::vec3 Position;
    // normal
//    glm::vec3 Normal;
    // texCoords
//    glm::vec2 TexCoords;
    // tangent
//    glm::vec3 Tangent;
    // bitangent
//    glm::vec3 Bitangent;
};

//struct Texture {
//    unsigned int id;
//    std::string type;
//    std::string path;
//};

class Mesh {
public:
    // mesh Data
    std::vector<Vertex>       vertices;
    std::vector<unsigned int> indices;
//    std::vector<Texture>      textures;
//    QOpenGLVertexArrayObject *VAO;
    QOpenGLFunctions *func;

    // constructor
    Mesh(QOpenGLFunctions *f)
    {
        func = f;
//        VAO = nullptr;
//        VBO = EBO = nullptr;
    }
    ~Mesh(){
//        if(VAO) delete VAO;
//        if(VBO) delete VBO;
//        if(EBO) delete EBO;
    }

    decltype (vertices.size()) getVerticesNum() {
        return vertices.size();
    }

    decltype (indices.size()) getIndicesNum() {
        return indices.size();
    }

    void pushVertex(Vertex v){
        vertices.push_back(v);
    }

    void pushVertex(float x, float y, float z){
        vertices.push_back(Vertex{glm::vec3(x, y, z)});
    }

    void pushIndex(unsigned int ind){
        indices.push_back(ind);
    }

    // initializes all the buffer objects/arrays
    void setupMesh()
    {
        // create buffers/arrays
//        if(!VAO)
//            VAO = new QOpenGLVertexArrayObject();
//        VAO->create();
//        VAO->bind();

//        if(!VBO)
//            VBO = new QOpenGLBuffer(QOpenGLBuffer::Type::VertexBuffer);
//        VBO->create();
//        VBO->bind();
//        VBO->allocate(&vertices[0], sizeof(Vertex) * vertices.size());

//        if(!EBO)
//            EBO = new QOpenGLBuffer(QOpenGLBuffer::Type::IndexBuffer);
//        EBO->create();
//        EBO->bind();
//        EBO->allocate(&indices[0], sizeof(unsigned int) * indices.size());

        if(func->glGetError() != GL_NO_ERROR){
            qDebug() << "Mesh prep error!" << endl;
        }

//        func->glEnableVertexAttribArray(0);
//        func->glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(Vertex), 0);

//        VBO->release();
//        VAO->release();
    }

    // render the mesh
    void DrawVertex(Shader *shader, bool drawVertex=true)
    {
        // draw mesh
//        VAO->bind();

        if(drawVertex){
//            func->glDrawArrays(GL_POINTS, 0, vertices.size());

            glBegin(GL_POINTS);
            for(auto &i : vertices){
                glVertex3f(i.Position.x, i.Position.y, i.Position.z);
            }
            glEnd();
        }

//        if(drawEdge)
//            func->glDrawElements(GL_LINES, indices.size(), GL_UNSIGNED_INT, 0);

        // always good practice to set everything back to defaults once configured.
//        VAO->release();
    }

    void DrawEdge(Shader *shader, bool drawEdge=true)
    {
//        VAO->bind();
        if(drawEdge){
//            func->glDrawElements(GL_LINES, indices.size(), GL_UNSIGNED_INT, 0);
            glBegin(GL_LINES);
            for(auto i : indices){
                Vertex &v = vertices[i];
                glVertex3f(v.Position.x, v.Position.y, v.Position.z);
            }
            glEnd();
        }
//        VAO->release();
    }

private:
    // render data
//    QOpenGLBuffer *VBO, *EBO;
};

#endif // MESH_H
