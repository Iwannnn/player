#pragma once
extern "C" {
#include <libavcodec/avcodec.h>
#include <libavcodec/packet.h>
#include <libavformat/avformat.h>
}

#include <QString>

class Decoder {
 public:
  Decoder();
  ~Decoder();

  int open_stream(AVFormatContext *fmt_ctx, int stream_index);

  AVCodecContext *get_codec_ctx() const;

  int get_stream_idx() const;

  int send_packet(AVPacket *pkt);

  int reveive_frame(AVFrame *frame);

  int seek(int64_t timestamp, int stream_index);

 private:
  AVCodecContext *codec_ctx = nullptr;
  int stream_idx = -1;
};