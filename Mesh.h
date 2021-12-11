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
    unsigned V[3];
    // Face normal
    glm::vec3 Normal;

    Face(unsigned int V0=-1, unsigned int V1=-1, unsigned int V2=-1, float x=0, float y=0, float z=0)
    {
        V[0] = V0, V[1] = V1, V[2] = V2;
        Normal = glm::vec3(x, y, z);
    }
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
//    std::vector<unsigned int> indices;
    std::vector<Face>         faces;
//    std::vector<Texture>      textures;
//    QOpenGLVertexArrayObject *VAO;
    QOpenGLFunctions *func;
    std::string mesh_file;
    glm::vec3 maxPoint;
    glm::vec3 minPoint;
    glm::vec3 centerPoint;
    const float MINMAXBOUND = 1e6f;

    // constructor
    Mesh(QOpenGLFunctions *f)
    {
        func = f;

        maxPoint = glm::vec3(-MINMAXBOUND);
        minPoint = glm::vec3(MINMAXBOUND);
        centerPoint = glm::vec3(0.0f);
    }
    ~Mesh(){

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

//    void pushIndex(unsigned int ind){
//        indices.push_back(ind);
//    }

    // initializes all the buffer objects/arrays
    void setupMesh()
    {
        // create buffers/arrays

        if(func->glGetError() != GL_NO_ERROR){
            qDebug() << "Mesh prep error!" << endl;
        }
    }

    // render the mesh
    void DrawVertex(Shader *shader, bool drawVertex=true)
    {
        if(drawVertex){
            glDisable(GL_LIGHTING);
            glBegin(GL_POINTS);
            for(auto &i : vertices){
                glVertex3f(i.Position.x, i.Position.y, i.Position.z);
            }
            glEnd();
            glEnable(GL_LIGHTING);
        }
    }

    void DrawEdge(Shader *shader, bool drawEdge=true)
    {
        if(drawEdge){
            glDisable(GL_LIGHTING);
            glBegin(GL_LINES);
            for(auto &i : faces){
                glVertex3f(vertices[i.V[0]].Position.x, vertices[i.V[0]].Position.y, vertices[i.V[0]].Position.z);
                for(int j=1; j<3; j++){
                    glVertex3f(vertices[i.V[j]].Position.x, vertices[i.V[j]].Position.y, vertices[i.V[j]].Position.z);
                    glVertex3f(vertices[i.V[j]].Position.x, vertices[i.V[j]].Position.y, vertices[i.V[j]].Position.z);
                }
                glVertex3f(vertices[i.V[0]].Position.x, vertices[i.V[0]].Position.y, vertices[i.V[0]].Position.z);
            }
            glEnd();
            glEnable(GL_LIGHTING);
        }
    }

     void DrawFace(Shader *shader, bool drawFace=true)
     {
         if(drawFace){
             glDisable(GL_LIGHTING);
             glBegin(GL_TRIANGLES);
//            for(auto &i : indices){
//                glNormal3f(vertices[i].Normal.x, vertices[i].Normal.y, vertices[i].Normal.z);
//                glVertex3f(vertices[i].Position.x, vertices[i].Position.y, vertices[i].Position.z);
//            }
             for(auto &i : faces){
                 glNormal3f(i.Normal.x, i.Normal.y, i.Normal.z);
                 for(int j=0; j<3; j++){
                     glVertex3f(vertices[i.V[j]].Position.x, vertices[i.V[j]].Position.y, vertices[i.V[j]].Position.z);
                 }
             }
             glEnd();
             glEnable(GL_LIGHTING);
         }
     }

private:
};

#endif // MESH_H
