#include "mainwindow.h"

#include <QDebug>
#include <QFileDialog>
#include <QMessageBox>
#include <qcoreevent.h>
#include <qtimer.h>

#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent), ui(new Ui::MainWindow) {
  ui->setupUi(this); // 👈 必须调用这个，否则不会触发 uic

  timer = new QTimer(this);
  connect(timer, &QTimer::timeout, this, &MainWindow::update_frame);
}

MainWindow::~MainWindow() { delete ui; }

void MainWindow::on_btnOpen_clicked() {
  QString file_path = QFileDialog::getOpenFileName(
      this, "打开视频文件", ".", "视频文件 (*.mp4 *.mkv *.avi)");
  if (file_path.isEmpty())
    return;

  int ret = 0;

  ret = controller.open(file_path);
  if (ret < 0) {
    QMessageBox::critical(this, "错误", "打开媒体文件失败");
    return;
  }

  qDebug() << "媒体加载完成";
}

void MainWindow::on_btnPlay_clicked() {
  if (!timer->isActive()) {
    timer->start(10); // 40ms ≈ 25 FPS
  }
}

void MainWindow::on_btnPause_clicked() {
  if (timer->isActive()) {
    timer->stop();
  }
}

void MainWindow::update_frame() {
  int ret = 0;
  ret = controller.get_frame();

  if (ret < 0) {
    timer->stop();
    return;
  }
  QImage img = controller.get_cur_image();

  if (!img.isNull()) {
    ui->labelVideo->setPixmap(QPixmap::fromImage(img).scaled(
        ui->labelVideo->size(), Qt::KeepAspectRatio, Qt::SmoothTransformation));
  }
}
