
find_package(glfw3 3.4 REQUIRED)
find_package(OpenGL REQUIRED)

add_executable(main src/main.cpp)
target_link_libraries(main glfw OpenGL::GL)
target_include_directories(main PRIVATE include)

