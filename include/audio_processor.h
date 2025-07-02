#pragma once

#include <QDebug>

#include "processor.h"

extern "C" {
#include <SDL2/SDL.h>
#include <libavutil/frame.h>
#include <libswresample/swresample.h>
}

class AudioProcessor : public Processor {
 public:
  AudioProcessor();
  ~AudioProcessor();

  int init_audio_processor(AVCodecContext *codec_ctx);
  int process_frame(AVFrame *frame, AVCodecContext *codec_ctx,
                    AVStream *stream);
  double get_audio_clock() const;
  double get_audio_pts() const;
  int close_audio();

 private:
  SDL_AudioDeviceID dev_id;
  SDL_AudioSpec audio_spec;
  bool is_init = false;

  SwrContext *swr_ctx = nullptr;
  AVChannelLayout in_layout;
  AVChannelLayout out_layout;

  AVSampleFormat target_fmt = AV_SAMPLE_FMT_S16;

  double audio_pts = 0.0;
};