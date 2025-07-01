#include "controller.h"
#include "libavcodec/packet.h"
#include "libavformat/avformat.h"
#include "libavutil/frame.h"
#include <QDebug>

Controller::Controller() {

  qDebug() << "Controller constructor start";
  pkt = av_packet_alloc();
  frame = av_frame_alloc();
  qDebug() << "Controller constructor end";
}

Controller::~Controller() {
  av_packet_free(&pkt);
  av_frame_free(&frame);
  demuxer.close();
}

int Controller::open(const QString &file_path) {
  int ret = 0;
  ret = demuxer.open_file(file_path);
  if (ret < 0) {
    qDebug() << "controller::open open_file failed";
    return -1;
  }

  int v_idx = demuxer.get_video_idx();
  if (v_idx < 0) {
    qDebug() << "controller::open v_idx failed";
    return -1;
  }

  AVFormatContext *fmt_ctx = demuxer.get_fmt_ctx();

  ret = video_decoder.open_stream(fmt_ctx, v_idx);
  if (ret < 0) {
    qDebug() << "controller::open open_stream failed";
    return -1;
  }

  int width = fmt_ctx->streams[v_idx]->codecpar->width;
  int height = fmt_ctx->streams[v_idx]->codecpar->height;
  video_processor.video_init(width, height);

  return 0;
}

int Controller::get_frame() {
  int ret = 0;
  ret = av_read_frame(demuxer.get_fmt_ctx(), pkt);

  if (ret < 0) {
    qDebug() << "controller::play_one_frame read_frame failed";
    return -1;
  }

  if (pkt->stream_index == video_decoder.get_stream_idx()) {
    video_decoder.send_packet(pkt);

    while (video_decoder.reveive_frame(frame) == 0) {
      video_processor.process_frame(frame, video_decoder.get_codec_ctx());
    }
  }

  av_packet_unref(pkt);
  return 0;
}

QImage Controller::get_cur_image() const {
  return video_processor.get_last_image();
}