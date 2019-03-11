#ifndef ARCBALL_H
#define ARCBALL_H

#include <QVector3D>
#include <QMatrix4x4>
#include <QQuaternion>

/*
* Special thanks to:
*   - http://www.cabiatl.com/mricro/obsolete/graphics/3d.html
*   - https://en.wikibooks.org/wiki/OpenGL_Programming/Modern_OpenGL_Tutorial_Arcball (ArcBall without Quaternions)
*   - https://github.com/mmeeks/rep-snapper/blob/master/ArcBall.cpp
*/

class ArcBall {
private:
    int window_w;
    int window_h;

    float adjust_w;
    float adjust_h;

public:
    ArcBall(int w, int h);
    ~ArcBall();

    void update_speed_rotation(float speed);
    void update_window_size(int w, int h);
    QMatrix4x4 get_rotation_matrix(int curr_x, int curr_y, int last_x, int last_y);

private:
    QVector3D get_ball_coord(int x, int y);
    void update_adjust_size();
};

#endif // ARCBALL_H
