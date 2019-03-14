#include "../include/drawableobject.h"

#include <iostream>

DrawableObject::DrawableObject():
    nb_vertices(0),
    nb_elements(0),
    tuple_size(0),
    initialized(false),
    location_vertices_coordinates(-1),
    location_vertices_colors(-1),
    location_vertices_normals(-1),
    properties(0),
    raw_vertices_coordinates(nullptr),
    raw_vertices_colors(nullptr),
    raw_vertices_normals(nullptr),
    raw_vertices_indices(nullptr),
    vao(nullptr),
    ebo(nullptr),
    vbo(nullptr),
    model(new QMatrix4x4())
{
    reset_model_matrix();
}

DrawableObject::~DrawableObject()
{
    if( model != nullptr ){
        delete model;
        model = nullptr;
    }

    free_vertices_colors();
    free_vertices_normals();
    free_vertices_geometry();
    free_buffers();
}

DrawableObject::DrawableObject(const DrawableObject& obj)
    :DrawableObject()
{
    obj.model->copyDataTo(model->data());

    obj.copy_geometry_to(this);
    obj.copy_colors_to(this);
    obj.copy_normals_to(this);

    initialize(obj.nb_vertices, obj.nb_elements, obj.tuple_size);
}

bool
DrawableObject::update_buffers(QOpenGLShaderProgram* program)
{
    if( !initialized ){
        std::cerr << "initialize() function was not called." << std::endl;
        std::cerr << "Cannot proceed to build()" << std::endl;
        return false;
    }

    vao->bind();
    {
        ebo->bind();
        ebo->setUsagePattern(QOpenGLBuffer::StaticDraw);
        ebo->allocate(raw_vertices_indices, int(sizeof(GLuint) * nb_elements));

        int offset = 0;
        int bytes = int(sizeof(GLfloat) * nb_vertices * tuple_size);

        vbo->bind();
        vbo->setUsagePattern(QOpenGLBuffer::StaticDraw);
        vbo->allocate( int(properties) * bytes );

        if( location_vertices_coordinates >= 0 ){
            vbo->write(offset, raw_vertices_coordinates, bytes);
            program->enableAttributeArray(location_vertices_coordinates);
            program->setAttributeBuffer(location_vertices_coordinates, GL_FLOAT, offset, int(tuple_size), 0);
        }

        if( location_vertices_colors >= 0 ){
            offset += bytes;
            vbo->write(offset, raw_vertices_colors, bytes);
            program->enableAttributeArray(location_vertices_colors);
            program->setAttributeBuffer(location_vertices_colors, GL_FLOAT, offset, int(tuple_size), 0);
        }

        if( location_vertices_normals >= 0 ){
            offset += bytes;
            vbo->write(offset, raw_vertices_normals, bytes);
            program->enableAttributeArray(location_vertices_normals);
            program->setAttributeBuffer(location_vertices_normals, GL_FLOAT, offset, int(tuple_size), 0);
        }
    }
    vao->release();
    ebo->release();
    vbo->release();

    return true;
}


void
DrawableObject::show(QOpenGLShaderProgram* program, GLenum mode) const
{
    if( initialized ){
        // Update uniform values into vertex shader
        program->setUniformValue("model", *model); // shader transformation computation
        program->setUniformValue("model_inverse", model->transposed().inverted()); // shader light computation

        vao->bind();
        glDrawElements(mode, GLsizei(nb_elements), GL_UNSIGNED_INT, nullptr);
        vao->release();
    }
}

void
DrawableObject::translate(float x, float y, float z)
{
    model->translate(x, y, z);
}

void
DrawableObject::scale(float x, float y, float z)
{
    model->scale(x, y, z);
}

void
DrawableObject::rotate(float angle, float x, float y, float z)
{
    model->rotate(angle, x, y, z);
}

void
DrawableObject::reset_model_matrix()
{
    model->setToIdentity();
    model->optimize();
}

const QMatrix4x4&
DrawableObject::model_matrix() const
{
    return *model;
}


/*
 * This function which fills the raw_vertices_colors array.
 * It needs to be called before the `build()` function;
 * Otherwise, the color setup will not be used.
 */
void
DrawableObject::use_unique_color(float r, float g, float b)
{
    if( raw_vertices_colors == nullptr ){
        std::cerr << "`set_vertices_colors()` need to be called before." << std::endl;
        return;
    }

    for(size_t i=0; i < 3*nb_vertices; i+=3){
        raw_vertices_colors[i] = r;
        raw_vertices_colors[i+1] = g;
        raw_vertices_colors[i+2] = b;
    }
}

