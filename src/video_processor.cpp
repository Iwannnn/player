#include "video_processor.h"

#include "libavformat/avformat.h"

VideoProcessor::VideoProcessor() {}

VideoProcessor::~VideoProcessor() {
  if (sws_ctx) sws_freeContext(sws_ctx);
}

int VideoProcessor::init_video_processor(AVCodecContext *codec_ctx) {
  if (!codec_ctx) return -1;

  width = codec_ctx->width;
  height = codec_ctx->height;

  return 0;
}

int VideoProcessor::process_frame(AVFrame *frame, AVCodecContext *codec_ctx,
                                  AVStream *stream) {
  // 检查输入帧是否有效
  if (!frame || !frame->data[0]) return -1;

  int width = codec_ctx->width;
  int height = codec_ctx->height;
  AVPixelFormat fmt = static_cast<AVPixelFormat>(frame->format);

  if (!sws_ctx || frame->format != video_fmt) {
    // 判断是否需要重新创建转换上下文
    if (sws_ctx) {
      sws_freeContext(sws_ctx);
      sws_ctx = nullptr;
    }

    video_fmt = static_cast<AVPixelFormat>(frame->format);
    sws_ctx = sws_getContext(frame->width, frame->height, video_fmt, width,
                             height, AV_PIX_FMT_RGB24, SWS_BILINEAR, nullptr,
                             nullptr, nullptr);
    if (!sws_ctx) return -2;
    last_image = QImage(width, height, QImage::Format_RGB888);
  }

  // 准备目标缓冲区
  uint8_t *dst_data[1] = {last_image.bits()};  //返回图像首地址
  int dst_lineszie[1] = {static_cast<int>(
      last_image.bytesPerLine())};  //返回每行像素所占字节数（用于行对齐）

  // 执行图像格式转换
  int ret = sws_scale(sws_ctx,
                      frame->data,  // 输入图像数据指针数组（如 YUV）
                      frame->linesize,  // 输入每行步长（每个平面）
                      0,                // 从第几行开始
                      height,           // 处理多少行
                      dst_data,         // 输出图像数据指针数组
                      dst_lineszie      // 输出每行步长
  );

  if (frame->pts != AV_NOPTS_VALUE) {
    video_pts = frame->pts * av_q2d(stream->time_base);
  }
  return (ret > 0) ? 0 : -3;
}

QImage VideoProcessor::get_last_image() const { return last_image; }

double VideoProcessor::get_video_pts() const { return video_pts; }