#include <iostream>
#include <thread>
#include <future>

#include "../include/meshviewerwidget.h"
#include "../include/mainwindow.h"

MeshViewerWidget::MeshViewerWidget(QWidget* parent)
    :QOpenGLWidget(parent)
{
    setMouseTracking(true);

    mouse_pressed = false;
    wheel_pressed = false;

    wireframe_on = true;
    fill_on = true;
    smooth_on = true;
    axis_on = true;

    frames = 0;
    lap = Clock::now();

    program = nullptr;

    arcball = nullptr;
    light = nullptr;
    axis = nullptr;
    mesh = nullptr;
}

/*
 * Destructor:
 *
 * Destroy every Buffers (Vertex Buffer Object: ARRAY_BUFFER, INDEX_BUFFER AND Vertex Array Object).
 * Free memory.
*/
MeshViewerWidget::~MeshViewerWidget()
{
    if( arcball == nullptr ){
        delete arcball;
        arcball = nullptr;
    }

    if( program != nullptr ){
        program->removeAllShaders();
        delete program;
        program = nullptr;
    }

    if( light != nullptr ){
        delete light;
        light = nullptr;
    }

    if( axis != nullptr ){
        delete axis;
        axis = nullptr;
    }

    if( mesh != nullptr ){
        delete mesh;
        mesh = nullptr;
    }
}

/* Update the view matrix
 * i.e.: can be called after every user inputs (mouse, keyboard ...)
 *
*/
void
MeshViewerWidget::update_view()
{
    view.setToIdentity();
    view.translate(position);
    view *= rotation;
}

/* Update the projection matrix
 * -> used when changing window size.
*/
void
MeshViewerWidget::update_projection()
{
    projection.setToIdentity();
    projection.perspective(
        fov,
        window_ratio,
        zNear, zFar
    );
}

/* Load default values for the view matrix + update */
void
MeshViewerWidget::default_view()
{
    rotation = QMatrix4x4();
    rotation.rotate(-90.0f, 1.0f, 0.0f, 0.0f);

    position = QVector3D(0.0f, 0.0f, -1.5f);
    update_view();
}

/* Load default values for the projection matrix + update */
void
MeshViewerWidget::default_projection()
{
    fov = 45.0f;
    zNear = 0.001f;
    zFar = 1000.0f;
    update_projection();
}

/* Load every defaults */
void
MeshViewerWidget::default_ModelViewPosition()
{
    default_view();
    default_projection();
}

/* Setup our OpenGL context
 * Vertex Array Buffer,
 * Vertex Buffer Object,
 * Elements Buffer Object,
 * Shaders programs,
 * [ ... ]
*/
void
MeshViewerWidget::initializeGL()
{
    // Check if OpenGL context was successfully initialized
    if( !isValid() ){
        std::cerr << "Failed to init OpenGL context" << std::endl;
        return;
    }

    arcball = new ArcBall(width(), height());

    // Create Object(s) :
    axis = new Axis();

    program = new QOpenGLShaderProgram();
    program->addShaderFromSourceFile(QOpenGLShader::Vertex, "../shaders/simple.vert.glsl");
    program->addShaderFromSourceFile(QOpenGLShader::Fragment, "../shaders/simple.frag.glsl");
    program->link();
    program->bind();
    {
        light = new Light();
        light->set_position(0.0f, 100.0f, 200.0f, program->uniformLocation("light_position"))
             ->set_color(0.9f, 0.9f, 0.9f, program->uniformLocation("light_color"))
             ->set_ambient(0.4f, program->uniformLocation("light_ambient"))
             ->set_fixed(true, program->uniformLocation("light_fixed"))
             ->enable(program->uniformLocation("light_on"));

        axis->build(program);
        axis->update_buffers(program);
    }
    program->release();

    use_default_bg_color();
    glEnable(GL_DEPTH_TEST);
    glDepthFunc(GL_LESS);
    glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);

    default_ModelViewPosition();

    // IDLE Function
    timer_id_0 = startTimer(0);
}

/* When window (this widget) is resized */
void
MeshViewerWidget::resizeGL(int width, int height)
{
    glViewport(0, 0, width, height);
    window_ratio = width/float(height);
    arcball->update_window_size(width, height);
    update_projection();
    update();
}

