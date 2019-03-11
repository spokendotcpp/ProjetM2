#ifndef AXIS_H
#define AXIS_H

#include "drawableobject.h"

class Axis : public DrawableObject
{
private:
public:
    Axis();
    bool build(QOpenGLShaderProgram* program) override;
};

#endif // AXIS_H
