#ifndef VIDEO_SPLITTER_H
#define VIDEO_SPLITTER_H

#include "shutoh/frame_timecode_pair.hpp"

#include <string>
#include <vector>
#include <filesystem>

template <typename T> struct WithError;

class VideoSplitter {
    public:
        explicit VideoSplitter(const std::filesystem::path& output_dir, const std::string& output_filename, 
                               const int32_t crf, const std::string& preset, const std::string& ffmpeg_args);
        void split_video(const std::filesystem::path& input_path,
                         const std::vector<FrameTimeCodePair>& scene_list) const;

    private:
        std::vector<std::string> _get_ffmpeg_split_commands(const std::filesystem::path& input_path,
                                                            const std::vector<FrameTimeCodePair>& scene_list) const;
        void _run_ffmpeg_parallel(const std::vector<std::string>& commands) const;

        const std::filesystem::path output_dir_;
        const std::string output_filename_;
        const int32_t crf_;
        const std::string preset_;
        const std::string ffmpeg_args_;
};

void cut_video_with_ffmpeg(const std::string& command);

#endif
