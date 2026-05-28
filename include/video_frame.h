#ifndef VIDEO_FRAME_H
#define VIDEO_FRAME_H

#include <opencv2/opencv.hpp>
#include <cstdint>

struct VideoFrame {
    const cv::Mat frame;
    const int32_t frame_num;
    const bool is_end_frame = false;
};

#endif