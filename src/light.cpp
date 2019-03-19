#include "../include/light.h"
#include <iostream>

Light::Light()
    :position(new QVector3D(0.0f, 0.0f, 0.0f)),
     color(new QVector3D(1.0f, 1.0f, 1.0f)),
     ambient(0.5f),
     is_on(false),
     fixed(false),
     uniform_location_position(-1),
     uniform_location_color(-1),
     uniform_location_ambient(-1),
     uniform_location_fixed(-1),
     uniform_location_is_on(-1)
{}

Light::~Light()
{
    if( position != nullptr ){
        delete position;
        position = nullptr;
    }

    if( color != nullptr ){
        delete color;
        color = nullptr;
    }
}

Light*
Light::set_position(float x, float y, float z, int loc)
{
    uniform_location_position = loc;
    return update_position(x, y, z);
}

Light*
Light::set_color(float r, float g, float b, int loc)
{
    uniform_location_color = loc;
    return update_color(r, g, b);
}

Light*
Light::set_ambient(float strength, int loc)
{
    uniform_location_ambient = loc;
    return update_ambient(strength);
}

Light*
Light::set_fixed(bool move, int loc)
{
    uniform_location_fixed = loc;
    return update_move_ability(move);
}

Light*
Light::enable(int loc)
{
    uniform_location_is_on = loc;
    is_on = true;
    return this;
}

Light*
Light::disable()
{
    is_on = false;
    uniform_location_is_on = -1;
    return this;
}

Light*
Light::update_position(float x, float y, float z)
{
    position->setX(x);
    position->setY(y);
    position->setZ(z);
    return this;
}

Light*
Light::update_color(float r, float g, float b)
{
    color->setX(r);
    color->setY(g);
    color->setZ(b);
    return this;
}

Light*
Light::update_ambient(float strength)
{
    ambient = strength;
    return this;
}

Light*
Light::update_move_ability(bool move)
{
    fixed = move;
    return this;
}

const QVector3D&
Light::get_color() const
{
    return *color;
}

const QVector3D&
Light::get_position() const
{
    return *position;
}

float
Light::get_ambient() const
{
    return ambient;
}

bool
Light::enabled() const
{
    return (is_on && (uniform_location_is_on >= 0));
}

bool
Light::is_fixed() const
{
    return fixed;
}

/* Watchout !
 * GLSL compiler is allowed to optimize away any unused uniforms.
 * program->uniformLocation(<string>) returns -1 if it's the case.
 */
void
Light::to_gpu(QOpenGLShaderProgram* program) const
{
    if( uniform_location_is_on >= 0 ){
        program->setUniformValue(uniform_location_is_on, is_on);

        if( uniform_location_position >= 0 )
            program->setUniformValue(uniform_location_position, *position);
        else
            std::cerr << "Warning: uniform_location_position wasn't found into shader(s)." << std::endl;

        if( uniform_location_color >= 0 )
            program->setUniformValue(uniform_location_color, *color);
        else
            std::cerr << "Warning: uniform_location_color wasn't found into shader(s)." << std::endl;

        if( uniform_location_ambient >= 0 )
            program->setUniformValue(uniform_location_ambient, ambient);
        else
            std::cerr << "Warning: uniform_location_ambient wasn't found into shader(s)." << std::endl;

        if( uniform_location_fixed >= 0 )
            program->setUniformValue(uniform_location_fixed, fixed);
        else
            std::cerr << "Warning: uniform_location_fixed wasn't found into shader(s)." << std::endl;
    }
}

void
Light::on(QOpenGLShaderProgram* program)
{
    is_on = true;
    if( program && uniform_location_is_on >= 0 )
        program->setUniformValue(uniform_location_is_on, is_on);
}

void
Light::off(QOpenGLShaderProgram* program)
{
    is_on = false;
    if( program && uniform_location_is_on >= 0 )
        program->setUniformValue(uniform_location_is_on, is_on);
}
