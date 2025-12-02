cmake_minimum_required(VERSION 3.10)

project(GroupChatOSProject LANGUAGES CXX)

# Use C++17
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# Use pthreads
set(THREADS_PREFER_PTHREAD_FLAG ON)
find_package(Threads REQUIRED)

# Let includes like "shared/protocol.h" work from anywhere
include_directories(${CMAKE_SOURCE_DIR})

# Source files shared between targets
set(SHARED_SOURCES
    shared/cache.cpp
)

# ======================
# Server executable
# ======================
add_executable(server
    server/main.cpp
    server/chat_server.cpp
    server/thread_pool.cpp
    server/group_manager.cpp
    ${SHARED_SOURCES}
)

target_link_libraries(server PRIVATE Threads::Threads)

# ======================
# Client executable
# ======================
add_executable(client
    client/main.cpp
    client/chat_client.cpp
    ${SHARED_SOURCES}
)

target_link_libraries(client PRIVATE Threads::Threads)

# ======================
# Bot test (for performance)
# ======================
add_executable(bot_test
    tests/bot_test.cpp
    ${SHARED_SOURCES}
)

target_link_libraries(bot_test PRIVATE Threads::Threads)




























































- [ ] Final report submitted- [ ] Demo video/screenshots- [ ] Documentation complete- [ ] Test harness passes all tests- [ ] All code compiles and runs## Deliverables- [ ] User manual/guide- [ ] Final report document- [x] Architecture diagrams (placeholders)- [x] Code comments added- [x] README.md completed## Documentation- [ ] Performance benchmarks- [ ] Integration tests passing- [ ] Cache tests passing- [ ] Thread pool tests passing- [x] bol_test.cpp - Test harness created## Testing- [ ] Deadlock Prevention: Resource ordering- [ ] Interrupts: Signal handling- [ ] Virtual Memory: Paging simulation- [ ] IPC: Binary protocol implemented## OS Concepts- [ ] Error handling utilities- [ ] Protocol implementation and validation- [x] utils.h - Utility functions- [x] cache.h - Cache header file- [x] protocol.h - Binary protocol definitions## Shared Components- [ ] Graceful shutdown handling- [ ] Client connection management- [ ] Broadcast message functionality- [x] thread_pool.cpp - Thread pool for clients- [x] cache.cpp - LRU cache for messages- [x] group_manager.cpp - Group chat functionality- [x] chat_server.cpp - Server socket implementation- [x] main.cpp - Server entry point## Server Implementation- [ ] Command parsing (/join, /quit, etc.)- [ ] Connection handling and error recovery- [ ] User interface for sending/receiving messages- [x] audio_client.cpp - Optional audio streaming- [x] chat_client.cpp - Client socket implementation- [x] main.cpp - Client entry point## Client Implementation- [ ] Git repository properly configured- [ ] All source files compile without errors- [x] CMakeLists.txt for building the project- [x] README.md with comprehensive documentation- [x] Project directory structure created## Project Setupcmake_minimum_required(VERSION 3.10)
project(GroupChat)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -pthread -Wall -Wextra")

# Include directories
include_directories(${PROJECT_SOURCE_DIR}/shared)

# Server executable
add_executable(chat_server
    server/main.cpp
    server/chat_server.cpp
    server/group_manager.cpp
    server/cache.cpp
    server/thread_pool.cpp
)

# Client executable
add_executable(chat_client
    client/main.cpp
    client/chat_client.cpp
)

# Optional: Audio client
add_executable(audio_client
    client/audio_client.cpp
)

# Test executable
add_executable(test_harness
    tests/bol_test.cpp
)

# Link pthread
target_link_libraries(chat_server pthread)
target_link_libraries(chat_client pthread)
target_link_libraries(audio_client pthread)
target_link_libraries(test_harness pthread)
