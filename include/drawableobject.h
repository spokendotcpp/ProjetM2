#ifndef DRAWABLEOBJECT_H
#define DRAWABLEOBJECT_H

#include <QOpenGLShaderProgram>
#include <QOpenGLVertexArrayObject>
#include <QOpenGLBuffer>

#include <QMatrix4x4>

class DrawableObject {
private:
    size_t nb_vertices; // Number of vertices into our Object
    size_t nb_elements; // Number of elements to draw (IndexBufferObject)
    size_t tuple_size; // How many coordinates per vertices (vertices 2D/3D)

    // needed to use build() function.
    bool initialized;

    // Shader location
    int location_vertices_coordinates;
    int location_vertices_colors;
    int location_vertices_normals;

    size_t properties;

    // Raw data transfered to VBO
    GLfloat* raw_vertices_coordinates;
    GLfloat* raw_vertices_colors;
    GLfloat* raw_vertices_normals;
    GLuint* raw_vertices_indices;

    // Buffers
    QOpenGLVertexArrayObject* vao;
    QOpenGLBuffer* ebo;
    QOpenGLBuffer* vbo;

    // Model matrix
    QMatrix4x4* model;

public:
    DrawableObject();
    DrawableObject(const DrawableObject& obj);
    virtual ~DrawableObject();

    virtual bool build(QOpenGLShaderProgram* program) =0;
    bool update_buffers(QOpenGLShaderProgram* program);
    virtual void show(QOpenGLShaderProgram* program, GLenum mode) const;

    void translate(float x, float y, float z);
    void scale(float x, float y, float z);
    void rotate(float angle, float x, float y, float z);

    void reset_model_matrix();
    const QMatrix4x4& model_matrix() const;

    void use_unique_color(float r, float g, float b);
    void copy_geometry_to(DrawableObject* obj) const;
    void copy_colors_to(DrawableObject* obj) const;
    void copy_normals_to(DrawableObject* obj) const;

    const GLfloat* get_vertices_coordinates() const;
    const GLuint* get_vertices_indices() const;
    const GLfloat* get_vertices_colors() const;
    const GLfloat* get_vertices_normals() const;

protected:
    bool initialize(size_t nb_vertices, size_t nb_elements, size_t tuple_size);

    void set_vertices_geometry(int shader_location, GLfloat* coordinates, GLuint* indices);
    void set_vertices_colors(int shader_location, GLfloat* data);
    void set_vertices_normals(int shader_location, GLfloat* data);

private:
    void free_vertices_geometry();
    void free_vertices_colors();
    void free_vertices_normals();
    void free_buffers();

    bool create_buffers();
};

#endif // DRAWABLEOBJECT_H
