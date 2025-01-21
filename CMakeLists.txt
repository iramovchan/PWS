cmake_minimum_required(VERSION 3.10)

# Project name and version
project(MyOpenGLProject VERSION 1.0)

# Find OpenGL
find_package(OpenGL REQUIRED)

set(CMAKE_CXX_STANDARD 14)  # Or 14, 17, etc.
set(CMAKE_CXX_STANDARD_REQUIRED ON)

if (WIN32)  # For Windows
    add_library(glad STATIC glad/glad/src/glad.c)
    target_include_directories(glad PUBLIC glad/glad/include)
endif()

# Add GLFW and Assimp as subdirectories
add_subdirectory(glfw)
add_subdirectory(external/assimp)
add_subdirectory(external/freetype)

# Add the executable
add_executable(main 
    

    src/mainCopy.cpp
    # src/text.cpp
    src/classes/stb_image.cpp
)

# Set the source directory (if needed for other configurations)
set (source_dir "${PROJECT_SOURCE_DIR}/src/")

# Include directories and link libraries for Windows
# Include directories and link libraries for Windows
if (WIN32)
    target_include_directories(main PRIVATE 
        glad/glad/include 
        glfw/include 
        external/freetype/include 
        external/assimp/include 
        glm-0.9.8.5/glm 
        include
    )
    target_link_libraries(main PRIVATE 
        glfw 
        glad 
        OpenGL::GL 
        opengl32 
        assimp
        freetype
    )
    
# Include directories and link libraries for macOS
elseif(APPLE)
    target_include_directories(main PRIVATE 
        glfw/include 
        external/assimp/include 
        include
    )
    target_link_libraries(main PRIVATE 
        glfw 
        "-framework OpenGL" 
        "-framework Cocoa" 
        "-framework IOKit" 
        "-framework CoreVideo" 
        assimp
    )
endif()
