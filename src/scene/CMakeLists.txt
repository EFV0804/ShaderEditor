cmake_minimum_required(VERSION 3.23)

#get all files from renderer
file(GLOB SCENE_SRC_FILES ${CMAKE_CURRENT_SOURCE_DIR}/*.cpp ${CMAKE_CURRENT_SOURCE_DIR}/*.h)
message("generating scene manager")
#message(${ENGINE_SRC_FILES})

# make renderer library
add_library(scene_manager STATIC ${SCENE_SRC_FILES})
# include current source to library
target_include_directories(scene_manager PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})

target_link_libraries(scene_manager renderer vulkan_renderer core glm glfw)