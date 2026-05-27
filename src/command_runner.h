#ifndef COMMAND_RUNNER_H
#define COMMAND_RUNNER_H

#include "shutoh/frame_timecode_pair.hpp"
#include "config.hpp"

#include <string>
#include <vector>
#include <filesystem>

class VideoStream;
template <typename T> struct WithError;
struct Config;

class CommandRunner {
    public:
        explicit CommandRunner(const Config& cfg, const std::vector<FrameTimeCodePair>& scene_list);
        WithError<void> execute(VideoStream& video) const;

    private:
        WithError<void> _list_scenes() const;
        WithError<void> _split_video() const;
        WithError<void> _save_images(VideoStream& video) const;

        const Config cfg_;
        const std::vector<FrameTimeCodePair> scene_list_;
};

#endif