void
DrawableObject::copy_geometry_to(DrawableObject* obj) const
{
    const GLfloat* positions = get_vertices_coordinates();
    if( positions != nullptr ){
        GLfloat* copy_positions = new GLfloat[nb_vertices*tuple_size];
        for(size_t i=0; i < nb_vertices*tuple_size; ++i)
            copy_positions[i] = positions[i];

        const GLuint* indices = get_vertices_indices();
        GLuint* copy_indices = new GLuint[nb_elements];
        for(size_t i=0; i < nb_elements; ++i)
            copy_indices[i] = indices[i];

        obj->set_vertices_geometry(
            location_vertices_coordinates,
            copy_positions,
            copy_indices
        );
    }
}

void
DrawableObject::copy_colors_to(DrawableObject* obj) const
{
    const GLfloat* colors = get_vertices_colors();
    if( colors != nullptr ){
        GLfloat* copy_colors = new GLfloat[nb_vertices*3];
        for(size_t i=0; i < nb_vertices*tuple_size; ++i)
            copy_colors[i] = colors[i];

        obj->set_vertices_colors(location_vertices_colors, copy_colors);
    }
}

void
DrawableObject::copy_normals_to(DrawableObject* obj) const
{
    const GLfloat* normals = get_vertices_normals();
    GLfloat* copy_normals = nullptr;

    if( normals != nullptr ){
        copy_normals = new GLfloat[nb_vertices*3];
        for(size_t i=0; i < nb_vertices*tuple_size; ++i)
            copy_normals[i] = normals[i];
        obj->set_vertices_normals(location_vertices_normals, copy_normals);
    }
}

const GLfloat*
DrawableObject::get_vertices_coordinates() const
{
    return raw_vertices_coordinates;
}

const GLuint*
DrawableObject::get_vertices_indices() const
{
    return raw_vertices_indices;
}

const GLfloat*
DrawableObject::get_vertices_colors() const
{
    return raw_vertices_colors;
}

const GLfloat*
DrawableObject::get_vertices_normals() const
{
    return raw_vertices_normals;
}

bool
DrawableObject::initialize(size_t _nb_vertices, size_t _nb_elements, size_t _tuple_size)
{
    if( create_buffers() ){
        nb_vertices = _nb_vertices;
        nb_elements = _nb_elements;
        tuple_size = _tuple_size;
        initialized = true;
    }
    else {
        std::cerr << "Failed to create GPU buffers." << std::endl;
    }

    return initialized;
}

void
DrawableObject::set_vertices_geometry(int shader_location, GLfloat* coordinates, GLuint* indices)
{
    if( shader_location < 0 )
        return;

    // exists yet
    if( location_vertices_coordinates >= 0 )
        free_vertices_geometry();
    else
        ++properties;

    location_vertices_coordinates = shader_location;
    free_vertices_geometry();
    raw_vertices_coordinates = coordinates;
    raw_vertices_indices = indices;
}

void
DrawableObject::set_vertices_colors(int shader_location, GLfloat* colors)
{
    if( shader_location < 0 )
        return;

    if( location_vertices_colors >= 0 )
        free_vertices_colors();
    else
        ++properties;

    location_vertices_colors = shader_location;
    free_vertices_colors();
    raw_vertices_colors = colors;
}

void
DrawableObject::set_vertices_normals(int shader_location, GLfloat* normals)
{
    if( shader_location < 0 )
        return;

    if( location_vertices_normals >= 0 )
        free_vertices_normals();
    else
        ++properties;

    location_vertices_normals = shader_location;

    free_vertices_normals();
    raw_vertices_normals = normals;
}

void
DrawableObject::free_vertices_geometry()
{
    if( raw_vertices_coordinates != nullptr ){
        delete [] raw_vertices_coordinates;
        raw_vertices_coordinates = nullptr;
    }

    if( raw_vertices_indices != nullptr ){
        delete [] raw_vertices_indices;
        raw_vertices_indices = nullptr;
    }

    nb_vertices = 0;
    nb_elements = 0;
}

void
DrawableObject::free_vertices_colors()
{
    if( raw_vertices_colors != nullptr ){
        delete [] raw_vertices_colors;
        raw_vertices_colors = nullptr;
    }
}

void
DrawableObject::free_vertices_normals()
{
    if( raw_vertices_normals != nullptr ){
        delete [] raw_vertices_normals;
        raw_vertices_normals = nullptr;
    }
}

void
DrawableObject::free_buffers()
{
    if( vao != nullptr ){
        vao->destroy();
        delete vao;
        vao = nullptr;
    }

    if( ebo != nullptr ){
        ebo->destroy();
        delete ebo;
        ebo = nullptr;
    }

    if( vbo != nullptr ){
        vbo->destroy();
        delete vbo;
        vbo = nullptr;
    }
}

bool
DrawableObject::create_buffers()
{
    free_buffers();

    vao = new QOpenGLVertexArrayObject();
    ebo = new QOpenGLBuffer(QOpenGLBuffer::IndexBuffer);
    vbo = new QOpenGLBuffer();

    return vao->create() && ebo->create() && vbo->create();
}
