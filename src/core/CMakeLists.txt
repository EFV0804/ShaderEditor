message("Renderer::Generating Core Library")
file(GLOB CORE_SRC_FILES ${CMAKE_CURRENT_SOURCE_DIR}/*.cpp ${CMAKE_CURRENT_SOURCE_DIR}/*.h)
add_library(core STATIC ${CORE_SRC_FILES})
target_include_directories(core PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})

target_link_libraries(core spdlog::spdlog glfw)