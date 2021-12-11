#include "export_all_types.h"

bool export_all_types(const char *name, Mesh *mesh)
{
    MyMesh export_mesh;

    std::vector<vcg::Point3f> coords;
    std::vector<vcg::Point3i> indices;

    for(auto &i : mesh->vertices){
        coords.push_back(vcg::Point3f(i.Position.x, i.Position.y, i.Position.z));
    }

    for(auto &i : mesh->faces){
        indices.push_back(vcg::Point3i(i.V[0], i.V[1], i.V[2]));
    }

    vcg::tri::BuildMeshFromCoordVectorIndexVector(export_mesh, coords, indices);

    int mask = 0;
    mask |= vcg::tri::io::Mask::IOM_VERTNORMAL;

    int err;
    err = vcg::tri::io::Exporter<MyMesh>::Save(export_mesh, name, mask);
    return err ? false : true;
}
