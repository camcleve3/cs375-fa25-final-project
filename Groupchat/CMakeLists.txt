cmake_minimum_required(VERSION 3.10)

project(CS375FinalGroupChat LANGUAGES CXX)

# Use C++17
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# Prefer pthreads for std::thread
set(THREADS_PREFER_PTHREAD_FLAG ON)
find_package(Threads REQUIRED)

# So includes like "shared/protocol.h" / "utils.h" work from anywhere
include_directories(
    ${CMAKE_SOURCE_DIR}
)

# Shared source files (non-header-only stuff)
set(SHARED_SOURCES
    shared/cache.cpp
)

# ============================
# Server executable
# ============================
add_executable(server
    server/main.cpp
    server/chat_server.cpp
    server/group_manager.cpp
    server/thread_pool.cpp
    ${SHARED_SOURCES}
)

target_link_libraries(server
    PRIVATE Threads::Threads
)

# ============================
# Client executable
# ============================
add_executable(client
    client/main.cpp
    client/chat_client.cpp
    ${SHARED_SOURCES}
)

target_link_libraries(client
    PRIVATE Threads::Threads
)

# ============================
# Audio Client (optional)
# ============================
add_executable(audio_client
    client/audio_client.cpp
    ${SHARED_SOURCES}
)

target_link_libraries(audio_client
    PRIVATE Threads::Threads
)

# ============================
# Bot test (performance harness)
# ============================
add_executable(bot_test
    tests/bot_test.cpp
    server/thread_pool.cpp
    ${SHARED_SOURCES}
)

target_link_libraries(bot_test
    PRIVATE Threads::Threads
)
