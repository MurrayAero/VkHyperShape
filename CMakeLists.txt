cmake_minimum_required (VERSION 3.21)
project (demo)
if(MSVC)
    add_compile_options(/utf-8)
endif()
set(CMAKE_BUILD_TYPE Debug)
if(NOT CMAKE_BUILD_TYPE)
    SET(CMAKE_BUILD_TYPE Release)
endif()
MESSAGE("Build type: " ${CMAKE_BUILD_TYPE})
# Check C++11 or C++0x support
#include(CheckCXXCompilerFlag)
#CHECK_CXX_COMPILER_FLAG("-std=c++20" COMPILER_SUPPORTS_CXX20)
#CHECK_CXX_COMPILER_FLAG("-std=c++17" COMPILER_SUPPORTS_CXX17)
#CHECK_CXX_COMPILER_FLAG("-std=c++11" COMPILER_SUPPORTS_CXX11)
#CHECK_CXX_COMPILER_FLAG("-std=c++0x" COMPILER_SUPPORTS_CXX0X)
#if(COMPILER_SUPPORTS_CXX20)
#    set(CMAKE_CXX_STANDARD 20)
#elseif(COMPILER_SUPPORTS_CXX17)
#    set(CMAKE_CXX_STANDARD 17)
#else()
#    set(CMAKE_CXX_STANDARD 11)
#endif()

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(Vulkan REQUIRED)

INCLUDE_DIRECTORIES(include/vma)

INCLUDE_DIRECTORIES($ENV{VULKAN_SDK}/Include)
# LINK_DIRECTORIES($ENV{VULKAN_SDK}/Lib)

INCLUDE_DIRECTORIES(${PROJECT_SOURCE_DIR}/include)
# LINK_DIRECTORIES(${PROJECT_SOURCE_DIR}/lib)

add_subdirectory(imgui)
add_subdirectory(vulkan)
add_subdirectory(geometry)
add_subdirectory(imgui_impl)

set(DEMO_INCLUDE_DIRS ${PROJECT_SOURCE_DIR}/include)
message(${DEMO_INCLUDE_DIRS})
set(DEMO_LIB_DIRS ${PROJECT_SOURCE_DIR}/lib)
message(${DEMO_LIB_DIRS})
#可以把所有资源放到一个统一的文件夹内，然后直接复制那个文件夹
file(COPY ${PROJECT_SOURCE_DIR}/shaders DESTINATION ${PROJECT_SOURCE_DIR}/build/)
file(COPY ${PROJECT_SOURCE_DIR}/fonts DESTINATION ${PROJECT_SOURCE_DIR}/build/)

file(GLOB DEMO_CPP ${PROJECT_SOURCE_DIR}/*.cpp)

if(WIN32)
    add_executable(demo WIN32 ${DEMO_CPP})
    target_link_libraries(demo PRIVATE vulkan-1 glfw3)
else()
    add_executable(demo ${DEMO_CPP})
    target_link_libraries(demo PRIVATE pthread vulkan dl glfw3)
endif()
# target_compile_definitions(demo PRIVATE DEBUG)
target_link_directories(demo PRIVATE
    ${PROJECT_SOURCE_DIR}/lib
    "$ENV{VULKAN_SDK}/Lib"
    "${CMAKE_CURRENT_DIR}/imgui/$<CONFIG>"
    "${CMAKE_CURRENT_DIR}/vulkan/$<CONFIG>"
    "${CMAKE_CURRENT_DIR}/geometry/$<CONFIG>"
    "${CMAKE_CURRENT_DIR}/imgui_impl/$<CONFIG>"
)
target_link_libraries(demo PRIVATE 
    imgui_impl_cpp
)
target_link_libraries(demo PRIVATE 
    imgui_cpp
)
target_link_libraries(demo PRIVATE 
    vulkan_cpp
)
target_link_libraries(demo PRIVATE 
    geometry_cpp
)