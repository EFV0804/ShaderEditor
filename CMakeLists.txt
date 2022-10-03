add_subdirectory(glfw)
add_subdirectory(glm)
add_subdirectory(externals/vma)


message("generating vma")
file(GLOB VMA_SRC_FILES externals/vma/*.h externals/vma/*.hpp)
message(${VMA_SRC_FILES})
add_library(externals/vma INTERFACE ${VMA_SRC_FILES})
target_include_directories(externals/vma INTERFACE externals/vma)
set_target_properties(externals/vma PROPERTIES LINKER_LANGUAGE CXX)