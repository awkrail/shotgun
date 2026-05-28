#ifndef VIDEO_STREAM_H
#define VIDEO_STREAM_H

#include "frame_timecode.h"
#include <opencv2/opencv.hpp>
#include <string>
#include <filesystem>
#include <optional>

class VideoStream {
public:
    explicit VideoStream(const std::string& input_path, const float framerate, cv::VideoCapture& cap);
    FrameTimeCode position() const;
    bool is_end_frame() const;
    int32_t width() const;
    int32_t height() const;
    bool set_time(const std::optional<std::string>& start, const std::optional<std::string>& end,
                  const std::optional<std::string>& duration);
    cv::VideoCapture& get_cap() { return cap; }
    float get_framerate() const { return framerate; }
    const FrameTimeCode& get_start() const { return start; }
    const FrameTimeCode& get_end() const { return end; }
    bool seek(const int32_t frame_num);
    static std::optional<VideoStream> initialize_video_stream(const std::filesystem::path& input_path);

private:
    const std::string input_path;
    const float framerate;
    cv::VideoCapture cap;
    FrameTimeCode start;
    FrameTimeCode end;
};

#endif
