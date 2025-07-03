## player demo

实现音视频同步播放的demo

基于 C++ 和 FFmpeg 的跨平台视频播放器，支持本地视频的加载、解复用、解码与播放。项目采用 Qt 构建图形界面，支持播放、暂停、进度控制等基本操作。通过手动调用 FFmpeg 的核心库（libavformat、libavcodec、libswscale）完成视频流解析与帧渲染，实现 YUV 到 RGB 的格式转换与音视频同步机制。播放器采用多线程架构提升播放流
畅度，使用 CMake 实现跨平台构建