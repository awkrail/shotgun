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

class VideoStream;
template <typename T> class BlockingQueue;
constexpr int32_t TRANSNET_WINDOW = 100;

class SceneManager {
public:
    explicit SceneManager(TransNetV2& detector);
    void detect_scenes(VideoStream& video);
    std::optional<std::vector<FrameTimeCodePair>> get_scene_list() const;

private:
    void process_frame(VideoFrame& next_frame);
    void flush_buffer();
    void decode_thread(VideoStream& video, BlockingQueue<VideoFrame>& frame_queue);
    std::vector<FrameTimeCode> cutting_list_as_timecodes() const;

    TransNetV2& detector;
    float framerate = 0.0f;
    std::optional<FrameTimeCode> start = std::nullopt;
    std::optional<FrameTimeCode> end = std::nullopt;
    std::vector<int32_t> cutting_list;

    std::vector<uint8_t> frame_buffer;
    std::vector<uint32_t> frame_num_buffer;
};

#endif
