#ifndef MYVCGMESH_H
#define MYVCGMESH_H

#include <vcg/complex/complex.h>

class MyVertex; class MyEdge; class MyFace;
struct MyUsedTypes : public vcg::UsedTypes<vcg::Use<MyVertex> :: AsVertexType,
//        vcg::Use<MyEdge> :: AsEdgeType,
        vcg::Use<MyFace> :: AsFaceType>{};
class MyVertex : public vcg::Vertex< MyUsedTypes, vcg::vertex::Coord3f, vcg::vertex::Normal3f, vcg::vertex::BitFlags  >{};
//class MyFace : public vcg::Face<   MyUsedTypes, vcg::face::FFAdj, vcg::face::VertexRef, vcg::face::BitFlags > {};
class MyFace : public vcg::Face<   MyUsedTypes, vcg::face::Normal3f, vcg::face::VertexRef, vcg::face::BitFlags > {};
//class MyEdge : public vcg::Edge<   MyUsedTypes> {};
class MyMesh : public vcg::tri::TriMesh< std::vector<MyVertex>, std::vector<MyFace> > {};

#endif // MYVCGMESH_H