/* RENDER TIME */
void
MeshViewerWidget::paintGL()
{
    // Wait refresh-rate setup by user before painting
    long mcs = MeshViewerWidget::microseconds_diff(Clock::now(), lap);
    if( mcs < frequency ){
        std::this_thread::sleep_for(
            std::chrono::microseconds(frequency - (mcs + 100))
        );
    }

    update_lap(); // increment FPS counter

    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    program->bind();
    {
        // send light parameters to shaders
        light->to_gpu(program);

        // push projection & views matrix to the GPU
        program->setUniformValue("projection", projection);
        program->setUniformValue("view", view);
        program->setUniformValue("view_inverse", view.transposed().inverted());

        if( axis_on )
            draw_axis(program);

        // In case user imported a mesh into the viewer, display it.
        if( mesh != nullptr ){
            mesh->show(program, GL_TRIANGLES);
        }
    }
    program->release();
}

/* When mouse is moving inside the widget */
void
MeshViewerWidget::mouseMoveEvent(QMouseEvent* event)
{
    QPoint pos = event->pos();

    if( mouse_pressed ){
        // The new rotation equals arcball rotation * the previous.
        rotation = arcball->get_rotation_matrix(
            pos.x(), pos.y(), mouse.x(), mouse.y()
        ) * rotation;
    }
    else
    if( wheel_pressed ){
        float step = position.z() / (zFar - zNear);

        position.setX(position.x() - ((pos.x() - mouse.x())*step));
        position.setY(position.y() + ((pos.y() - mouse.y())*step));
    }

    mouse = pos;
    update_view();
    update();
}

void
MeshViewerWidget::mousePressEvent(QMouseEvent* event)
{
    if( event->button() == Qt::MouseButton::LeftButton ){
        mouse_pressed = true;
        mouse = event->pos();
    }
    else
    if( event->button() == Qt::MouseButton::MiddleButton ){
        wheel_pressed = true;
        mouse = event->pos();
    }
}

void
MeshViewerWidget::mouseReleaseEvent(QMouseEvent* event)
{
    if( event->button() == Qt::MouseButton::LeftButton )
        mouse_pressed = false;
    else
    if( event->button() == Qt::MouseButton::MiddleButton )
        wheel_pressed = false;
}

// IDLE Function: when no user inputs repaint
void
MeshViewerWidget::timerEvent(QTimerEvent* event)
{
    int id = event->timerId();

    if( id == timer_id_0 ){
        update();
    }
}

/*
 * Make sure that position.z()
 * stay between ]zNear+step & zFar-step[
*/
void
MeshViewerWidget::wheelEvent(QWheelEvent* event)
{
    float speed = 0.5f;
    float step = (event->delta() * speed) * (position.z() / (zFar-zNear));

    position.setZ(position.z() - step);
    update_view();
    update();
}

void
MeshViewerWidget::handle_key_events(QKeyEvent* event)
{
    float step = position.z()/(zFar - zNear);

    switch( event->key() ){
    case Qt::Key_Up :
        position.setY(position.y()-step);
        update_view();
        break;

    case Qt::Key_Down :
        position.setY(position.y()+step);
        update_view();
        break;

    case Qt::Key_Left :
        position.setX(position.x()+step);
        update_view();
        break;

    case Qt::Key_Right :
        position.setX(position.x()-step);
        update_view();
        break;
    }

    update();
}

void
MeshViewerWidget::set_frames_per_second(size_t fps)
{
    frequency = long(1.0f/fps * 1000000);
}

size_t
MeshViewerWidget::get_computed_frames() const
{
    return frames;
}

void
MeshViewerWidget::reset_computed_frames()
{
    frames = 0;
}

void
MeshViewerWidget::update_lap()
{
    ++frames;
    lap = Clock::now();
}

void
MeshViewerWidget::draw_axis(QOpenGLShaderProgram* program)
{
    bool light_on = light->enabled();
    if( light_on ) light->off(program);
    axis->show(program, GL_LINES);
    if( light_on ) light->on(program);
}

void
MeshViewerWidget::show_axis(bool mode)
{
    axis_on = mode;
    update();
}

void
MeshViewerWidget::draw_back_faces(bool mode)
{
    makeCurrent();
    mode ? glDisable(GL_CULL_FACE) : glEnable(GL_CULL_FACE);
    doneCurrent();
    update();
}

void
MeshViewerWidget::draw_wireframe(bool mode)
{
    makeCurrent();

    mode ?
    glPolygonMode(GL_FRONT_AND_BACK, GL_LINE):
    glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);

    doneCurrent();
    update();
}

