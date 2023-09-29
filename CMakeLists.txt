cmake_minimum_required(VERSION 3.10)

project(LkEngine LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

if(NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE Debug)
endif()

set(OUTPUT_DIR "${CMAKE_BUILD_TYPE}-${CMAKE_SYSTEM_NAME}")

add_compile_options($<$<BOOL:${MSVC}>:/MP>) # MultiProcessorCompile for MSVC

# Configuration specific settings
if(CMAKE_BUILD_TYPE STREQUAL "Debug")
    add_definitions(-DLK_DEBUG)
    set(CMAKE_RUNTIME_OUTPUT_DIRECTORY_DEBUG "${CMAKE_BINARY_DIR}/bin/${OUTPUT_DIR}")
    set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY_DEBUG "${CMAKE_BINARY_DIR}/bin-int/${OUTPUT_DIR}")
endif()

if(CMAKE_BUILD_TYPE STREQUAL "Release" OR CMAKE_BUILD_TYPE STREQUAL "Dist")
    add_definitions(-DLK_${CMAKE_BUILD_TYPE})
    set(CMAKE_RUNTIME_OUTPUT_DIRECTORY_RELEASE "${CMAKE_BINARY_DIR}/bin/${OUTPUT_DIR}")
    set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY_RELEASE "${CMAKE_BINARY_DIR}/bin-int/${OUTPUT_DIR}")
    set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} -O2")
endif()


list(APPEND CMAKE_MODULE_PATH 
"${PROJECT_SOURCE_DIR}/cmake"
"${CMAKE_CURRENT_LIST_DIR}/cmake"
"cmake"
)
find_package(GLFW3 REQUIRED)

add_executable(${PROJECT_NAME} LkEngine/main.cpp)

include(${PROJECT_SOURCE_DIR}/lib/imgui/CMakeLists.txt)


target_include_directories(LkEngine PRIVATE
    ${CMAKE_SOURCE_DIR}/LkEngine/src
    LkEngine
    LkEngine/src
    ${CMAKE_SOURCE_DIR}/lib
    ${CMAKE_SOURCE_DIR}/lib/glm
    ${CMAKE_SOURCE_DIR}/lib/imgui
    ${CMAKE_SOURCE_DIR}/lib/imgui/examples
    ${CMAKE_SOURCE_DIR}/lib/glad
    ${CMAKE_SOURCE_DIR}/lib/glad/include
    ${CMAKE_SOURCE_DIR}/lib/stb_image
    ${CMAKE_SOURCE_DIR}/lib/glfw/include
    ${CMAKE_SOURCE_DIR}/lib/spdlog/include
)

target_link_libraries(${PROJECT_NAME}
    ${GLFW3_LIBRARIES}
    opengl32
)

target_link_directories(${PROJECT_NAME} 
        PRIVATE
    ${CMAKE_SOURCE_DIR}/lib/GLFW/lib
)

target_compile_definitions(${PROJECT_NAME} 
        PRIVATE
    _GLM_WIN32
    GLFW_INCLUDE_NONE
    _CRT_SECURE_NO_WARNINGS
    IMGUI_IMPL_OPENGL_LOADER_GLAD
    IMGUI_DEFINE_MATH_OPERATORS
)

if(WIN32)
    target_compile_definitions(${PROJECT_NAME} 
            PRIVATE
    LK_PLATFORM_WINDOWS
    _IMGUI_WIN32
    _CRT_SECURE_NO_WARNINGS
)
endif()


