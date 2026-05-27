# Shotgun

## Now under construction, the following README is outdated. Stay tuned!!

Shutoh (手刀, meaning karate chop in Japanese) is a fast and efficient scene detection tool implemented in C++20.
Inspired by [PySceneDetect](https://github.com/Breakthrough/PySceneDetect), Shutoh aims to provide a powerful and flexible alternative with enhanced performance. The key features of Shutoh is threefold:
- **Fast**: Shutoh leverages the speed of C++ for faster processing compared to PySceneDetect.
- **Compatibility**: Designed to produce outputs comparable to PySceneDetect as much as possible.
- **Flexible**: Supports both rule-based and machine-learning-based approaches.

## Installation
Ensure that FFmpeg, OpenCV, and CMake are installed.
```
sudo apt install libopencv-dev ffmpeg cmake
```
To build `shutoh` with `cmake`, run:
```shell
cmake -S . -B build
cmake --build build
sudo cmake --install build
shutoh --help
```
If you got an error of loading shared libraries: libshutoh.so, please add PATH to `$LD_LIBRARY_PATH` (Linux):
```
export LD_LIBRARY_PATH=/usr/local/lib:$LD_LIBRARY_PATH
```

## Quick Start (Command Line)
I focus on three main commands: `split-video`, `list-scenes`, and `save-images`. 
Save scene information (e.g., frame numbers and time) as csv file:
```
shutoh -i video/input.mp4 -c list-scenes
```
Split an input video into shots and save them as videos:
```
shutoh -i video/input.mp4 -c split-video
```
Save some frames from each shot:
```
shutoh -i video/input.mp4 -c list-scenes
```
See [documentation](docs/cli_documentation.md) for details.

## API
### Python
Install Shutoh by running:
```
pip install git+https://github.com/awkrail/shutoh.git
```
To detect scenes with `ContentDetector()`, run the following code:
```python
from libshutoh import detect, ContentDetector
detector = ContentDetector.initialize_detector(threshold=27, min_scene_len=15)
scenes = detect('video/input.mp4', detector)
for i, scene in enumerate(scene_list):
    print('Scene %2d: Start %s / Frame %d, End %s / Frame %d' % (
        i+1,
        scene[0].get_timecode(), scene[0].get_frames(),
        scene[1].get_timecode(), scene[1].get_frames(),))
```
Using `scenedetect.split_video_ffmpeg`, we can also split the video into each scene:
```python
from libshutoh import detect, ContentDetector
from scenedetect import split_video_ffmpeg
detector = ContentDetector.initialize_detector(threshold=27, min_scene_len=15)
scenes = detect('video/input.mp4', detector)
split_video_ffmpeg('vide/input.mp4', scenes, show_progress=True)
```
If you want to use other detectors, replace the detector with the new one:
```python
from libshutoh import AdaptiveDetector
detector = AdaptiveDetector.initialize_detector()
```
**Detector list:** AdaptiveDetector, ContentDetector, HashDetector, HistogramDetector, ThresholdDetector

### C++
The simpletest code is as follow:
```cpp
#include "shutoh/video_stream.hpp"
#include "shutoh/frame_timecode.hpp"
#include "shutoh/scene_manager.hpp"
#include "shutoh/frame_timecode_pair.hpp"
#include "shutoh/detector/content_detector.hpp"

int main() {
    VideoStream video = VideoStream::initialize_video_stream("video/input.mp4").value();
    auto detector = ContentDetector::initialize_detector();
    SceneManager scene_manager = SceneManager(detector);
    scene_manager.detect_scenes(video);
    std::vector<FrameTimeCodePair> scene_list = scene_manager.get_scene_list().value();

    for (auto& scene : scene_list) {
        const FrameTimeCode start = std::get<0>(scene);
        const FrameTimeCode end = std::get<1>(scene);
        std::cout << "Start Time: " << start.to_string() << " Frame: " << start.get_frame_num()
        << " / End Time: " << end.to_string() << " Frame: " << end.get_frame_num() << std::endl; 
    }
}
```
To compile the code, run the following g++ command (replace `-I` and `-L` with your directory):
```
g++ -std=c++20 -I/path/to/shutoh/include -I/usr/include/opencv4 main.cpp -L/path/to/shutoh/build -lopencv_core -lopencv_videoio -lfmt -Wl,-rpath,/path/to/build -lshutoh_lib
```
Or you can use for example FetchContent:
```cmake
include(FetchContent)
FetchContent_Declare(
  shutoh
  GIT_REPOSITORY https://github.com/awkrail/shutoh.git
  GIT_TAG main
)
FetchContent_MakeAvailable(shutoh)
target_link_libraries(YOUR_LIBRARY PUBLIC shutoh OTHER_LIBRARIES)
```

## Contribution
Pull requests are welcome. For major changes, please open an issue first to discuss what you would like to change.

## LICENSE
MIT License

## Contact
Taichi Nishimura ([taichitary@gmail.com](taichitary@gmail.com)) 
Copyright (C) 2024-2025 Taichi Nishimura. 
All rights reserved.
