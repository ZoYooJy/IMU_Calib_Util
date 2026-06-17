cmake_minimum_required(VERSION 3.10)
project(allan_variance_calib)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Find dependencies
set(EIGEN3_INCLUDE_DIR "/opt/homebrew/Cellar/eigen/3.4.0_1/include/eigen3")
set(YAML_CPP_INCLUDE_DIR "/opt/homebrew/Cellar/yaml-cpp/0.8.0/include")
set(YAML_CPP_LIBRARY "/opt/homebrew/Cellar/yaml-cpp/0.8.0/lib/libyaml-cpp.dylib")

# Compiler flags
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -O3 -Wall -Wuninitialized")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -g3 -fno-omit-frame-pointer")

# Include directories
include_directories(
  include
  ${EIGEN3_INCLUDE_DIR}
  ${YAML_CPP_INCLUDE_DIR}
)

# Library
add_library(${PROJECT_NAME}_lib
    src/yaml_parser.cc
    src/data_reader.cc
    src/imu_meas.cc
    src/allan_variance_computor.cc
)
target_link_libraries(${PROJECT_NAME}_lib ${YAML_CPP_LIBRARY})

# Executable
add_executable(${PROJECT_NAME} src/main.cc)
target_link_libraries(${PROJECT_NAME} ${PROJECT_NAME}_lib ${YAML_CPP_LIBRARY})
