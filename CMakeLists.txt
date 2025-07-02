cmake_minimum_required(VERSION 3.16.0)
project(player VERSION 0.1.0 LANGUAGES C CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_AUTOUIC ON)


# package
find_package(Qt5 REQUIRED COMPONENTS Widgets)
find_package(SDL2 REQUIRED)


# output path
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${PROJECT_SOURCE_DIR}/bin)

# FFmpeg include 和 lib 路径
set(FFMPEG_DIR /home/iwan/code/FFmpeg/build64)

# include path
include_directories(${FFMPEG_DIR}/include)
include_directories(${PROJECT_SOURCE_DIR}/include)
include_directories(${SDL2_INCLUDE_DIRS})

# lib path
link_directories(${FFMPEG_DIR}/lib)
set(CMAKE_INSTALL_RPATH "${FFMPEG_DIR}/lib")


add_executable(player 
    src/main.cpp
    src/mainwindow.cpp
    src/decoder.cpp
    src/demuxer.cpp
    src/video_processor.cpp
    src/audio_processor.cpp
    src/controller.cpp
    include/mainwindow.h
    include/decoder.h
    include/demuxer.h
    include/processor.h
    include/video_processor.h
    include/audio_processor.h
    include/controller.h
)


target_link_libraries(player
    Qt5::Widgets
    swscale
    swresample
    avcodec
    avformat
    avfilter
    avutil
    ${SDL2_LIBRARIES}      # ✅ 加入这一行，链接 SDL2
)