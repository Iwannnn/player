#pragma once

extern "C" {
#include <libavformat/avformat.h>
}

#include <QString>

class Demuxer {
 public:
  Demuxer();
  ~Demuxer();

  int open_file(const QString& file_path);

  void close();

  AVFormatContext* get_fmt_ctx() const;

  int get_video_idx() const;

  int get_audio_idx() const;

 private:
  AVFormatContext* fmt_ctx = nullptr;

  int video_idx = -1;

  int audio_idx = -1;
};