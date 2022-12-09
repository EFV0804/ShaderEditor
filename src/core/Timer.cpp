//
// Created by elise.vidal.
//

#include "Timer.h"
#include "glfw/glfw3.h"

float Timer::computeDeltaTime() {
    float time = (float)glfwGetTime();
    float deltaTime = time - lastFrame;
    lastFrame = time;
    return deltaTime;
}

void Timer::delayTime(uint64_t time) {
//    auto frameTime = static_cast<uint32_t>(time - frameStart);
//    if (frameTime < frameDelay) {
//        std::chrono::milliseconds timespan(frameTime);
//        std::this_thread::sleep_for(timespan);    }
}
void Timer::reset() {
    start = std::chrono::high_resolution_clock::now();
}
float Timer::elapsed() {
    return std::chrono::duration_cast<std::chrono::nanoseconds>(std::chrono::high_resolution_clock::now() - start).count() * 0.001f * 0.001f * 0.001f;
}

float Timer::elapsedMilliseconds() {
    return elapsed() / 1000.0f;
}