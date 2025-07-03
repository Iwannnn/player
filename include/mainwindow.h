#pragma once

#include <qcoreevent.h>

#include <QDebug>
#include <QFileDialog>
#include <QMainWindow>
#include <QMessageBox>
#include <QTimer>

#include "controller.h"
#include "decoder.h"
#include "demuxer.h"

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow {
  Q_OBJECT

 public:
  explicit MainWindow(QWidget *parent = nullptr);
  ~MainWindow();

 private:
  Ui::MainWindow *ui;
  Controller controller;
  QTimer *timer = nullptr;
  QString format_time(double sec);
  void update_frame();
 private slots:
  void on_btnOpen_clicked();
  void on_btnPlay_clicked();
  void on_btnPause_clicked();
  void on_sliderProgress_sliderReleased();
};
