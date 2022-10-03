cmake_minimum_required(VERSION 3.23)
project(shaderEditor)
set(CMAKE_CXX_STANDARD 17)

add_subdirectory(src)
add_subdirectory(externals)

set(GLFW_BUILD_DOCS OFF CACHE BOOL "" FORCE)
set(GLFW_BUILD_TESTS OFF CACHE BOOL "" FORCE)
set(GLFW_BUILD_EXAMPLES OFF CACHE BOOL "" FORCE)

#add_subdirectory(externals/glfw)
#add_subdirectory(externals/glm)
#add_subdirectory(externals/vma)
