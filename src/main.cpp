/**
#include "shutoh/video_stream.hpp"
#include "shutoh/frame_timecode.hpp"
#include "shutoh/scene_manager.hpp"
#include "shutoh/frame_timecode_pair.hpp"

#include "command_runner.hpp"
#include "parameters.hpp"
#include "config.hpp"
**/

#include "config.h"
#include "video_stream.h"
#include "scene_manager.h"

#include <iostream>
#include <vector>
#include <cmath>
#include "transnetv2.h"

int main(int argc, char *argv[]) {

    std::optional<Config> opt_cfg = parse_args(argc, argv);
    if (!opt_cfg.has_value())
        return 1;
    Config cfg = opt_cfg.value();

    std::optional<VideoStream> opt_video = VideoStream::initialize_video_stream(cfg.input_path);
    if (!opt_video.has_value())
        return 1;
    VideoStream video = opt_video.value();

    TransNetV2 transnetv2_detector(cfg.model_path);
    SceneManager scene_manager = SceneManager(transnetv2_detector);
    scene_manager.detect_scenes(video);

    return 0;
}
