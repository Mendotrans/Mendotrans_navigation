cmake_minimum_required(VERSION 3.16)

project(mendotrans_router LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)


# =========================================================
# ================== EXECUTABLE ============================
# =========================================================

add_executable(${PROJECT_NAME}
    ./src/main.cpp
    ./src/navigator/reader.cpp
    ./src/navigator/reader_handler.cpp
)

# Headers for libosmium (non‑system)
target_include_directories(${PROJECT_NAME}
    PRIVATE
        vendor/osmium/include
)


# =========================================================
# ================== DEPENDENCIES ==========================
# =========================================================

find_package(ZLIB REQUIRED)
target_link_libraries(${PROJECT_NAME} PRIVATE ZLIB::ZLIB)

find_package(EXPAT REQUIRED)
target_link_libraries(${PROJECT_NAME} PRIVATE EXPAT::EXPAT)

find_package(BZip2)
if (BZip2_FOUND)
    target_link_libraries(${PROJECT_NAME} PRIVATE BZip2::BZip2)
endif()

# Boost (only if you actually use it)
find_package(Boost 1.55)
if (Boost_FOUND)
    target_link_libraries(${PROJECT_NAME} PRIVATE Boost::boost)
endif()

# Threads
find_package(Threads REQUIRED)
target_link_libraries(${PROJECT_NAME} PRIVATE Threads::Threads)


# =========================================================
# ================== RAYLIB CONFIG =========================
# =========================================================

set(PLATFORM "Desktop")
set(API_VERSION 500)
set(BUILD_SHARED_LIBS ON)
set(SUPPORT_MODULE_RAUDIO ON)

# Make sure raylib builds with the same C++ standard
set(CMAKE_C_STANDARD 11)  # or 99, as raylib uses C
set(CMAKE_POSITION_INDEPENDENT_CODE ON)

add_subdirectory(vendor/raylib raylib-build)

target_link_libraries(${PROJECT_NAME} PRIVATE raylib)
target_include_directories(${PROJECT_NAME} PRIVATE
    vendor/raylib/src)   # makes raylib.h, rlgl.h, etc. visible


# =========================================================
# ================== COMPILER FLAGS ========================
# =========================================================

if (CMAKE_CXX_COMPILER_ID MATCHES "GNU|Clang")
    target_compile_options(${PROJECT_NAME} PRIVATE
        -Wall -Wextra -Wpedantic
    )
endif()

if (CMAKE_C_COMPILER_ID MATCHES "GNU|Clang")
    target_compile_options(${PROJECT_NAME} PRIVATE
        -Wall -Wextra -Wpedantic
    )
endif()
