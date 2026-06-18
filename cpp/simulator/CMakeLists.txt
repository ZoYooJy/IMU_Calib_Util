cmake_minimum_required(VERSION 3.16)
project(imu_simulator LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# ── Eigen ──────────────────────────────────────────────────
find_package(Eigen3 QUIET)
if(Eigen3_FOUND)
    message(STATUS "[Eigen3] found via find_package: ${Eigen3_DIR}")
else()
    message(STATUS "[Eigen3] NOT found via find_package, using manual path")
    set(EIGEN3_INCLUDE_DIR "/opt/homebrew/Cellar/eigen/3.4.0_1/include/eigen3")
endif()

# ── yaml-cpp ──────────────────────────────────────────────
find_package(yaml-cpp QUIET)
if(yaml-cpp_FOUND)
    message(STATUS "[yaml-cpp] found via find_package: ${yaml-cpp_DIR}")
else()
    message(STATUS "[yaml-cpp] NOT found via find_package, using manual path")
    set(YAML_CPP_INCLUDE_DIR "/opt/homebrew/Cellar/yaml-cpp/0.8.0/include")
    set(YAML_CPP_LIBRARY "/opt/homebrew/Cellar/yaml-cpp/0.8.0/lib/libyaml-cpp.dylib")
endif()

# ── 可执行目标 ────────────────────────────────────────────
add_executable(imu_simulator
    src/imu_simulator.cc
    src/main.cc
)

target_include_directories(imu_simulator PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/include
)

# 链接 Eigen
if(Eigen3_FOUND)
    target_link_libraries(imu_simulator PRIVATE Eigen3::Eigen)
else()
    target_include_directories(imu_simulator PRIVATE ${EIGEN3_INCLUDE_DIR})
endif()

# 链接 yaml-cpp
if(yaml-cpp_FOUND)
    target_link_libraries(imu_simulator PRIVATE yaml-cpp::yaml-cpp)
else()
    target_include_directories(imu_simulator PRIVATE ${YAML_CPP_INCLUDE_DIR})
    target_link_libraries(imu_simulator PRIVATE ${YAML_CPP_LIBRARY})
endif()
