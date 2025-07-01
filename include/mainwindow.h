#pragma once

#include <QMainWindow>
#include <QTimer>
#include <cstddef>

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

private slots:
  void on_btnOpen_clicked();
  void on_btnPlay_clicked();
  void on_btnPause_clicked();

  void update_frame();
};
