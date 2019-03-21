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
    std::string _name;
    size_t _nb_faces;
    size_t _nb_vertices;
    MyMesh mesh;

private:
    std::string filename_from_path(const std::string& path) const;

public:
    MeshObject(const std::string&);
    ~MeshObject() override;

    bool build(QOpenGLShaderProgram* program) override;
    void normalize();

    inline size_t nb_faces() const { return _nb_faces; }
    inline size_t nb_vertices() const { return _nb_vertices; }
    inline const std::string& name() const { return _name; }
};

#endif // MESHOBJECT_H
