/**
#include "shutoh/video_stream.hpp"
#include "shutoh/frame_timecode.hpp"
#include "shutoh/scene_manager.hpp"
#include "shutoh/frame_timecode_pair.hpp"

#include "command_runner.hpp"
#include "parameters.hpp"
#include "config.hpp"
**/

#include <iostream>
#include <vector>
#include <cmath>
#include "transnetv2.h"

int main(int argc, char *argv[]) {
    if (argc < 2) {
        std::cerr << "usage: shotgun <model.onnx>\n";
        return 1;
    }

    TransNetV2 model(argv[1]);

    const int T = 100;
    std::vector<uint8_t> frames(T * 27 * 48 * 3, 0);

    auto logits = model.predict(frames, T);

    std::cout << "output shape: [" << logits.size() << "]\n";
    for (int i = 0; i < 5; i++) {
        float prob = 1.0f / (1.0f + std::exp(-logits[i]));
        std::cout << "frame " << i << ": logit=" << logits[i]
                  << " prob=" << prob << "\n";
    }

    return 0;
    /**
    const WithError<Config> opt_cfg = parse_args(argc, argv);
    if (opt_cfg.has_error()) {
        opt_cfg.error.show_error_msg();
        return 1;
    }
    Config cfg = opt_cfg.value();

    WithError<VideoStream> opt_video = VideoStream::initialize_video_stream(cfg.input_path);
    if (opt_video.has_error()) {
        opt_video.error.show_error_msg();
        return 1;
    }
    VideoStream video = opt_video.value();    
    WithError<void> settime_err = video.set_time(cfg.start, cfg.end, cfg.duration);
    if (settime_err.has_error()) {
        settime_err.error.show_error_msg();
        return 1;
    }

    const DetectorParameters params = initialize_parameters(cfg);
    auto detector = _select_detector(params);
    
    SceneManager scene_manager = SceneManager(detector);
    scene_manager.detect_scenes(video);
    WithError<std::vector<FrameTimeCodePair>> opt_scene_list = scene_manager.get_scene_list();
    if (opt_scene_list.has_error()) {
        opt_scene_list.error.show_error_msg();
        return 1;
    }
    std::vector<FrameTimeCodePair> scene_list = opt_scene_list.value();
    
    CommandRunner command_runner = CommandRunner(cfg, scene_list);
    WithError<void> err = command_runner.execute(video);
    if (err.has_error()) {
        err.error.show_error_msg();
        return 1;
    }
    return 0;
    **/
}
