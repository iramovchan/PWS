cmake_minimum_required(VERSION 3.10)

# Project name and version
project(MyOpenGLProject VERSION 1.0)

# Find OpenGL (native support on both macOS and Windows)
find_package(OpenGL REQUIRED)

if (WIN32)  # For Windows
    add_library(glad STATIC glad/glad/src/glad.c)
    target_include_directories(glad PUBLIC glad/glad/include)
endif()

add_subdirectory(glfw) # Adjust the path if necessary, assuming you put the GLFW submodule in 'external/glfw'
add_subdirectory(assimp)


# Add the executable
# add_executable(main src/main.cpp src/sugma.cpp src/ligma.cpp)
add_executable(main 
    src/main.cpp
    # src/classes/stb_image.cpp
    )

set (source_dir "${PROJECT_SOURCE_DIR}/src/")


if (WIN32)
    target_include_directories(main PRIVATE glad/glad/include glfw/include external/assimp/include glm-0.9.8.5/glm include)
    target_link_libraries(main PRIVATE glfw glad OpenGL::GL opengl32 assimp)
elseif(APPLE)
    target_include_directories(main PRIVATE glfw/include assimp/include include)
    target_link_libraries(main PRIVATE glfw "-framework OpenGL" "-framework Cocoa" "-framework IOKit" "-framework CoreVideo" assimp)
endif()

# if (WIN32)
#     target_include_directories(main PRIVATE glad/glad/include glfw/include external/assimp/include glm-0.9.8.5/glm include)
#     target_link_libraries(main PRIVATE glfw glad OpenGL::GL opengl32 assimp)
# elseif(APPLE)
#     target_include_directories(main PRIVATE glfw/include include)
#     target_link_libraries(main PRIVATE glfw "-framework OpenGL" "-framework Cocoa" "-framework IOKit" "-framework CoreVideo")
# endif()