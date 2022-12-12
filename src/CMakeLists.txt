cmake_minimum_required(VERSION 3.23)

#create main exec and include source
add_executable(shaderEditor main.cpp Application.cpp Application.h)
target_include_directories(shaderEditor PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})


add_library(application STATIC Application.cpp Application.h events/Inputs.cpp events/Inputs.h events/Event.h events/MouseEvents.h events/KeyboardEvents.h)
# include current source to library
target_include_directories(application PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})
target_link_libraries(application PRIVATE scene_manager)

# add renderer folder and link renderer lib with main exec
add_subdirectory(renderer)
add_subdirectory(ui)
add_subdirectory(core)
add_subdirectory(scene)
target_link_libraries(shaderEditor PRIVATE renderer scene_manager core glfw ui)