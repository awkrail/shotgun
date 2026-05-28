#pragma once
#include <string>
#include <vector>
#include <onnxruntime_cxx_api.h>

class TransNetV2 {
public:
    explicit TransNetV2(const std::string& model_path);
    // frames: [T, 27, 48, 3] uint8
    std::vector<float> predict(const std::vector<uint8_t>& frames, int T);

private:
    Ort::Env env;
    Ort::Session session;
    Ort::MemoryInfo memory_info;
};
