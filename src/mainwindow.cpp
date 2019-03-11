#include "../include/mainwindow.h"

#include <QMessageBox>
#include <QFileDialog>

MainWindow::MainWindow(QWidget *parent):
    QMainWindow(parent), ui(new Ui::MainWindow())
{
    ui->setupUi(this);
    ui->viewer->set_frames_per_second(9999);

    this->setWindowTitle("Viewer");
    this->resize(1280, 720);
    this->center();

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

    // Render Flat or Smooth
    connect(ui->action_flat, &QAction::triggered, ui->viewer, [this]{ ui->viewer->smooth_render(false); });
    connect(ui->action_smooth, &QAction::triggered, ui->viewer, [this]{ ui->viewer->smooth_render(true); });

    connect(ui->actionLoadMesh, &QAction::triggered, ui->viewer, [this]{
        QString file = QFileDialog::getOpenFileName(
            this, "Load a Mesh file", "..", "Mesh files (*.off, *.obj)",
            nullptr, QFileDialog::DontUseNativeDialog
        );

        if( !file.isEmpty() ){
       	    ui->viewer->load_off_file(file.toStdString());
        }
    });
}
