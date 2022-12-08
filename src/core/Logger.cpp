//
// Created by elise.vidal on 24/10/2022.
//

#include "Logger.h"
#include "spdlog/sinks/stdout_color_sinks.h"

std::shared_ptr<spdlog::logger> Logger::rendererLogger;
std::shared_ptr<spdlog::logger> Logger::coreLogger;
std::shared_ptr<spdlog::logger> Logger::sceneLogger;
std::shared_ptr<spdlog::logger> Logger::UILogger;


void Logger::init() {
    spdlog::set_pattern("%^[%T] %n %v%$");
    rendererLogger = spdlog::stdout_color_mt("RENDERER");
    rendererLogger->set_level(spdlog::level::trace);

    coreLogger = spdlog::stdout_color_mt("CORE");
    coreLogger->set_level(spdlog::level::trace);

    sceneLogger = spdlog::stdout_color_mt("SCENE");
    sceneLogger->set_level(spdlog::level::trace);

    UILogger = spdlog::stdout_color_mt("UI");
    UILogger->set_level(spdlog::level::trace);

}