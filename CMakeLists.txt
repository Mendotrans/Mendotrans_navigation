cmake_minimum_required(VERSION 3.16)
project(osmium_app LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# ---- Executable ----
add_executable(${PROJECT_NAME}
  ./src/main.cpp
  ./src/navigator/reader.cpp
  ./src/navigator/reader_handler.cpp
)

# ---- libosmium headers ----
target_include_directories(${PROJECT_NAME}
    PRIVATE
        vendor/osmium/include
)

# =========================================================
# ================= DEPENDENCIES ===========================
# =========================================================

find_package(ZLIB REQUIRED)
target_link_libraries(${PROJECT_NAME} PRIVATE ZLIB::ZLIB)

find_package(EXPAT REQUIRED)
target_link_libraries(${PROJECT_NAME} PRIVATE EXPAT::EXPAT)

find_package(BZip2)
if (BZip2_FOUND)
    target_link_libraries(${PROJECT_NAME} PRIVATE BZip2::BZip2)
endif()

# find_package(Protozero REQUIRED)
# target_link_libraries(${PROJECT_NAME} PRIVATE protozero::protozero)

# Boost (optional depending on usage)
find_package(Boost 1.55)
if (Boost_FOUND)
    target_link_libraries(${PROJECT_NAME} PRIVATE Boost::boost)
endif()

# Threads (sometimes required indirectly)
find_package(Threads REQUIRED)
target_link_libraries(${PROJECT_NAME} PRIVATE Threads::Threads)

# =========================================================
# ================= COMPILER FLAGS =========================
# =========================================================

if (CMAKE_CXX_COMPILER_ID MATCHES "GNU|Clang")
    target_compile_options(${PROJECT_NAME} PRIVATE -Wall -Wextra -Wpedantic)
endif()
