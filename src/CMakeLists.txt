cmake_minimum_required(VERSION 3.23)

#create main exec and include source
add_executable(shaderEditor main.cpp Scene.cpp Scene.h Renderable.cpp Renderable.h)
target_include_directories(shaderEditor PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})

# add renderer folder and link renderer lib with main exec
add_subdirectory(renderer)
add_subdirectory(ui)
target_link_libraries(shaderEditor PRIVATE renderer)
target_link_libraries(shaderEditor PRIVATE ui)




