cmake_minimum_required(VERSION 3.10)

project(gym-tracker VERSION 1.0)

add_library(compiler_flags INTERFACE)
target_compile_features(compiler_flags INTERFACE cxx_std_20)

add_subdirectory(sqlite3)

add_executable(${PROJECT_NAME} src/main.cc src/data.cc)

target_link_libraries(${PROJECT_NAME} PUBLIC sqlite3 compiler_flags)

target_include_directories(${PROJECT_NAME} PUBLIC include)