#ifndef MESHVIEWERWIDGET_H
#define MESHVIEWERWIDGET_H

#include <chrono>
#include <random>

#include <QApplication>
#include <QScreen>
#include <QSize>
#include <QDir>
#include <QDesktopServices>
#include <QMessageBox>

#include <QTimerEvent>

#include <QMouseEvent>
#include <QWheelEvent>

#include <QOpenGLWidget>
#include <QOpenGLBuffer>
#include <QOpenGLVertexArrayObject>
#include <QOpenGLShaderProgram>
#include <QOpenGLFramebufferObject>

#include <QMatrix4x4>
#include <QInputDialog>

#include <QProgressBar>

#include "axis.h"
#include "light.h"
#include "arcball.h"
#include "meshobject.h"

typedef std::chrono::steady_clock Clock;

/*
 * QGLWidget child class.
 */

class MeshViewerWidget: public QOpenGLWidget
{
    Q_OBJECT
/* Private members */
private:
    QOpenGLShaderProgram* program;

    /* Matrix which compose our Model View Projection Matrix -- uniform values */
    QMatrix4x4 view;
    QMatrix4x4 projection;

    // FPS related
    Clock::time_point lap;
    long frequency;
    size_t frames;
    int timer_id_0;

    // Mouse related
    bool mouse_pressed;
    bool wheel_pressed;
    QPoint mouse;
    QVector3D position;
    QMatrix4x4 rotation;

    // Viewport related
    float fov;
    float zNear;
    float zFar;
    float window_ratio;

    // User objects into our scene
    Light* light;
    ArcBall* arcball;
    Axis* axis;
    MeshObject* mesh;

    // DISPLAY METHODS
    bool wireframe_on;
    bool fill_on;
    bool smooth_on;
    bool axis_on;

/* Public methods */
public:
    MeshViewerWidget(QWidget *parent=nullptr);
    ~MeshViewerWidget() override;

    /* *********************************************** */
    /* Qt OpenGL override functions */
    void initializeGL() override;
    void resizeGL(int width, int height) override;
    void paintGL() override;

    /* Qt override functions */
    /* Mouse */
    void mouseMoveEvent(QMouseEvent*) override;
    void mousePressEvent(QMouseEvent*) override;
    void mouseReleaseEvent(QMouseEvent*) override;

    /* QTimerEvent */
    void timerEvent(QTimerEvent*) override;

    /* Wheel */
    void wheelEvent(QWheelEvent*) override;

    /* *********************************************** */
    /* User defined methods */
    /* Since we don't want our QMainWindow to lose Focus,
     * we handle key events with our own function.
     * See mainwindow.cpp > keyPressEvent method.
     */
    void handle_key_events(QKeyEvent*);

    /* caps frames per second */
    void set_frames_per_second(size_t fps);

    /* Number of frames drawn during the last second */
    size_t get_computed_frames() const;

    /* Reset counter */
    void reset_computed_frames();

    /* Flat or Smooth render */
    void smooth_render(bool on);

    /* Display using GL_LINES PolygonMode */
    void display_wireframe(bool mode);

    /* Display using GL_FILL PolygonMode */
    void display_fill(bool mode);

    /* Reset view matrix to default */
    void reset_view();

    inline void use_default_bg_color(){
        makeCurrent();
        glClearColor(252.0f/255.0f, 224.0f/255.0f, 239.0f/255.0f, 1.0f);
    }

    inline void set_bg_color(float r, float g, float b)
    {
        makeCurrent();
        glClearColor(r, g, b, 1.0f);
    }

    inline Light* get_light() const { return light; }
    inline MeshObject* get_mesh() const { return mesh; }

    /* *********************************************** */
    /* STATIC METHODS */
    /* Difference between two high resolution clock time point as microseconds */
    static
    long microseconds_diff(Clock::time_point t1, Clock::time_point t2);

    /* ********************************************* */
public slots:
    void load_mesh_file(const std::string& str);
    void draw_back_faces(bool mode);
    void take_screenshots(int w, int h, Qt::AspectRatioMode aspect, int nimages, int quality, int format, QString dir, QProgressBar* pb);
    void show_axis(bool mode);
    void draw_wireframe(bool mode);
    void update_mesh_color(float r, float g, float b);
    void flip_back_faces(bool mode);

/* Private methods */
private:
    void default_view();
    void default_projection();
    void default_ModelViewPosition();

    void update_view();
    void update_projection();

    void update_lap();

    void draw_axis(QOpenGLShaderProgram* program);
};

#endif // MESHVIEWERWIDGET_H
