cmake_minimum_required(VERSION 3.20)
project(mygl LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

include(FetchContent)

# ----------------------------
# GLFW
# ----------------------------
FetchContent_Declare(
    glfw
    GIT_REPOSITORY https://github.com/glfw/glfw.git
    GIT_TAG 3.4
)

set(GLFW_BUILD_DOCS OFF CACHE BOOL "" FORCE)
set(GLFW_BUILD_TESTS OFF CACHE BOOL "" FORCE)
set(GLFW_BUILD_EXAMPLES OFF CACHE BOOL "" FORCE)

FetchContent_MakeAvailable(glfw)

# ----------------------------
# GLAD
# ----------------------------
FetchContent_Declare(
    glad
    GIT_REPOSITORY https://github.com/Dav1dde/glad.git
    GIT_TAG v0.1.36
)

FetchContent_MakeAvailable(glad)

FetchContent_Declare(
	glm
	GIT_REPOSITORY	https://github.com/g-truc/glm.git
	GIT_TAG 	0af55ccecd98d4e5a8d1fad7de25ba429d60e863 #refs/tags/1.0.1
)
FetchContent_MakeAvailable(glm)

file(GLOB_RECURSE SRC_FILES
    CONFIGURE_DEPENDS
    src/*.cpp
    src/*.h
)

# ----------------------------
# Executable
# ----------------------------
add_executable(minecraft
    ${SRC_FILES}
)

# ----------------------------
# OpenGL + GLFW + GLAD
# ----------------------------
find_package(OpenGL REQUIRED)

target_link_libraries(minecraft
    PRIVATE
        glfw
        glad
        OpenGL::GL
        glm::glm
)

if (MINGW)
    target_link_libraries(minecraft PRIVATE stdc++exp)
endif()
