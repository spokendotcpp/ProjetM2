#include "../include/mainwindow.h"

#include <QMessageBox>
#include <QFileDialog>
#include <QColorDialog>

#include <thread>

MainWindow::MainWindow(QWidget *parent):
    QMainWindow(parent), ui(new Ui::MainWindow())
{
    ui->setupUi(this);
    size_t refresh_rate = size_t(QApplication::primaryScreen()->refreshRate());
    ui->viewer->set_frames_per_second(refresh_rate);

    this->setWindowTitle("Viewer");
    this->resize(1280, 720);
    this->center();

    // TODO: function which setup all default behavior
    ui->cbox_light_fixed->setChecked(true);
    ui->cbox_light_enable->setChecked(true);
    ui->cbox_cull_bfaces->setChecked(false);
    ui->cbox_show_axis->setChecked(true);
    ui->cbox_wireframe->setChecked(false);
    ui->cbox_default_quality->setChecked(true);
    ui->spinbox_quality->setValue(100);
    ui->spinbox_quality->setEnabled(false);

    connect_signals_and_slots();

    // start a Callback timer of (1s, 1000ms)
    startTimer(1000);
}

MainWindow::~MainWindow()
{
    delete ui;
}

void
MainWindow::center()
{
    QSize ssize = QApplication::primaryScreen()->size();
    int w = size().width();
    int h = size().height();

    this->setGeometry(
        (ssize.width() - w)/2,
        (ssize.height() - h)/2,
        w, h
    );
}

void
MainWindow::keyPressEvent(QKeyEvent* event)
{
    if( event->key() == Qt::Key_Escape )
        close();
    else
        ui->viewer->handle_key_events(event);
}

void
MainWindow::timerEvent(QTimerEvent*)
{   
    ui->fps->display(int(ui->viewer->get_computed_frames()));
    ui->viewer->reset_computed_frames();
}

void
MainWindow::connect_signals_and_slots()
{
    // Exit App
    connect(ui->action_quit, &QAction::triggered, this, &QMainWindow::close);  // QUIT APP

    // Load a Mesh File from disk
    connect(ui->action_load_mesh, &QAction::triggered, this, [=](){
        QString file = QFileDialog::getOpenFileName(
            this, "Load a Mesh file", "..", "Mesh files (*.off *.obj)",
            nullptr, QFileDialog::DontUseNativeDialog
        );

        if( !file.isEmpty() ){
            ui->viewer->load_off_file(file.toStdString());
            MeshObject* mesh = ui->viewer->get_mesh();
            ui->statusBar->showMessage(
                "Mesh: " + QString::fromStdString(mesh->name()) +
                " | Faces: " + QString::number(mesh->nb_faces()) +
                " | Vertices: " + QString::number(mesh->nb_vertices())
            );
        }
    });

    // Fixed Light
    connect(ui->cbox_light_fixed, &QCheckBox::toggled, this, [=](bool move){
        ui->viewer->get_light()->update_move_ability(move);
    });

    // Enable Light
    connect(ui->cbox_light_enable, &QCheckBox::toggled, this, [=](bool on){
        Light* l = ui->viewer->get_light();
        on ? l->on() : l->off();
        ui->cbox_light_fixed->setEnabled(on);
    });

    // Cull Back-Faces
    connect(ui->cbox_cull_bfaces, &QCheckBox::toggled, this, [=](bool val){
        ui->viewer->draw_back_faces(!val);
    });

    // Show Axis
    connect(ui->cbox_show_axis, &QCheckBox::toggled, this, [=](bool val){
        ui->viewer->show_axis(val);
    });

    // Display all scene with Wireframe
    connect(ui->cbox_wireframe, &QCheckBox::toggled, this, [=](bool val){
        ui->viewer->draw_wireframe(val);
    });

    // Run Screenshots Sequence
    connect(ui->b_run_sequence, &QPushButton::pressed, this, [=](){
        int quality = -1;
        if( ui->spinbox_quality->isEnabled() )
            quality = ui->spinbox_quality->value();

        ui->viewer->take_screenshots(
            ui->spinbox_image_number->value(),
            quality,
            ui->combobox_image_format->currentIndex(),
            ui->progressBar
        );
    });

    // Framerate of the viewer == Monitor frequency
    // Might have errors if multiple screens
    connect(ui->action_monitor_frequency, &QAction::triggered, this, [=](){
        size_t framerate = size_t(QApplication::primaryScreen()->refreshRate());
        ui->viewer->set_frames_per_second(framerate);
    });

    // User Defined framerate
    connect(ui->action_user_fps, &QAction::triggered, this, [=](){
        bool ok;
        int framerate = QInputDialog::getInt(
            this, "Viewer framerate", "FPS:", 60, 30, 9999, 1, &ok);
        if( ok )
            ui->viewer->set_frames_per_second(size_t(framerate));
    });

    // Reset View Position
    connect(ui->action_reset_view, &QAction::triggered, this, [=](){
        ui->viewer->reset_view();
    });

    // Background Color
    connect(ui->action_bg_color_defined, &QAction::triggered, this, [=](){
        const QColor color = QColorDialog::getColor(
            Qt::white, this, "Choose a Background color",
            QColorDialog::DontUseNativeDialog);

        if( color.isValid() ){
            ui->viewer->set_bg_color(
                color.red()/255.0f,
                color.green()/255.0f,
                color.blue()/255.0f
            );
        }
    });

    connect(ui->action_bg_color_default, &QAction::triggered, this, [=](){
        ui->viewer->use_default_bg_color();
    });


    // Mesh Color
    connect(ui->action_mesh_color_default, &QAction::triggered, this, [=](){
        ui->viewer->update_mesh_color(0.5f, 0.5f, 0.5f);
    });

    connect(ui->action_mesh_color_defined, &QAction::triggered, this, [=](){
        const QColor color = QColorDialog::getColor(
            Qt::gray, this, "Choose a Mesh color",
            QColorDialog::DontUseNativeDialog);

        if( color.isValid() ){
            ui->viewer->update_mesh_color(
                color.red()/255.0f,
                color.green()/255.0f,
                color.blue()/255.0f
            );
        }
    });

    connect(ui->cbox_default_quality, &QCheckBox::toggled, this, [=](bool val){
        ui->spinbox_quality->setEnabled(!val);
    });
}
