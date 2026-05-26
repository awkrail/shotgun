#include "transnetv2.h"

TransNetV2::TransNetV2(const std::string& model_path)
    : env_(ORT_LOGGING_LEVEL_WARNING, "shotgun")
    , session_(env_, model_path.c_str(), Ort::SessionOptions{})
    , memory_info_(Ort::MemoryInfo::CreateCpu(OrtArenaAllocator, OrtMemTypeDefault))
{}

std::vector<float> TransNetV2::predict(const std::vector<uint8_t>& frames, int T) {
    const size_t expected = static_cast<size_t>(T) * 27 * 48 * 3;
    if (frames.size() != expected)
        throw std::runtime_error("frames size mismatch");

    std::array<int64_t, 5> input_shape{1, T, 27, 48, 3};
    auto input_tensor = Ort::Value::CreateTensor<uint8_t>(
        memory_info_,
        const_cast<uint8_t*>(frames.data()),
        frames.size(),
        input_shape.data(),
        input_shape.size()
    );

    const char* input_names[]  = {"frames"};
    const char* output_names[] = {"one_hot"};

    auto outputs = session_.Run(
        Ort::RunOptions{nullptr},
        input_names,
        &input_tensor,
        1,
        output_names,
        1
    );

    float* data = outputs[0].template GetTensorMutableData<float>();
    return std::vector<float>(data, data + T);
}

