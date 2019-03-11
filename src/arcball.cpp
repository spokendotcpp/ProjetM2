#include "../include/arcball.h"

#include <cmath>
#include <iostream>

ArcBall::ArcBall(int w, int h):
    window_w(w), window_h(h)
{
    update_adjust_size();
}

ArcBall::~ArcBall()
{}

QVector3D
ArcBall::get_ball_coord(int x, int y)
{
    QVector3D coord = QVector3D(
        (x * adjust_w) - 1.0f,
        1.0f - (y*adjust_h),
        0.0f
    );

    // Pythagoras, compute z length thanks to x & y ones.
    float squared_len = (coord.x() * coord.x()) + (coord.y() * coord.y());
    if( squared_len <= 1.0f ) // where 1.0f is the squared sphere radius.
        coord.setZ(std::sqrt(1.0f - squared_len));

    // If the point is not in ball, get the nearest one.
    else
        coord.normalize();

    return coord;
}

void
ArcBall::update_adjust_size()
{
    adjust_w = 1.0f / ((window_w - 1.0f) * 0.5f);
    adjust_h = 1.0f / ((window_h - 1.0f) * 0.5f);
}

void
ArcBall::update_window_size(int w, int h)
{
    window_w = w;
    window_h = h;
    update_adjust_size();
}

/*
 * Get the rotation matrix (4x4) between two points.
 * Where a point is composed of two coordinates x and y (window coordinates).
 *
 * "click" position into window (last_x, last_y).
 * "drag" position into window (curr_x, curr_y).
 *
 * Rotation is handled by Quaternion:
 * one angle (radiants) + one vector (x, y, z).
 *
 */
QMatrix4x4
ArcBall::get_rotation_matrix(int curr_x, int curr_y, int last_x, int last_y)
{
    // Convert window coordinates to ball surfaces coordinates.
    QVector3D v0 = get_ball_coord(last_x, last_y); // last position
    QVector3D v1 = get_ball_coord(curr_x, curr_y); // current position

    // Get the angle + the perpendicular vector to v0 & v1:
    float angle = std::min(1.0f, QVector3D::dotProduct(v0, v1));
    QVector3D axis = QVector3D::crossProduct(v0, v1);

    QMatrix4x4 rotation;
    rotation.setToIdentity();

    // If the axis vector is not too small, rotate
    if( axis.length() > 1.0e-5f ){
        rotation.rotate(QQuaternion(angle, axis));
    }

    return rotation;
}


