//
// Created by elise.vidal on 24/10/2022.
//

#ifndef SHADEREDITOR_LOGGER_H
#define SHADEREDITOR_LOGGER_H
#include "spdlog/spdlog.h"
#include <memory>
#include <filesystem>

#define VERBOSITY spdlog::level::trace

class Logger {
public:
    Logger() = delete;
    ~Logger() = delete;
    Logger(const Logger&) = delete;
    Logger& operator=(const Logger&) = delete;
    static void init();
    inline static std::shared_ptr<spdlog::logger>& getRendererLogger() {return rendererLogger;}
    inline static std::shared_ptr<spdlog::logger>& getCoreLogger() {return coreLogger;}
    inline static std::shared_ptr<spdlog::logger>& getSceneLogger() {return sceneLogger;}
    inline static std::shared_ptr<spdlog::logger>& getUILogger() {return UILogger;}

private:
    static std::shared_ptr<spdlog::logger> rendererLogger;
    static std::shared_ptr<spdlog::logger> coreLogger;
    static std::shared_ptr<spdlog::logger> sceneLogger;
    static std::shared_ptr<spdlog::logger> UILogger;

};

#define SE_RENDERER_TRACE(...) Logger::getRendererLogger()->trace(__VA_ARGS__)
#define SE_RENDERER_DEBUG(...) Logger::getRendererLogger()->debug(__VA_ARGS__)
#define SE_RENDERER_INFO(...) Logger::getRendererLogger()->info(__VA_ARGS__)
#define SE_RENDERER_WARN(...) Logger::getRendererLogger()->warn(__VA_ARGS__)
#define SE_RENDERER_ERROR(...) Logger::getRendererLogger()->error(__VA_ARGS__)
#define SE_RENDERER_CRITICAL(...) Logger::getRendererLogger()->critical(__VA_ARGS__)

#define SE_CORE_TRACE(...) Logger::getCoreLogger()->trace(__VA_ARGS__)
#define SE_CORE_DEBUG(...) Logger::getCoreLogger()->debug(__VA_ARGS__)
#define SE_CORE_INFO(...) Logger::getCoreLogger()->info(__VA_ARGS__)
#define SE_CORE_WARN(...) Logger::getCoreLogger()->warn(__VA_ARGS__)
#define SE_CORE_ERROR(...) Logger::getCoreLogger()->error(__VA_ARGS__)
#define SE_CORE_CRITICAL(...) Logger::getCoreLogger()->critical(__VA_ARGS__)

#define SE_SCENE_TRACE(...) Logger::getSceneLogger()->trace(__VA_ARGS__)
#define SE_SCENE_DEBUG(...) Logger::getSceneLogger()->debug(__VA_ARGS__)
#define SE_SCENE_INFO(...) Logger::getSceneLogger()->info(__VA_ARGS__)
#define SE_SCENE_WARN(...) Logger::getSceneLogger()->warn(__VA_ARGS__)
#define SE_SCENE_ERROR(...) Logger::getSceneLogger()->error(__VA_ARGS__)
#define SE_SCENE_CRITICAL(...) Logger::getSceneLogger()->critical(__VA_ARGS__)

#define SE_UI_TRACE(...) Logger::getUILogger()->trace(__VA_ARGS__)
#define SE_UI_DEBUG(...) Logger::getUILogger()->debug(__VA_ARGS__)
#define SE_UI_INFO(...) Logger::getUILogger()->info(__VA_ARGS__)
#define SE_UI_WARN(...) Logger::getUILogger()->warn(__VA_ARGS__)
#define SE_UI_ERROR(...) Logger::getUILogger()->error(__VA_ARGS__)
#define SE_UI_CRITICAL(...) Logger::getUILogger()->critical(__VASCENE

#define SE_INTERNAL_ASSERT_IMPL(type, check, msg, ...) { if(!(check)) { SE##type##ERROR(msg, __VA_ARGS__); __debugbreak(); } }
#define SE_INTERNAL_ASSERT_WITH_MSG(type, check, ...) SE_INTERNAL_ASSERT_IMPL(type, check, "Assertion failed: {0}", __VA_ARGS__)
#define SE_INTERNAL_ASSERT_NO_MSG(type, check) SE_INTERNAL_ASSERT_IMPL(type, check, "Assertion '{0}' failed at {1}:{2}", check, std::filesystem::path(__FILE__).filename().string(), __LINE__)


#endif //SHADEREDITOR_LOGGER_H
