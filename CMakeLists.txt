cmake_minimum_required(VERSION 3.16)
project(osmium_app LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# ---- Executable ----
add_executable(${PROJECT_NAME} main.cpp)

# ---- Include libosmium headers ----
target_include_directories(${PROJECT_NAME}
    PRIVATE
        vendor/osmium/include
)

# ---- Compiler warnings (recommended) ----
if (CMAKE_CXX_COMPILER_ID MATCHES "GNU|Clang")
    target_compile_options(osmium_app PRIVATE -Wall -Wextra -Wpedantic)
endif()
