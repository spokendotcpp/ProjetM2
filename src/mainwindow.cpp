#include "../include/mainwindow.h"

#include <QMessageBox>
#include <QFileDialog>

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
    connect(ui->action_quit, &QAction::triggered, this, &QMainWindow::close);  // QUIT APP

    connect(ui->action_load_mesh, &QAction::triggered, this, [=](){
        QString file = QFileDialog::getOpenFileName(
            this, "Load a Mesh file", "..", "Mesh files (*.off *.obj)",
            nullptr, QFileDialog::DontUseNativeDialog
        );

        if( !file.isEmpty() )
            ui->viewer->load_off_file(file.toStdString());
    });

    // Fixed Light
    connect(ui->cbox_light_fixed, &QCheckBox::toggled, this, [=](bool move){
        ui->viewer->get_light()->update_move_ability(move);
    });

    // Enable Light
    connect(ui->cbox_light_enable, &QCheckBox::toggled, this, [=](bool on){
        Light* l = ui->viewer->get_light();
        on ? l->on() : l->off();
    });

    // Cull Back-Faces
    connect(ui->cbox_cull_bfaces, &QCheckBox::toggled, this, [=](bool val){
        ui->viewer->draw_back_faces(!val);
    });

    connect(ui->b_run_sequence, &QPushButton::pressed, this, [=](){
        ui->viewer->take_screenshots();
    });
}
