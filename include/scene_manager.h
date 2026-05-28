#ifndef SCENE_MANAGER_H
#define SCENE_MANAGER_H

#include "transnetv2.h"
#include "video_frame.h"
#include "frame_timecode.h"
#include "frame_timecode_pair.h"

#include <opencv2/opencv.hpp>
#include <vector>
#include <cstdint>
#include <optional>
#include <memory>

class VideoStream;
template <typename T> class BlockingQueue;

class SceneManager {
    public:
        explicit SceneManager(TransNetV2& detector);
        void detect_scenes(VideoStream& video);
        std::optional<std::vector<FrameTimeCodePair>> get_scene_list() const;

    private:
        void _process_frame(VideoFrame& next_frame);
        void _decode_thread(VideoStream& video, BlockingQueue<VideoFrame>& frame_queue);
        std::vector<FrameTimeCode> _get_cutting_list() const;

        cv::Mat previous_frame_;
        std::vector<int32_t> cutting_list_;
        TransNetV2& detector_;
        float framerate_ = 0.0f;
        std::optional<FrameTimeCode> start_ = std::nullopt;
        std::optional<FrameTimeCode> end_ = std::nullopt;
};

#endif
