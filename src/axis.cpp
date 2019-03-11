#include "../include/axis.h"
#include <iostream>

Axis::Axis()
    :DrawableObject()
{}

bool
Axis::build(QOpenGLShaderProgram* program)
{
    size_t nb_vertices = 6;

    GLfloat* positions = new GLfloat[nb_vertices*3] {
        -1.0f, 0.0f, 0.0f,
        +1.0f, 0.0f, 0.0f,
        0.0f, -1.0f, 0.0f,
        0.0f, +1.0f, 0.0f,
        0.0f, 0.0f, -1.0f,
        0.0f, 0.0f, +1.0f
    };

    GLfloat* colors = new GLfloat[nb_vertices*3] {
        1.0f, 0.0f, 0.0f,
        1.0f, 0.0f, 0.0f,
        0.0f, 0.0f, 1.0f,
        0.0f, 0.0f, 1.0f,
        0.0f, 1.0f, 0.0f,
        0.0f, 1.0f, 0.0f
    };


    GLuint* indices = new GLuint[nb_vertices] {
        0, 1,  2, 3,  4, 5
    };

    set_vertices_geometry(program->attributeLocation("position"), positions, indices);
    set_vertices_colors(program->attributeLocation("color"), colors);

    return initialize(nb_vertices, nb_vertices, 3);
}
