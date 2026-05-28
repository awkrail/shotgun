#include "scene_manager.h"
#include "video_frame.h"
#include "video_stream.h"
#include "blocking_queue.h"
#include <thread>
#include <algorithm>

constexpr int32_t TRANSNET_WIDTH         = 48;
constexpr int32_t TRANSNET_HEIGHT        = 27;
constexpr int32_t MAX_FRAME_QUEUE_LENGTH = 100;

SceneManager::SceneManager(TransNetV2& detector) : detector{detector} {}

void SceneManager::detect_scenes(VideoStream& video) {
    start     = video.get_start();
    end       = video.get_end();
    framerate = video.get_framerate();

    const int32_t start_frame_num = start.value().get_frame_num();
    video.seek(start_frame_num);

    BlockingQueue<VideoFrame> frame_queue(MAX_FRAME_QUEUE_LENGTH);
    std::thread thread(&SceneManager::decode_thread, this,
                       std::ref(video), std::ref(frame_queue));

    while (true) {
        VideoFrame next_frame = frame_queue.get();
        process_frame(next_frame);
        if (next_frame.is_end_frame)
            break;
    }
    thread.join();
}

std::optional<std::vector<FrameTimeCodePair>> SceneManager::get_scene_list() const {
    if (!start.has_value() || !end.has_value()) {
        std::cerr << "Run detect_scenes() before get_scene_list()." << std::endl;
        return std::nullopt;
    }
    const FrameTimeCode start_pos = start.value();
    const FrameTimeCode last_pos  = end.value();

    std::vector<FrameTimeCode> timecode_cut_list = cutting_list_as_timecodes();
    std::vector<FrameTimeCodePair> scenes;

    if (timecode_cut_list.empty()) {
        scenes.push_back({ start_pos, last_pos });
        return scenes;
    }

    FrameTimeCode last_cut = start_pos;
    for (const auto& cut : timecode_cut_list) {
        scenes.push_back({ last_cut, cut });
        last_cut = cut;
    }
    scenes.push_back({ last_cut, last_pos });
    return scenes;
}

std::vector<FrameTimeCode> SceneManager::cutting_list_as_timecodes() const {
    std::vector<FrameTimeCode> timecode_list;
    for (const auto& cut : cutting_list) {
        const FrameTimeCode timecode = FrameTimeCode::from_frame_nums(cut, framerate).value();
        timecode_list.push_back(timecode);
    }
    return timecode_list;
}

void SceneManager::process_frame(VideoFrame& next_frame) {
    cv::Mat rgb;
    cv::cvtColor(next_frame.frame, rgb, cv::COLOR_BGR2RGB);

    frame_buffer.insert(frame_buffer.end(), rgb.data, rgb.data + TRANSNET_HEIGHT * TRANSNET_WIDTH * 3);
    frame_num_buffer.push_back(next_frame.frame_num);

    if (static_cast<int32_t>(frame_num_buffer.size()) == TRANSNET_WINDOW || next_frame.is_end_frame)
        flush_buffer();
}

void SceneManager::flush_buffer() {
    const int32_t buffer_count = static_cast<int32_t>(frame_num_buffer.size());

    // zero-padding
    const int32_t pad = TRANSNET_WINDOW - buffer_count;
    if (pad > 0)
        frame_buffer.resize(frame_buffer.size() + pad * TRANSNET_HEIGHT * TRANSNET_WIDTH * 3, 0);

    std::vector<float> scores = detector.predict(frame_buffer, TRANSNET_WINDOW);

    for (int32_t i = 0; i < buffer_count; i++) {
        if (scores[i] > 0.5f)
            cutting_list.push_back(frame_num_buffer[i]);
    }

    frame_buffer.clear();
    frame_num_buffer.clear();
}

void SceneManager::decode_thread(VideoStream& video,
                                 BlockingQueue<VideoFrame>& frame_queue) {
    const cv::Size transnet_size(TRANSNET_WIDTH, TRANSNET_HEIGHT);

    while (true) {
        cv::Mat frame;
        if (!video.get_cap().read(frame))
            break;

        cv::Mat resized;
        cv::resize(frame, resized, transnet_size, 0, 0, cv::INTER_LINEAR);

        VideoFrame video_frame{ resized, video.position().get_frame_num(), video.is_end_frame() };
        frame_queue.push(video_frame);

        if (video.is_end_frame())
            break;
    }
}
