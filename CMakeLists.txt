cmake_minimum_required(VERSION 3.10)

project(GroupChatOSProject LANGUAGES CXX)

# Use C++17
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# Use pthreads
set(THREADS_PREFER_PTHREAD_FLAG ON)
find_package(Threads REQUIRED)

# Let includes like "Groupchat/shared/protocol.h" work from anywhere
include_directories(${CMAKE_SOURCE_DIR}/Groupchat)

# Source files shared between targets
set(SHARED_SOURCES
    Groupchat/shared/cache.cpp
)

# ======================
# Server executable
# ======================
add_executable(server
    Groupchat/server/main.cpp
    Groupchat/server/chat_server.cpp
    Groupchat/server/thread_pool.cpp
    Groupchat/server/group_manager.cpp
    ${SHARED_SOURCES}
)

target_link_libraries(server PRIVATE Threads::Threads)

# ======================
# Client executable
# ======================
add_executable(client
    Groupchat/client/main.cpp
    Groupchat/client/chat_client.cpp
    ${SHARED_SOURCES}
)

target_link_libraries(client PRIVATE Threads::Threads)

# ======================
# Bot test (for performance)
# ======================
add_executable(bot_test
    Groupchat/tests/bot_test.cpp
    ${SHARED_SOURCES}
)

target_link_libraries(bot_test PRIVATE Threads::Threads)
