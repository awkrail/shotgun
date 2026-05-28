#include "frame_timecode.h"

#include <cctype>
#include <string>
#include <vector>
#include <cmath>
#include <regex>
#include <fmt/core.h>

FrameTimeCode::FrameTimeCode(const int32_t frame_num, const float fps)
    : framerate{fps}, frame_num{frame_num} {}

FrameTimeCode::FrameTimeCode(const FrameTimeCode& timecode)
    : framerate{timecode.framerate}, frame_num{timecode.frame_num} {}

std::optional<int32_t> FrameTimeCode::parse_timecode_string(const std::string& timecode_str) const {
    auto sep_found = std::find(timecode_str.begin(), timecode_str.end(), ':');
    if (sep_found != timecode_str.end()) {
        std::optional<TimeStamp> timestamp = parse_hrs_mins_secs(timecode_str);
        if (!timestamp)
            return std::nullopt;
        const float secs = calculate_total_seconds(*timestamp);
        return std::round(secs * framerate);
    }
    return std::nullopt;
}

int32_t FrameTimeCode::parse_timecode_number(const int32_t seconds) const {
    return std::round(seconds * framerate);
}

int32_t FrameTimeCode::parse_timecode_number(const float seconds) const {
    return std::round(seconds * framerate);
}

std::string FrameTimeCode::to_string() const {
    float secs = static_cast<float>(frame_num / framerate);
    int32_t hrs = static_cast<int32_t>(secs / frame_timecode::SECONDS_PER_HOUR);
    secs -= (hrs * frame_timecode::SECONDS_PER_HOUR);
    int32_t mins = static_cast<int32_t>(secs / frame_timecode::SECONDS_PER_MINUTE);
    secs = std::max(0.0f, secs - (mins * frame_timecode::SECONDS_PER_MINUTE));
    secs = std::round(secs * 1000) / 1000;
    secs = std::min(frame_timecode::SECONDS_PER_MINUTE, secs);
    if (static_cast<int32_t>(secs) == frame_timecode::SECONDS_PER_MINUTE) {
        secs = 0.0f;
        mins += 1;
        if (mins >= frame_timecode::MINUTES_PER_HOUR) {
            mins = 0;
            hrs += 1;
        }
    }
    return convert_timecode_to_datetime(hrs, mins, secs);
}

std::string FrameTimeCode::to_string_second() const {
    float secs = static_cast<float>(frame_num / framerate);
    return std::to_string(secs);
}

std::optional<TimeStamp> FrameTimeCode::parse_hrs_mins_secs(const std::string& timecode_str) const {
    std::vector<std::string> tokens;
    std::regex delimiter(":");
    auto ite = std::sregex_token_iterator(timecode_str.begin(), timecode_str.end(), delimiter, -1);
    auto end = std::sregex_token_iterator();
    while (ite != end)
        tokens.push_back(*ite++);

    if (tokens.size() != 3)
        return std::nullopt;

    int32_t hour_val = 0;
    int32_t minute_val = 0;
    float second_val = 0.0f;
    try {
        hour_val = std::stoi(tokens[0]);
        minute_val = std::stoi(tokens[1]);
        second_val = std::stof(tokens[2]);
    } catch (const std::exception&) {
        return std::nullopt;
    }

    std::optional<Hour> hour = Hour::create_hour(hour_val);
    std::optional<Minute> minute = Minute::create_minute(minute_val);
    std::optional<Second> second = Second::create_second(second_val);

    if (!hour || !minute || !second)
        return std::nullopt;

    return TimeStamp{ *hour, *minute, *second };
}

bool FrameTimeCode::operator==(const FrameTimeCode& other) const {
    return framerate == other.get_framerate() && frame_num == other.get_frame_num();
}
bool FrameTimeCode::operator!=(const FrameTimeCode& other) const {
    return !(framerate == other.get_framerate() && frame_num == other.get_frame_num());
}
bool FrameTimeCode::operator<(const FrameTimeCode& other) const { return frame_num < other.get_frame_num(); }
bool FrameTimeCode::operator>(const FrameTimeCode& other) const { return frame_num > other.get_frame_num(); }
bool FrameTimeCode::operator<=(const FrameTimeCode& other) const { return frame_num <= other.get_frame_num(); }
bool FrameTimeCode::operator>=(const FrameTimeCode& other) const { return frame_num >= other.get_frame_num(); }

FrameTimeCode FrameTimeCode::operator+(const FrameTimeCode& other) const {
    return FrameTimeCode(frame_num + other.get_frame_num(), framerate);
}

FrameTimeCode FrameTimeCode::operator-(const FrameTimeCode& other) const {
    return FrameTimeCode(std::max(frame_num - other.get_frame_num(), 0), framerate);
}

std::optional<FrameTimeCode> FrameTimeCode::from_timecode_string(const std::string& timecode_str, const float fps) {
    if (fps < frame_timecode::MIN_FPS_DELTA)
        return std::nullopt;
    FrameTimeCode ft(0, fps);
    std::optional<int32_t> frame_num = ft.parse_timecode_string(timecode_str);
    if (!frame_num)
        return std::nullopt;
    return FrameTimeCode(*frame_num, fps);
}

std::optional<FrameTimeCode> FrameTimeCode::from_frame_nums(const int32_t frame_num, const float fps) {
    if (fps < frame_timecode::MIN_FPS_DELTA || frame_num < 0)
        return std::nullopt;
    return FrameTimeCode(frame_num, fps);
}

std::optional<FrameTimeCode> FrameTimeCode::from_seconds(const int32_t seconds, const float fps) {
    if (fps < frame_timecode::MIN_FPS_DELTA || seconds < 0)
        return std::nullopt;
    FrameTimeCode ft(0, fps);
    return FrameTimeCode(ft.parse_timecode_number(seconds), fps);
}

std::optional<FrameTimeCode> FrameTimeCode::from_seconds(const float seconds, const float fps) {
    if (fps < frame_timecode::MIN_FPS_DELTA || seconds < 0.0f)
        return std::nullopt;
    FrameTimeCode ft(0, fps);
    return FrameTimeCode(ft.parse_timecode_number(seconds), fps);
}

std::string convert_timecode_to_datetime(const int32_t hrs, const int32_t mins, const float secs) {
    const int32_t int_sec = static_cast<int32_t>(secs);
    const std::string frac_part_str = std::to_string(secs - int_sec).substr(2, 3);
    return fmt::format("{:02}:{:02}:{:02}.{}", hrs, mins, int_sec, frac_part_str);
}

float calculate_total_seconds(const TimeStamp& timestamp) {
    return (timestamp.hour.get_hour() * frame_timecode::SECONDS_PER_HOUR)
         + (timestamp.minute.get_minute() * frame_timecode::SECONDS_PER_MINUTE)
         + timestamp.second.get_second();
}
