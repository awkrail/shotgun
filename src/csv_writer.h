#ifndef CSV_WRITER_H
#define CSV_WRITER_H

#include "shutoh/frame_timecode_pair.hpp"

#include <string>
#include <vector>
#include <filesystem>

template <typename T> struct WithError;

class CSVWriter {
    public:
        explicit CSVWriter(const std::filesystem::path& output_dir, const std::string& output_filename,
                           const bool no_output_file);
        WithError<void> list_scenes(const std::vector<FrameTimeCodePair>& scene_list) const;

    private:
        void _print_scenes(const std::vector<FrameTimeCodePair>& scene_list) const;
        WithError<void> _write_scenes_to_csv(const std::vector<FrameTimeCodePair>& scene_list) const;

        const std::filesystem::path output_dir_;
        const std::string output_filename_;
        const bool no_output_file_;
};

#endif
