cmake_minimum_required(VERSION 3.23)

#get all files from renderer
file(GLOB VULKAN_RENDERER_SRC_FILES ${CMAKE_CURRENT_SOURCE_DIR}/*.cpp ${CMAKE_CURRENT_SOURCE_DIR}/*.h)
message("generating vulkan renderer")
#message(${ENGINE_SRC_FILES})

# make renderer library
add_library(vulkan_renderer STATIC ${VULKAN_RENDERER_SRC_FILES})
# include current source to library
target_include_directories(vulkan_renderer PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})

# find Vulkan package and link it with exec
find_package(Vulkan REQUIRED)
message(STATUS ${Vulkan_LIBRARY})

target_link_libraries(vulkan_renderer scene_manager core ui application glm tinyobjloader glfw Vulkan::Vulkan)
