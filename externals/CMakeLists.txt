add_subdirectory(glfw)
add_subdirectory(glm)
add_subdirectory(spdlog)
add_subdirectory(tinyOBJ)

message("Generating DearImGUI")
file(GLOB IMGUI_SCR_FILES ${CMAKE_CURRENT_SOURCE_DIR}/imgui/*.cpp ${CMAKE_CURRENT_SOURCE_DIR}/imgui/*.h)

add_library(imgui STATIC)


target_include_directories(imgui PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/imgui)
find_package(Vulkan REQUIRED)
target_link_libraries(imgui Vulkan::Vulkan glfw)
target_sources(imgui PRIVATE
        "${CMAKE_CURRENT_SOURCE_DIR}/imgui/imgui.h"
        "${CMAKE_CURRENT_SOURCE_DIR}/imgui/imgui.cpp"

        "${CMAKE_CURRENT_SOURCE_DIR}/imgui/misc/cpp/imgui_stdlib.h"
        "${CMAKE_CURRENT_SOURCE_DIR}/imgui/misc/cpp/imgui_stdlib.cpp"

        "${CMAKE_CURRENT_SOURCE_DIR}/imgui/imgui_demo.cpp"
        "${CMAKE_CURRENT_SOURCE_DIR}/imgui/imgui_draw.cpp"
        "${CMAKE_CURRENT_SOURCE_DIR}/imgui/imgui_widgets.cpp"
        "${CMAKE_CURRENT_SOURCE_DIR}/imgui/imgui_tables.cpp"

        "${CMAKE_CURRENT_SOURCE_DIR}/imgui/backends/imgui_impl_vulkan.cpp"
        "${CMAKE_CURRENT_SOURCE_DIR}/imgui/backends/imgui_impl_glfw.cpp"
        )