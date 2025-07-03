#include "audio_processor.h"

#include "libavformat/avformat.h"

AudioProcessor::AudioProcessor() : dev_id(0), swr_ctx(nullptr), is_init(false) {
  in_layout.order = AV_CHANNEL_ORDER_UNSPEC;
  out_layout.order = AV_CHANNEL_ORDER_UNSPEC;
}

AudioProcessor::~AudioProcessor() { close_audio(); }

int AudioProcessor::init_audio_processor(AVCodecContext *codec_ctx) {
  if (is_init) {
    qDebug() << "Audio already initialized.";
    return 0;
  }

  int sample_rate = codec_ctx->sample_rate;
  int channels = codec_ctx->ch_layout.nb_channels;
  AVSampleFormat fmt = codec_ctx->sample_fmt;

  SDL_AudioSpec want_spec;
  SDL_zero(want_spec);
  want_spec.freq = sample_rate;
  want_spec.format = AUDIO_S16SYS;
  want_spec.channels = channels;
  want_spec.samples = 1024;
  want_spec.callback = nullptr;

  dev_id = SDL_OpenAudioDevice(nullptr, 0, &want_spec, &audio_spec, 0);

  if (dev_id == 0) {
    qDebug() << "Failed to open audio device" << SDL_GetError();
    return -1;
  }

  // 设置输入和输出声道布局
  av_channel_layout_default(&in_layout, channels);
  av_channel_layout_default(&out_layout, channels);

  int ret = 0;

  ret = swr_alloc_set_opts2(&swr_ctx, &out_layout, target_fmt, sample_rate,
                            &in_layout, fmt, sample_rate, 0, nullptr);
  if (ret < 0) {
    qDebug() << "Failed to init SwrContext.";
    return -1;
  }
  ret = swr_init(swr_ctx);
  if (ret < 0) {
    qDebug() << "Failed to init Swr.";
    return -1;
  }
  SDL_PauseAudioDevice(dev_id, 0);
  is_init = true;

  return 0;
}

int AudioProcessor::process_frame(AVFrame *frame, AVCodecContext *codec_ctx,
                                  AVStream *stream) {
  int ret = 0;

  if (!is_init || !frame || !codec_ctx) {
    qDebug() << "Audio not initialized or invalid frame/context.";
    return -1;
  }

  // 获取输入通道数和样本数
  int in_channels = codec_ctx->ch_layout.nb_channels;
  int in_samples = frame->nb_samples;

  // 分配输出缓冲区
  int out_buf_size = av_samples_get_buffer_size(nullptr, in_channels,
                                                in_samples, target_fmt, 1);
  if (out_buf_size < 0) {
    qDebug() << "Failed to get output buffer size.";
    return -1;
  }

  uint8_t *out_buf = static_cast<uint8_t *>(av_malloc(out_buf_size));
  if (!out_buf) {
    qDebug() << "alloc buf failed";
    return -1;
  }

  // 格式转换（重采样）
  uint8_t *out[] = {out_buf};
  int conv_samples = swr_convert(
      swr_ctx,     //音频重采样上下文
      out,         //输出缓冲区数组，out[i] 是第 i 个通道的指针
      in_samples,  //输出的最大采样数（每个通道）
      const_cast<const uint8_t **>(
          frame->data),  //输入缓冲区数组，指向解码帧每个通道的 PCM 数据
      in_samples  //输入帧中每个通道的采样数
  );
  if (conv_samples < 0) {
    qDebug() << "swr_convert failed.";
    av_free(out_buf);
    return -1;
  }

  if (frame->pts != AV_NOPTS_VALUE) {
    audio_pts = frame->pts * av_q2d(stream->time_base);
  }

  // 推送音频数据到播放队列
  ret = SDL_QueueAudio(dev_id, out_buf, out_buf_size);
  if (ret < 0) {
    qDebug() << "SDL_QueueAudio failed:" << SDL_GetError();
    av_free(out_buf);
    return -1;
  }

  // 释放临时
  av_free(out_buf);
  return 0;
}

int AudioProcessor::close_audio() {
  if (!is_init && dev_id != 0) {
    SDL_CloseAudioDevice(dev_id);
    dev_id = 0;
  }

  if (swr_ctx) {
    swr_free(&swr_ctx);
    swr_ctx = nullptr;
  }

  if (in_layout.order != AV_CHANNEL_ORDER_UNSPEC)
    av_channel_layout_uninit(&in_layout);

  if (out_layout.order != AV_CHANNEL_ORDER_UNSPEC)
    av_channel_layout_uninit(&out_layout);

  is_init = false;

  return 0;
}

double AudioProcessor::get_audio_clock() const {
  if (!is_init) return 0.0;

  int bytes_per_sec = audio_spec.freq * audio_spec.channels *
                      SDL_AUDIO_BITSIZE(audio_spec.format);
  Uint32 queue_bytes = SDL_GetQueuedAudioSize(dev_id);

  double delay = static_cast<double>(queue_bytes) / bytes_per_sec;

  return audio_pts - delay;
}

double AudioProcessor::get_audio_pts() const { return audio_pts; }

int AudioProcessor::get_device_id() const { return dev_id; }