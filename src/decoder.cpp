#include "decoder.h"

#include <QDebug>

#include "libavformat/avformat.h"

Decoder::Decoder() {}

Decoder::~Decoder() {
  if (codec_ctx) {
    avcodec_free_context(&codec_ctx);
    codec_ctx = nullptr;
  }
}

int Decoder::init_decoder(AVFormatContext *fmt_ctx, int stream_idx) {
  int ret = 0;

  if (stream_idx < 0) {
    qDebug() << "stream_idx not found";
    return -1;
  }
  stream = fmt_ctx->streams[stream_idx];

  AVCodecParameters *codec_par = fmt_ctx->streams[stream_idx]->codecpar;
  codec = avcodec_find_decoder(codec_par->codec_id);
  if (!codec) {
    qDebug() << "avcodec_find_codec failed";
    return -1;
  }

  codec_ctx = avcodec_alloc_context3(codec);
  if (!codec_ctx) {
    qDebug() << "avcodec_alloc_context3 failed";
    return -1;
  }

  ret = avcodec_parameters_to_context(codec_ctx, codec_par);
  if (ret < 0) {
    qDebug() << "avcodec_parameters_to_context failed";
    return -1;
  }

  ret = avcodec_open2(codec_ctx, codec, nullptr);
  if (ret < 0) {
    qDebug() << "avcodec_open2 failed";
    return -1;
  }

  this->stream_idx = stream_idx;
  qDebug() << "video stream open success, stream index:" << stream_idx;
  return 0;
}

AVCodecContext *Decoder::get_codec_ctx() const { return codec_ctx; }

int Decoder::get_stream_idx() const { return stream_idx; }

int Decoder::send_packet(AVPacket *pkt) {
  return avcodec_send_packet(codec_ctx, pkt);
}

int Decoder::reveive_frame(AVFrame *frame) {
  return avcodec_receive_frame(codec_ctx, frame);
}

int Decoder::seek(int64_t timestamp, int stream_index) {
  return av_seek_frame((AVFormatContext *)codec_ctx->opaque, stream_index,
                       timestamp, AVSEEK_FLAG_BACKWARD);
}

AVStream *Decoder::get_stream() const { return stream; };