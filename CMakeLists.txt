add_subdirectory(glfw)
add_subdirectory(glm)
add_subdirectory(vma)


message("generating vma")
file(GLOB VMA_SRC_FILES ${CMAKE_CURRENT_SOURCE_DIR}/vma/*.h ${CMAKE_CURRENT_SOURCE_DIR}/vma/*.hpp)
message(${VMA_SRC_FILES})
add_library(vma INTERFACE ${VMA_SRC_FILES})
target_include_directories(vma INTERFACE vma)
set_target_properties(vma PROPERTIES LINKER_LANGUAGE CXX)