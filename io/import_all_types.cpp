#include "io/import_all_types.h"

bool import_all_types(const char *name, Mesh *ret)
{
    MyMesh mesh;
    int mask;

    int err;
    err = vcg::tri::io::Importer<MyMesh>::Open(mesh, name, mask);
    if(err)
        return false;
    vcg::tri::RequirePerVertexNormal(mesh);
    vcg::tri::UpdateNormal<MyMesh>::PerVertexNormalized(mesh);

    std::vector<MyVertex> &vertices = mesh.vert;
    std::vector<MyFace> &faces = mesh.face;

    for (auto &i : vertices){
        Vertex v(i.P().X(), i.P().Y(), i.P().Z(), i.N().X(), i.N().Y(), i.N().Z());
        ret->maxPoint.x = fmax(ret->maxPoint.x, v.Position.x);
        ret->maxPoint.y = fmax(ret->maxPoint.y, v.Position.y);
        ret->maxPoint.z = fmax(ret->maxPoint.z, v.Position.z);
        ret->minPoint.x = fmin(ret->minPoint.x, v.Position.x);
        ret->minPoint.y = fmin(ret->minPoint.y, v.Position.y);
        ret->minPoint.z = fmin(ret->minPoint.z, v.Position.z);
        ret->vertices.push_back(v);
    }

    ret->centerPoint = (ret->maxPoint + ret->minPoint) / 2.0f;

    for (auto &i : faces){
        Face f(i.V(0) - &vertices[0], i.V(1) - &vertices[0], i.V(2) - &vertices[0], \
                i.N().X(), i.N().Y(), i.N().Z());
        ret->faces.push_back(f);
    }

    return true;
}
