#include "controller.h"

#include <QDebug>
#include <cstdint>

#include "SDL_audio.h"
#include "libavcodec/avcodec.h"
#include "libavformat/avformat.h"
#include "libavformat/avio.h"
#include "libavutil/avutil.h"
Controller::Controller() {
  pkt = av_packet_alloc();
  frame = av_frame_alloc();
}

Controller::~Controller() {
  av_packet_free(&pkt);
  av_frame_free(&frame);
  audio_processor.close_audio();
  demuxer.close();
}

int Controller::open_video_stream(AVFormatContext *fmt_ctx) {
  int v_idx = demuxer.get_video_idx();
  if (v_idx < 0) {
    qDebug() << "controller::open v_idx failed";
    return -1;
  }

  int ret = video_decoder.init_decoder(fmt_ctx, v_idx);
  if (ret < 0) {
    qDebug() << "no video steam";
    return 0;
  }

  AVCodecContext *v_ctx = video_decoder.get_codec_ctx();
  ret = video_processor.init_video_processor(v_ctx);
  if (ret < 0) {
    qDebug() << "controller::open_video_stream video_processor init failed";
    return -1;
  }
  return 0;
}

int Controller::open_audio_stream(AVFormatContext *fmt_ctx) {
  int a_idx = demuxer.get_audio_idx();

  if (a_idx < 0) {
    qDebug() << "no audio stream";
    return 0;
  }

  int ret = audio_decoder.init_decoder(fmt_ctx, a_idx);
  if (ret < 0) {
    qDebug() << "controller::open_audio_stream open_stream failed";
    return -1;
  }

  AVCodecContext *a_ctx = audio_decoder.get_codec_ctx();

  ret = audio_processor.init_audio_processor(a_ctx);
  if (ret < 0) {
    qDebug() << "controller::open_audio_stream audio_processor init failed";
    return -1;
  }

  return 0;
}

int Controller::open(const QString &file_path) {
  int ret = 0;
  ret = demuxer.open_file(file_path);
  if (ret < 0) {
    qDebug() << "controller::open open_file failed";
    return -1;
  }

  AVFormatContext *fmt_ctx = demuxer.get_fmt_ctx();

  open_video_stream(fmt_ctx);
  open_audio_stream(fmt_ctx);

  return 0;
}

int Controller::decode_frame() {
  int ret = 0;
  ret = av_read_frame(demuxer.get_fmt_ctx(), pkt);
  if (ret < 0) {
    qDebug() << "controller::play_one_frame read_frame failed";
    return -1;
  }

  if (pkt->stream_index == video_decoder.get_stream_idx()) {
    if (video_decoder.send_packet(pkt) >= 0) {
      while (video_decoder.reveive_frame(frame) == 0) {
        video_processor.process_frame(frame, video_decoder.get_codec_ctx(),
                                      video_decoder.get_stream());
      }
    }
  } else if (pkt->stream_index == audio_decoder.get_stream_idx()) {
    if (audio_decoder.send_packet(pkt) >= 0) {
      while (audio_decoder.reveive_frame(frame) == 0) {
        audio_processor.process_frame(frame, audio_decoder.get_codec_ctx(),
                                      audio_decoder.get_stream());
      }
    }
  }

  av_packet_unref(pkt);
  return 0;
}

QImage Controller::get_cur_image() const {
  return video_processor.get_last_image();
}

double Controller::get_audio_clock() const {
  return audio_processor.get_audio_clock();
}

double Controller::get_video_pts() const {
  return video_processor.get_video_pts();
}

double Controller::get_duration() const {
  if (!demuxer.get_fmt_ctx()) return 0.0;
  return demuxer.get_fmt_ctx()->duration / (double)AV_TIME_BASE;
}

int Controller::seek_to(double seconds) {
  int64_t ts = static_cast<int64_t>(seconds * AV_TIME_BASE);

  int ret = av_seek_frame(demuxer.get_fmt_ctx(), -1, ts, AVSEEK_FLAG_BACKWARD);
  if (ret < 0) {
    qDebug() << "seek failed";
    return -1;
  }

  // 清除旧数据
  avcodec_flush_buffers(video_decoder.get_codec_ctx());
  avcodec_flush_buffers(audio_decoder.get_codec_ctx());
  SDL_ClearQueuedAudio(audio_processor.get_device_id());

  return 0;
}