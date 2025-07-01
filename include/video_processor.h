#pragma once

extern "C" {
#include "libavcodec/avcodec.h"
#include <libavutil/frame.h>
#include <libswscale/swscale.h>
}

#include "processor.h"
#include <QImage>

class VideoProcessor : public Processor {
public:
  VideoProcessor();
  ~VideoProcessor();

  int video_init(int width, int height);
  int process_frame(AVFrame *frame, AVCodecContext *codec_ctx) override;
  QImage get_last_image() const;

private:
  int width = 0;
  int height = 0;
  SwsContext *sws_ctx = nullptr;
  QImage last_image;
  AVPixelFormat video_fmt = AV_PIX_FMT_NONE;
};