#pragma once

#include "audio_processor.h"
#include "decoder.h"
#include "demuxer.h"
#include "video_processor.h"

extern "C" {
#include <libavcodec/packet.h>
#include <libavformat/avformat.h>
}

#include <QImage>
#include <QString>

class Controller {
 public:
  Controller();
  ~Controller();

  int open(const QString &file_path);
  int decode_frame();
  QImage get_cur_image() const;
  double get_audio_clock() const;
  double get_video_pts() const;
  double get_duration() const;
  int seek_to(double seconds);

 private:
  int open_video_stream(AVFormatContext *fmt_ctx);
  int open_audio_stream(AVFormatContext *fmt_ctx);

  Demuxer demuxer;
  Decoder video_decoder;
  Decoder audio_decoder;
  VideoProcessor video_processor;
  AudioProcessor audio_processor;

  AVPacket *pkt = nullptr;
  AVFrame *frame = nullptr;
};