#include "../include/meshobject.h"

#include <iostream>

MeshObject::MeshObject(const std::string& filename)
    :DrawableObject()
{
    mesh.request_vertex_normals();
    mesh.request_face_normals();

    if( OpenMesh::IO::read_mesh(mesh, filename) ){
        // normalize();
        // mesh.update_face_normals();
        // mesh.update_vertex_normals();
    }
    else {
        std::cerr << "Failed to read " << filename << std::endl;
    }
}

MeshObject::~MeshObject()
{
    mesh.release_face_colors();
    mesh.release_vertex_normals();
}

bool
MeshObject::build(QOpenGLShaderProgram* program)
{
    size_t nb_vertices = mesh.n_vertices()*3;
    size_t nb_indices = mesh.n_faces()*3;

    GLuint* indices = new GLuint[nb_indices];
    GLfloat* positions = new GLfloat[nb_vertices];
    GLfloat* normals = new GLfloat[nb_vertices];
    GLfloat* colors = new GLfloat[nb_vertices];

    MyMesh::Normal normal;
    MyMesh::Point point;
    MyMesh::ConstFaceVertexIter cfv_it;

    size_t i = 0;
    size_t j = 0;

    for(const auto& cv_it: mesh.vertices()){
        /* const vertex iterator */
        normal = mesh.normal(cv_it);
        point = mesh.point(cv_it);
        for(j=0; j < 3; ++j, ++i){
            normals[i] = normal[j];
            positions[i] = point[j];
            colors[i] = 0.5f;
        }
    }

    i = 0;
    for(const auto& cf_it: mesh.faces()){
        /* const face iterator */
        cfv_it = mesh.cfv_iter(cf_it);
        for(j=0; j < 3; ++j, ++i, ++cfv_it)
            indices[i] = static_cast<GLuint>(cfv_it->idx());
    }

    set_vertices_geometry(program->attributeLocation("position"), positions, indices);
    set_vertices_colors(program->attributeLocation("color"), colors);
    set_vertices_normals(program->attributeLocation("normal"), normals);

    return initialize(nb_vertices, nb_vertices, 3);
}

void
MeshObject::normalize()
{
    float min_x, min_y, min_z;
    float max_x, max_y, max_z;
    float scale;

    min_x = min_y = min_z = std::numeric_limits<float>::max();
    max_x = max_y = max_z = std::numeric_limits<float>::min();

    MyMesh::Point p;
    for(const auto& v_it: mesh.vertices()){
        p = mesh.point(v_it);

        min_x = std::min(min_x, p[0]);
        min_y = std::min(min_y, p[1]);
        min_z = std::min(min_z, p[2]);

        max_x = std::max(max_x, p[0]);
        max_y = std::max(max_y, p[1]);
        max_z = std::max(max_z, p[2]);
    }

    MyMesh::Point pos( min_x+max_x, min_y+max_y, min_z+max_z);
    pos *= 0.5f;

    MyMesh::Point size = MyMesh::Point(0.0f, 0.0f, 0.0f);
    MyMesh::Point bboxmin(min_x, min_y, min_z);
    MyMesh::Point bboxmax(max_x, max_y, max_z);

    for(size_t i=0; i < 3; ++i)
        size[i] = std::fabs(bboxmax[i] - bboxmin[i]);

    if( size[0] > size[1] ){
        if( size[0] > size[2] )
            scale = 1.0f / size[0];
        else
            scale = 1.0f / size[2];
    }
    else {
        if( size[1] > size[2] )
            scale = 1.0f / size[1];
        else
            scale = 1.0f / size[2];
    }

    for(auto& v_it: mesh.vertices()){
        mesh.point(v_it) = (mesh.point(v_it) - pos)*scale;
    }
}