void
MeshViewerWidget::smooth_render(bool on)
{
    smooth_on = on;
    update();
}

// The delay between two time in microseconds
long
MeshViewerWidget::microseconds_diff(
        Clock::time_point t1,
        Clock::time_point t2)
{
    return std::chrono::duration_cast<std::chrono::microseconds>(t1-t2).count();
}

void
MeshViewerWidget::display_wireframe(bool mode)
{
    wireframe_on = mode;
    update();
}

void
MeshViewerWidget::display_fill(bool mode)
{
    fill_on = mode;
    update();
}

void
MeshViewerWidget::reset_view()
{
    default_view();
    update_view();
    update();
}

/* Load OBJ or OFF mesh from disk */
void
MeshViewerWidget::load_mesh_file(const std::string& str)
{
    MeshObject* mesh = new MeshObject(str);

    if( mesh != nullptr ){
        if( this->mesh != nullptr )
            delete this->mesh;

        this->mesh = mesh;

        makeCurrent();
        {
            program->bind();
            {
                this->mesh->build(program);
                this->mesh->update_buffers(program);
            }
            program->release();
        }
        doneCurrent();
    }
}

void
MeshViewerWidget::flip_back_faces(bool mode)
{
    makeCurrent();
    program->bind();

    program->setUniformValue("flip_bfaces", mode);

    program->release();
    doneCurrent();
}

void
MeshViewerWidget::update_mesh_color(float r, float g, float b)
{
    if( mesh == nullptr )
        return;

    makeCurrent();
    program->bind();

    mesh->use_unique_color(r, g, b);
    mesh->update_buffers(program);

    program->release();
    doneCurrent();
}

void
MeshViewerWidget::take_screenshots(int width, int height, Qt::AspectRatioMode aspect, int nimages, int quality, int format, QString dir, QProgressBar* pb)
{
    // Possible images formats
    const char* extension[2] = {
        ".jpg", ".png"
    };

    // If format integer is not into boundaries, something is wrong, so go default
    if( format < 0 || format > 1 )
        format = 0;

    // UI progress bar range
    pb->setRange(0, nimages-1);

    // Create a random device + a random generator, then two uniform distributions
    // one for angles in degrees which goes to -360.0 to 360.0
    // one for rotation axes
    std::random_device random_device;
    std::mt19937 mt_generator(random_device());
    std::uniform_real_distribution<float> degrees{-360.0f, 360.0f};
    std::uniform_int_distribution<uint> axes{0, 1};

    QMatrix4x4 random_rotation;
    float degree, x, y, z;

    // Create a Lambda function to get timestamp when you want.
    // @parameters : true -> milliseconds | false -> microseconds
    auto timestamp = [](bool mode){
        auto time = Clock::now().time_since_epoch();
        return mode ?
               std::chrono::duration_cast<std::chrono::milliseconds>(time).count():
               std::chrono::duration_cast<std::chrono::microseconds>(time).count();
    };

    // Create a new save directory into the one choosen by user.
    dir = dir + "/" + QString::number(timestamp(1))
              + "_" + QString::number(width)
              + "x" + QString::number(height);

    QDir().mkdir(dir);

    makeCurrent();
    for(int i=0; i < nimages; ++i){
        degree = degrees(mt_generator);
        do {
            x = axes(mt_generator);
            y = axes(mt_generator);
            z = axes(mt_generator);
        } while( x == 0.0f && y == 0.0f && z == 0.0f );

        random_rotation.setToIdentity();
        random_rotation.rotate(degree, x, y, z);

        rotation = random_rotation * rotation;
        update_view(); // apply rotation to the view

        // path/filename of the futur image (directory + timestamp + extension)
        QString filename = dir + "/" + QString::number(timestamp(0)) + extension[format];

        // Save current framebuffer to disk
        QImage image = grabFramebuffer();
        image = image.scaled(width, height, aspect, Qt::SmoothTransformation);
        image.save(filename, nullptr, quality);
        pb->setValue(i); // ++ UI progressBar
    }
    doneCurrent();

    // User Dialog
    int ret = QMessageBox::question(
        this, "View results", "Open directory:\n" + dir,
        QMessageBox::Ok | QMessageBox::Cancel);

    if( ret == QMessageBox::Ok )
        QDesktopServices::openUrl("file://"+dir);
}
