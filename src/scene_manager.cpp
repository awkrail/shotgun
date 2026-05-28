#include "scene_manager.h"
#include "video_frame.h"
#include "video_stream.h"
#include "blocking_queue.h"

#include <thread>
#include <algorithm>

constexpr int32_t DEFAULT_MIN_WIDTH = 256;
constexpr int32_t MAX_FRAME_QUEUE_LENGTH = 100;

SceneManager::SceneManager(TransNetV2& detector) : detector_{detector} {}

void SceneManager::detect_scenes(VideoStream& video) {
    start_ = video.get_start();
    end_ = video.get_end();
    framerate_ = video.get_framerate();

    const int32_t start_frame_num = start_.value().get_frame_num();
    video.seek(start_frame_num);

    BlockingQueue<VideoFrame> frame_queue(MAX_FRAME_QUEUE_LENGTH);
    const float downscale_factor = compute_downscale_factor(video.width());
    std::thread thread(&SceneManager::_decode_thread, this, std::ref(video),
                       downscale_factor, std::ref(frame_queue));

    while (true) {
        VideoFrame next_frame = frame_queue.get();
        _process_frame(next_frame);

        if (next_frame.is_end_frame)
            break;
    }
    thread.join();
}

std::optional<std::vector<FrameTimeCodePair>> SceneManager::get_scene_list() const {
    if (!start_.has_value() || !end_.has_value()) {
        std::cerr << "Run detect_scenes() before get_scene_list()." << std::endl;
        return std::nullopt;
    }

    const FrameTimeCode start_pos = start_.value();
    const FrameTimeCode last_pos = end_.value();
    std::vector<FrameTimeCode> timecode_cut_list = _get_cutting_list();
    std::vector<FrameTimeCodePair> scenes;

    if (timecode_cut_list.size() == 0) {
        FrameTimeCodePair scene{ start_pos, last_pos }; 
        scenes.push_back(scene);
        return scenes;
    }

    FrameTimeCode last_cut = start_pos;
    for (auto& cut : timecode_cut_list) {
        FrameTimeCodePair scene{ last_cut, cut };
        scenes.push_back(scene);
        last_cut = cut;
    }

    FrameTimeCodePair scene{ last_cut, last_pos };
    scenes.push_back(scene);    
    return scenes;
}

std::vector<FrameTimeCode> SceneManager::_get_cutting_list() const {
    std::vector<FrameTimeCode> timecode_list;
    for(auto& cut : cutting_list_) {
        const FrameTimeCode timecode = FrameTimeCode::from_frame_nums(cut, framerate_).value();
        timecode_list.push_back(timecode);
    }
    return timecode_list;
}

void SceneManager::_process_frame(VideoFrame& next_frame) {
    /**
    std::optional<int32_t> cuts = detector_.predict(next_frame);
    if (cuts.has_value())
        cutting_list_.push_back(cuts.value());
    **/
}

void SceneManager::_decode_thread(VideoStream& video,
                                  const float downscale_factor, 
                                  BlockingQueue<VideoFrame>& frame_queue) {
    int32_t new_width = video.width();
    int32_t new_height = video.height();
    if (downscale_factor > 1) {
        new_width = std::max(1.0f, std::round(new_width / downscale_factor));
        new_height = std::max(1.0f, std::round(new_height / downscale_factor));
    }
    cv::Size new_size(new_width, new_height);
    
    while (true) {
        cv::Mat frame;

        if(!video.get_cap().read(frame))
            break;

        if (downscale_factor > 1)
            cv::resize(frame, frame, new_size, 0, 0, cv::INTER_LINEAR);

        VideoFrame video_frame {frame, video.position().get_frame_num(), video.is_end_frame()};
        frame_queue.push(video_frame);

        if (video.is_end_frame())
            break;
    }
}

float compute_downscale_factor(const int32_t frame_width) {
    if (frame_width < DEFAULT_MIN_WIDTH)
        return 1.0f;

    return static_cast<float>(frame_width) / DEFAULT_MIN_WIDTH;
}
