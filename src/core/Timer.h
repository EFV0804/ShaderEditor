//
// Created by elise.vidal.
//

#pragma once
#include <thread>
#include <chrono>

#include <cstdint>

class Timer {
public:
    Timer() = default;

    ~Timer() = default;

    Timer(const Timer &) = delete;

    Timer &operator=(const Timer &) = delete;

    float computeDeltaTime();
    void delayTime(uint64_t time);
    void reset();
    float elapsed();
    float elapsedMilliseconds();

private:
//    const static uint32_t FPS = 60;
//    const static uint32_t frameDelay = 1000/FPS;

    float lastFrame;
    std::chrono::time_point<std::chrono::high_resolution_clock> start;
};
