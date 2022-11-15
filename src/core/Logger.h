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

//    static Logger* getLogger(){return instance;}

private:
    static std::shared_ptr<spdlog::logger> rendererLogger;
//    static Logger* instance;

};

#define SD_RENDERER_TRACE(...) Logger::getRendererLogger()->trace(__VA_ARGS__)
#define SD_RENDERER_DEBUG(...) Logger::getRendererLogger()->debug(__VA_ARGS__)
#define SD_RENDERER_INFO(...) Logger::getRendererLogger()->info(__VA_ARGS__)
#define SD_RENDERER_WARN(...) Logger::getRendererLogger()->warn(__VA_ARGS__)
#define SD_RENDERER_ERROR(...) Logger::getRendererLogger()->error(__VA_ARGS__)
#define SD_RENDERER_CRITICAL(...) Logger::getRendererLogger()->critical(__VA_ARGS__)


#define SD_INTERNAL_ASSERT_IMPL(type, check, msg, ...) { if(!(check)) { SD##type##ERROR(msg, __VA_ARGS__); __debugbreak(); } }
#define SD_INTERNAL_ASSERT_WITH_MSG(type, check, ...) SD_INTERNAL_ASSERT_IMPL(type, check, "Assertion failed: {0}", __VA_ARGS__)
#define SD_INTERNAL_ASSERT_NO_MSG(type, check) SD_INTERNAL_ASSERT_IMPL(type, check, "Assertion '{0}' failed at {1}:{2}", check, std::filesystem::path(__FILE__).filename().string(), __LINE__)


#endif //SHADEREDITOR_LOGGER_H
