#ifndef FRAME_TIMECODE_H
#define FRAME_TIMECODE_H

#include <string>
#include <cstdint>
#include <optional>
#include <iostream>

constexpr int32_t HOUR_MAX = 10;

class Hour {
    public:
        explicit Hour(const int32_t hour) : hour_{hour} {}
        static std::optional<Hour> create_hour(const int32_t hour) {
            if (hour < 0 || hour >= HOUR_MAX) {
                std::cerr << "[Error] Hour should be 0 < x < 10, but got " << std::to_string(hour) << std::endl;
                return std::nullopt;
            }
            return Hour(hour);
        }
        const int32_t get_hour() const { return hour_; }
    
    private:
        const int32_t hour_;
};

class Minute {
    public:
        explicit Minute(const int32_t minute) : minute_{minute} {}
        static std::optional<Minute> create_minute(const int32_t minute) {
            if (minute < 0 || minute >= 60) {
                std::cerr << "[Error] Minute should be 0 < x < 60, but got " << std::to_string(minute) << std::endl;
                return std::nullopt;
            }
            return Minute(minute);
        }
        const int32_t get_minute() const { return minute_; }

    private:
        const int32_t minute_;
};

class Second {
    public:
        explicit Second(const float second) : second_{second} {}
        static std::optional<Second> create_second(const float second) {
            if (second < 0 || second >= 60) {
                std::cerr << "[Error] Second should be 0 < x < 60, but got " << std::to_string(second) << std::endl;
                return std::nullopt;
            }
            return Second(second);
        }
        const float get_second() const { return second_; }

    private:
        const float second_;
};

struct TimeStamp {
    Hour hour;
    Minute minute;
    Second second;
};

class FrameTimeCode {
    public:
        explicit FrameTimeCode(const int32_t frame_num, const float fps);
        FrameTimeCode(const FrameTimeCode& timecode);

        float get_framerate() const { return framerate_; }
        int32_t get_frame_num() const { return frame_num_; }

        std::optional<int32_t> parse_timecode_string(const std::string& timecode_str) const;
        int32_t parse_timecode_number(const int32_t seconds) const;
        int32_t parse_timecode_number(const float seconds) const;
        std::string to_string() const;
        std::string to_string_second() const;

        bool operator==(const FrameTimeCode& other) const;
        bool operator!=(const FrameTimeCode& other) const;
        bool operator<(const FrameTimeCode& other) const;
        bool operator>(const FrameTimeCode& other) const;
        bool operator<=(const FrameTimeCode& other) const;
        bool operator>=(const FrameTimeCode& other) const;

        FrameTimeCode operator+(const FrameTimeCode& other) const;
        FrameTimeCode operator-(const FrameTimeCode& other) const;

        static std::optional<FrameTimeCode> from_timecode_string(const std::string& timecode_str, const float fps);
        static std::optional<FrameTimeCode> from_frame_nums(const int32_t frame_num, const float fps);
        static std::optional<FrameTimeCode> from_seconds(const int32_t seconds, const float fps);
        static std::optional<FrameTimeCode> from_seconds(const float seconds, const float fps);

    private:
        std::optional<TimeStamp> _parse_hrs_mins_secs_to_second(const std::string& timecode_str) const;
        float framerate_;
        int32_t frame_num_;
};

namespace frame_timecode {

constexpr float MIN_FPS_DELTA = 1.0 / 100000;
constexpr float _SECONDS_PER_MINUTE = 60.0;
constexpr float _SECONDS_PER_HOUR = 60.0 * _SECONDS_PER_MINUTE;
constexpr float _MINUTES_PER_HOUR = 60.0;

}

float calculate_total_seconds(const TimeStamp& timestamp);
std::string convert_timecode_to_datetime(const int32_t hrs, const int32_t mins, const float secs);

#endif
