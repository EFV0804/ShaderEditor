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

/*!
 *\brief Example function to demonstrate how to use the logging functions.
 */
void Logger::loggingExample(){
// ***************--- LOGGING EXAMPLE ---************//
    int a = 5;
    SE_RENDERER_INFO("Test info message VAR{0}", a);
    SE_RENDERER_WARN("Test warning message");
    SE_RENDERER_ERROR("Test error message");
    SE_RENDERER_DEBUG("Test debug message");
    SE_RENDERER_CRITICAL("Test critical message");
    SE_INTERNAL_ASSERT_WITH_MSG(_RENDERER_, a>5, "a is inferior to 5");
    SE_INTERNAL_ASSERT_NO_MSG(_RENDERER_, a>5);
// **************************************************//
}