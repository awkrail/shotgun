#include "video_stream.h"

#include <string>
#include <filesystem>
#include <cmath>

VideoStream::VideoStream(const std::string& input_path, const float framerate, cv::VideoCapture& cap) 
                        : input_path_{input_path}, framerate_{framerate}, cap_{cap}, 
                          start_{0, framerate_}, end_{0, framerate_} {
    const int32_t total_frame_num = static_cast<int32_t>(cap_.get(cv::CAP_PROP_FRAME_COUNT));
    end_ = FrameTimeCode(total_frame_num, framerate_);
}

FrameTimeCode VideoStream::position() const {
    const int32_t frame_num = static_cast<int32_t>(cap_.get(cv::CAP_PROP_POS_FRAMES));
    if (frame_num < 1)
        return start_;

    std::optional<FrameTimeCode> cur_timecode = FrameTimeCode::from_frame_nums(frame_num - 1, framerate_);
    return *cur_timecode;
}

bool VideoStream::is_end_frame() const {
    return position().get_frame_num() == end_.get_frame_num() - 1;
}

int32_t VideoStream::width() const {
    return static_cast<int32_t>(cap_.get(cv::CAP_PROP_FRAME_WIDTH));
}

int32_t VideoStream::height() const {
    return static_cast<int32_t>(cap_.get(cv::CAP_PROP_FRAME_HEIGHT));
}

bool VideoStream::set_time(const std::optional<std::string>& start, const std::optional<std::string>& end,
                           const std::optional<std::string>& duration) {
    if (start.has_value()) {
        std::optional<FrameTimeCode> start_tc = FrameTimeCode::from_timecode_string(*start, framerate_);
        if (!start_tc)
            return false;
        start_ = *start_tc;
    }

    const int32_t frame_num = static_cast<int32_t>(cap_.get(cv::CAP_PROP_FRAME_COUNT));
    std::optional<FrameTimeCode> video_end = FrameTimeCode::from_frame_nums(frame_num, framerate_);
    if (!video_end)
        return false;

    if (end.has_value()) {
        std::optional<FrameTimeCode> end_tc = FrameTimeCode::from_timecode_string(*end, framerate_);
        if (!end_tc)
            return false;
        end_ = *end_tc <= *video_end ? *end_tc : *video_end;

    } else if (duration.has_value()) {
        std::optional<FrameTimeCode> duration_tc = FrameTimeCode::from_timecode_string(*duration, framerate_);
        if (!duration_tc)
            return false;
        end_ = start_ + *duration_tc <= *video_end ? start_ + *duration_tc : *video_end;
    }

    if (start_ >= end_)
        return false;

    return true;
}

bool VideoStream::seek(const int32_t frame_num) {
    if (frame_num < 0)
        return false;

    if (frame_num >= cap_.get(cv::CAP_PROP_FRAME_COUNT))
        return false;

    if (!cap_.set(cv::CAP_PROP_POS_FRAMES, frame_num))
        return false;

    return true;
}

std::optional<VideoStream> VideoStream::initialize_video_stream(const std::filesystem::path& input_path) {
    if (!std::filesystem::exists(input_path))
        return std::nullopt;

    cv::VideoCapture cap(input_path.string());
    if (!cap.isOpened())
        return std::nullopt;

    const int8_t codec = static_cast<int8_t>(cap.get(cv::CAP_PROP_FOURCC));
    if (std::abs(codec) == 0)
        return std::nullopt;

    float framerate = cap.get(cv::CAP_PROP_FPS);
    if (framerate < frame_timecode::MIN_FPS_DELTA)
        return std::nullopt;

    return VideoStream(input_path.string(), framerate, cap);
}
