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
    std::vector<MyEdge> &edges = mesh.edge; // edge normally unused
    std::vector<MyFace> &faces = mesh.face;

    for (auto &i : vertices){
        Vertex v(i.P().X(), i.P().Y(), i.P().Z(), i.N().X(), i.N().Y(), i.N().Z());
        ret->vertices.push_back(v);
    }

    for (auto &i : faces){
        ret->indices.push_back(i.V(0) - &vertices[0]);
        ret->indices.push_back(i.V(1) - &vertices[0]);
        ret->indices.push_back(i.V(2) - &vertices[0]);
    }

    return true;
}
