#ifndef MESHOBJECT_H
#define MESHOBJECT_H

#include <string>

#include <OpenMesh/Core/IO/MeshIO.hh>
#include <OpenMesh/Core/Mesh/TriMesh_ArrayKernelT.hh>
#include "drawableobject.h"

struct MyTraits : public OpenMesh::DefaultTraits {
    VertexAttributes(OpenMesh::Attributes::Normal);
    FaceAttributes(OpenMesh::Attributes::Normal);
    HalfedgeAttributes(OpenMesh::Attributes::PrevHalfedge);
};

typedef OpenMesh::TriMesh_ArrayKernelT<MyTraits> MyMesh;

class MeshObject : public DrawableObject
{
private:
    MyMesh mesh;

public:
    MeshObject(const std::string&);
    ~MeshObject() override;

    bool build(QOpenGLShaderProgram* program) override;
    void normalize();
};

#endif // MESHOBJECT_H
