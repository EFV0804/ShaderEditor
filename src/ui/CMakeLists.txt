file(GLOB UI_SRC ${CMAKE_CURRENT_SOURCE_DIR}/*.cpp ${CMAKE_CURRENT_SOURCE_DIR}/*.h)
message("generating ui")
add_subdirectory(editor)

# make renderer library
add_library(ui ${UI_SRC})
target_include_directories(ui PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})
target_link_libraries(ui renderer glfw core application editor)
