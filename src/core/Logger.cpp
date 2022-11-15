//
// Created by elise.vidal on 24/10/2022.
//

#include "Logger.h"
#include "spdlog/sinks/stdout_color_sinks.h"

std::shared_ptr<spdlog::logger> Logger::rendererLogger;

void Logger::init() {
    spdlog::set_pattern("%^[%T] %n %v%$");
    rendererLogger = spdlog::stdout_color_mt("RENDERER");
    rendererLogger->set_level(spdlog::level::trace);
}