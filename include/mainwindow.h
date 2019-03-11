#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <iostream> //debug mod

#include <QMainWindow>
#include <QStyle>
#include <QApplication>
#include <QKeyEvent>
#include <QScreen>
#include <QSizePolicy>

#include <QGridLayout>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QGroupBox>
#include <QMenuBar>
#include <QPushButton>

#include "ui_mainwindow.h"

class MainWindow : public QMainWindow
{
    Q_OBJECT
private:
    Ui::MainWindow* ui;

public:
    MainWindow(QWidget *parent=nullptr);
    ~MainWindow() override;

    void center();
    void keyPressEvent(QKeyEvent*) override;
    void timerEvent(QTimerEvent* event) override;

private:
    void connect_signals_and_slots();
};

#endif // MAINWINDOW_H
