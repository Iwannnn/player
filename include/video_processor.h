#pragma once

extern "C" {
#include <libavcodec/avcodec.h>
#include <libavformat/avformat.h>
#include <libavutil/frame.h>
#include <libswscale/swscale.h>
}

#include <QDebug>
#include <QImage>

#include "processor.h"

class VideoProcessor : public Processor {
 public:
  VideoProcessor();
  ~VideoProcessor();

  int init_video_processor(AVCodecContext *codec_ctx);
  int process_frame(AVFrame *frame, AVCodecContext *codec_ctx,
                    AVStream *stream) override;
  QImage get_last_image() const;
  double get_video_pts() const;

 private:
  int width = 0;
  int height = 0;
  SwsContext *sws_ctx = nullptr;
  QImage last_image;
  AVPixelFormat video_fmt = AV_PIX_FMT_NONE;
  double video_pts = 0.0;
};