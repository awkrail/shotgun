#ifndef IMAGE_EXTRACTOR_H
#define IMAGE_EXTRACTOR_H

#include "shutoh/frame_timecode_pair.hpp"

#include <string>
#include <vector>
#include <filesystem>
#include <optional>

class VideoStream;
template <typename T> struct WithError;
enum class ResizeMode;

struct StartEndSplitIndex {
    const int32_t start; /* start index of the scene */
    const int32_t end; /* end index of the scene */
};

struct SceneFrameIndex {
    const int32_t scene_ind; /* scene index in the video */
    const int32_t frame_ind_in_scene; /* frame index in the scene (relative) */
    const int32_t frame_ind_in_video; /* frame index in the video (absolute) */
};

enum class ResizeMode {
    ORIGINAL,
    RESIZE_TARGET,
    RESIZE_SCALE,
};

struct ResizedSize {
    const int32_t width;
    const int32_t height;
    const float scale;
    const ResizeMode resize_mode;
};

class ImageExtractor {
    public:
        explicit ImageExtractor(const std::filesystem::path& output_dir, const std::string output_filename, const int32_t num_images, 
                                const int32_t frame_margin, const std::string& format, const int32_t quality, const std::optional<int32_t> width,
                                const std::optional<int32_t> height, const std::optional<float> scale);
        
        WithError<void> save_images(VideoStream& video, const std::vector<FrameTimeCodePair>& scene_list) const;

    private:
        template <typename ResizeFunc>
        WithError<void> _save_scene_frames(VideoStream& video, const std::vector<SceneFrameIndex>& scene_frame_list,
                                           ResizeFunc resize_frame) const;

        std::vector<SceneFrameIndex> _get_selected_frame_ind_from_scenes(const std::vector<FrameTimeCodePair>& scene_list) const;

        std::vector<int32_t> _get_frame_inds(const FrameTimeCode& start, 
                                             const FrameTimeCode& end) const;
        
        std::vector<StartEndSplitIndex> _construct_splits(const FrameTimeCode& start,
                                                          const FrameTimeCode& end) const;

        ResizedSize _get_size(const VideoStream& video) const;
        std::pair<int32_t, int32_t> _calculate_resized_size(const int32_t original_width, const int32_t original_height) const;

        const std::filesystem::path output_dir_;
        const std::string output_filename_;
        const int32_t num_images_;
        const int32_t frame_margin_;
        std::string format_;
        const int32_t quality_;
        std::optional<int32_t> width_;
        std::optional<int32_t> height_;
        std::optional<float> scale_;
        std::vector<int32_t> params_;
};

#endif
