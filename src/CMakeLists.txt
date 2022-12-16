cmake_minimum_required(VERSION 3.24)
#create main exec and include source
message("Generating executable")

if(COMMAND cmake_policy)
    cmake_policy(SET CMP0003 NEW)
endif(COMMAND cmake_policy)

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
add_subdirectory(events)
target_link_libraries(shaderEditor PRIVATE renderer scene_manager core glfw ui)