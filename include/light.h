#ifndef LIGHT_H
#define LIGHT_H

#include <QOpenGLShaderProgram>
#include <QVector3D>

class Light {
private:
    QVector3D* position;
    QVector3D* color;
    float ambient;
    bool is_on;
    bool fixed;

    int uniform_location_position;
    int uniform_location_color;
    int uniform_location_ambient;
    int uniform_location_fixed;
    int uniform_location_is_on;

public:
    Light();
    ~Light();

    Light* set_position(float x, float y, float z, int uniform_loc);
    Light* set_color(float r, float g, float b, int uniform_loc);
    Light* set_ambient(float strength, int uniform_loc);
    Light* set_fixed(bool move, int uniform_loc);
    Light* enable(int uniform_loc);
    Light* disable();

    Light* update_position(float x, float y, float z);
    Light* update_color(float r, float g, float b);
    Light* update_ambient(float strength);
    Light* update_move_ability(bool move);

    const QVector3D& get_position() const;
    const QVector3D& get_color() const;
    float get_ambient() const;
    bool enabled() const;
    bool is_fixed() const;

    void to_gpu(QOpenGLShaderProgram* program) const;
    void on(QOpenGLShaderProgram* program=nullptr);
    void off(QOpenGLShaderProgram* program=nullptr);
};

#endif // LIGHT_H
