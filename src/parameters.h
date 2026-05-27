#ifndef PARAMETER_H
#define PARAMETER_H

#include "config.hpp"

struct AdaptiveParameters {
    const int32_t window_width = 2;
    const float min_content_val = 15.0f;
};

struct HashParameters {
    const int32_t dct_size = 16;
    const int32_t lowpass = 2;
};

struct HistogramParameters {
    const int32_t bins = 256;
};

struct ThresholdParameters {
    const float fade_bias = 0.0f;
};

struct DetectorParameters {
    const DetectorType detector_type;
    const float threshold;
    const int32_t min_scene_len;
    const AdaptiveParameters adaptive_params;
    const HashParameters hash_params;
    const HistogramParameters histogram_params;
    const ThresholdParameters threshold_params;
};

DetectorParameters initialize_parameters(const Config& cfg);

#endif