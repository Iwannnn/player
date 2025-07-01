#include "demuxer.h"

#include <libavformat/avformat.h>

#include <QDebug>

Demuxer::Demuxer() { qDebug() << "Demuxer constructed"; }

Demuxer::~Demuxer() { close(); }

int Demuxer::open_file(const QString &file_path) {
  //防止重复打开
  close();

  int ret = 0;
  // 打开视频
  ret = avformat_open_input(&fmt_ctx, file_path.toStdString().c_str(), nullptr,
                            nullptr);
  if (ret < 0) {
    qDebug() << "avformat_open_input failed";
    return -1;
  }

  // 获取流信息
  ret = avformat_find_stream_info(fmt_ctx, nullptr);
  if (ret < 0) {
    qDebug() << "av_find_stream_info failed";
    return -1;
  }

  // 查找视频流
  video_idx =
      av_find_best_stream(fmt_ctx, AVMEDIA_TYPE_VIDEO, -1, -1, nullptr, 0);
  if (video_idx < 0) {
    qDebug() << "No video stream found";
  }

  // 查找音频流
  audio_idx =
      av_find_best_stream(fmt_ctx, AVMEDIA_TYPE_AUDIO, -1, -1, nullptr, 0);
  if (audio_idx < 0) {
    qDebug() << "No audio stream found";
  }

  qDebug() << "Demux success, video idx:" << video_idx
           << "audio idx:" << audio_idx;
  return 0;
}

void Demuxer::close() {
  if (fmt_ctx) {
    avformat_close_input(&fmt_ctx);
    fmt_ctx = nullptr;
  }
  video_idx = -1;
  audio_idx = -1;
}

AVFormatContext *Demuxer::get_fmt_ctx() const { return fmt_ctx; }

int Demuxer::get_video_idx() const { return video_idx; }

int Demuxer::get_audio_idx() const { return audio_idx; }