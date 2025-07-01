#pragma once

#include "decoder.h"
#include "demuxer.h"
#include "video_processor.h"

extern "C" {
#include "libavcodec/packet.h"
#include <libavformat/avformat.h>
}

#include <QImage>
#include <QString>

class Controller {
public:
  Controller();
  ~Controller();

  int open(const QString &file_path);
  int get_frame();
  QImage get_cur_image() const;

private:
  Demuxer demuxer;
  Decoder video_decoder;
  VideoProcessor video_processor;

  AVPacket *pkt = nullptr;
  AVFrame *frame = nullptr;
};