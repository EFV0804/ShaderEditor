message("Generating Event Library")
file(GLOB EVENTS_SRC_FILES ${CMAKE_CURRENT_SOURCE_DIR}/*.cpp ${CMAKE_CURRENT_SOURCE_DIR}/*.h)
add_library(events STATIC ${EVENTS_SRC_FILES})
target_include_directories(events PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})

target_link_libraries(events core glfw)