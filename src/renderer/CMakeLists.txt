

file(GLOB RENDERER_SRC_FILES ${CMAKE_CURRENT_SOURCE_DIR}/*.cpp ${CMAKE_CURRENT_SOURCE_DIR}/*.h)
message("generating renderer")
add_library(renderer STATIC ${RENDERER_SRC_FILES})
add_subdirectory(vulkan)

target_include_directories(vulkan_renderer PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})

target_link_libraries(renderer vulkan_renderer)

