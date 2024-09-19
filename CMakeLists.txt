
# find_package(glfw3 3.4 REQUIRED)
# find_package(OpenGL REQUIRED)

# add_executable(main src/main.cpp)
# target_link_libraries(main glfw OpenGL::GL)
# target_include_directories(main PRIVATE include)




cmake_minimum_required(VERSION 3.10)

# Project name and version
project(MyOpenGLProject VERSION 1.0)

# Find OpenGL (native support on both macOS and Windows)
find_package(OpenGL REQUIRED)

# Add GLAD subdirectory
# add_subdirectory(glad/glad)

if (WIN32)  # For Windows
    add_library(glad STATIC glad/glad/src/glad.c)
    target_include_directories(glad PUBLIC glad/glad/include)
endif()

add_subdirectory(glfw) # Adjust the path if necessary, assuming you put the GLFW submodule in 'external/glfw'


# # Find GLFW (use find_package for cross-platform compatibility)
# if (APPLE)
#     # On macOS, find GLFW installed via Homebrew
#     find_package(glfw3 3.4 REQUIRED)
#     include_directories(/usr/local/include)  # Adjust this path if necessary
# elseif(WIN32)
#     # On Windows, either find GLFW via vcpkg or manually installed
#     find_package(glfw3 3.4 REQUIRED PATHS "C:/Libraries/glfw" NO_DEFAULT_PATH) # Replace with the path to your GLFW
# endif()

# Add the executable
#add_executable(main src/main.cpp src/sugma.cpp src/ligma.cpp)
add_executable(main src/sugma.cpp)


# # Platform-specific settings
# if (APPLE)
#     # Additional linking for macOS frameworks

#     target_link_libraries(main "-framework Cocoa" "-framework OpenGL" "-framework IOKit" "-framework CoreVideo")
# elseif(WIN32)
#     # Link against Windows OpenGL
#     target_link_libraries(main PRIVATE opengl32)
# endif()


if (WIN32)  # For Windows
    target_include_directories(main PRIVATE glad/glad/include glfw/include include)
    target_link_libraries(main PRIVATE glfw glad OpenGL::GL opengl32)
elseif(APPLE)  # For macOS
    target_include_directories(main PRIVATE glfw/include include)
    target_link_libraries(main PRIVATE glfw "-framework OpenGL" "-framework Cocoa" "-framework IOKit" "-framework CoreVideo")
endif()
