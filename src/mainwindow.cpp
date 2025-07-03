#include "mainwindow.h"

#include <qchar.h>

#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent), ui(new Ui::MainWindow) {
  ui->setupUi(this);  // 👈 必须调用这个，否则不会触发 uic

  timer = new QTimer(this);
  connect(timer, &QTimer::timeout, this, &MainWindow::update_frame);
}

MainWindow::~MainWindow() { delete ui; }

void MainWindow::on_btnOpen_clicked() {
  QString file_path = QFileDialog::getOpenFileName(
      this, "打开视频文件", ".", "视频文件 (*.mp4 *.mkv *.avi)");
  if (file_path.isEmpty()) return;

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
    timer->start(15);  // 40ms ≈ 25 FPS
  }
}

void MainWindow::on_btnPause_clicked() {
  if (timer->isActive()) {
    timer->stop();
  }
}

void MainWindow::update_frame() {
  int ret = 0;
  ret = controller.decode_frame();

  if (ret < 0) {
    timer->stop();
    return;
  }

  double audio_clock = controller.get_audio_clock();
  double video_pts = controller.get_video_pts();

  double threshold = 0.05;

  // 音频在controller中异步完成
  if (video_pts <= audio_clock + threshold) {
    QImage img = controller.get_cur_image();
    if (!img.isNull()) {
      ui->labelVideo->setPixmap(QPixmap::fromImage(img).scaled(
          ui->labelVideo->size(), Qt::KeepAspectRatio,
          Qt::SmoothTransformation));
    }
  }

  double pos = audio_clock;
  double duration = controller.get_duration();

  if (duration > 0) {
    int slider_value = static_cast<int>((pos / duration) * 100);
    ui->sliderProgress->blockSignals(true);
    ui->sliderProgress->setValue(slider_value);
    ui->sliderProgress->blockSignals(false);

    QString time_str =
        QString("%1 / %2").arg(format_time(pos)).arg(format_time(duration));

    ui->lableTime->setText(time_str);
  }
}

QString MainWindow::format_time(double sec) {
  int total = static_cast<int>(sec);
  int min = total / 60;
  int sec_remain = total % 60;
  return QString("%1:%2")
      .arg(min, 2, 10, QLatin1Char('0'))
      .arg(sec_remain, 2, 10, QLatin1Char('0'));
}

void MainWindow::on_sliderProgress_sliderReleased() {
  qDebug() << ui->sliderProgress->value();
  double percent = ui->sliderProgress->value() / 100.0;
  double duration = controller.get_duration();
  double seek_time = percent * duration;

  controller.seek_to(seek_time);
}