#pragma once

extern "C" {
#include "libavcodec/avcodec.h"
#include <libavutil/frame.h>
}

class Processor {
public:
  virtual ~Processor() = default;

  virtual int process_frame(AVFrame *frame, AVCodecContext *codec_ctx) = 0;
};