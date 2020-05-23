# Minimal CMakeLists.txt for the AWS SDK for C++.
cmake_minimum_required(VERSION 3.2)

# Use shared libraries, which is the default for the AWS C++ SDK build.
option(BUILD_SHARED_LIBS "Build shared libraries" ON)

project(nphw3)

# Locate the AWS SDK for C++ package.
find_package(AWSSDK REQUIRED COMPONENTS s3)

# The executable name and its sourcefiles.
add_executable(client main.cpp net_setting.cpp)

# Build using the C++ standard version 11.
set (CMAKE_CXX_STANDARD 11)

# The libraries used by your executable.
target_link_libraries(client ${AWSSDK_LINK_LIBRARIES})
