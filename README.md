# CS375 - Final Project: Group Chat System

## Overview
This is a C++ group chat application implementing multi-threaded client-server architecture with support for text messaging, audio streaming, and group management.

## Project Structure
```
GroupChat/
├── client/
│   ├── main.cpp                    # Client entry point
│   ├── chat_client.cpp             # Client implementation
│   └── audio_client.cpp (optional) # Audio streaming client
├── server/
│   ├── main.cpp                    # Server entry point
│   ├── chat_server.cpp             # Server implementation
│   ├── group_manager.cpp           # Group management
│   ├── cache.cpp                   # LRU cache implementation
│   └── thread_pool.cpp             # Thread pool for handling clients
├── shared/
│   ├── protocol.h                  # Binary protocol definitions
│   ├── cache.h                     # Cache header
│   └── utils.h                     # Utility functions
├── tests/
│   └── bol_test.cpp                # Test harness
├── logs/
│   └── chat_log.txt                # Chat logs
├── diagrams/
│   ├── thread_architecture.png     # Thread architecture diagram
│   ├── message_flow.png            # Message flow diagram
│   ├── virtual_memory.png          # Virtual memory simulation
│   └── task_queue.png              # Task queue diagram
├── README.md                       # This file
├── CMakeLists.txt                  # Build configuration
└── final_report.docx               # Project report
```

## Key OS Concepts Covered

### IPC (Inter-Process Communication)
- Binary Protocol: Structured packet definition instead of plain text
- TCP sockets for reliable communication
- Message serialization and deserialization

### Virtual Memory
- Simulated paging with fixed memory pool
- Memory management in cache implementation

### Interrupts
- Signal handling for client disconnections
- Graceful shutdown on SIGINT/SIGTERM

### Deadlock Prevention
- Resource ordering with banker's algorithm
- Lock hierarchy in multi-threaded operations

## Features
- Multi-client support using thread pool
- Text messaging with broadcast functionality
- Group chat management (create, join, leave groups)
- LRU cache for message history
- Binary protocol for efficient communication
- Audio streaming support (optional)
- Comprehensive error handling and logging

## Building the Project

### Prerequisites
- C++17 compatible compiler (g++ or clang++)
- CMake 3.10 or higher
- Linux/Unix environment

### Compilation
```bash
# Compile server
g++ -std=c++17 -pthread -o server server/main.cpp -I./shared

# Compile client
g++ -std=c++17 -pthread -o client client/main.cpp -I./shared

# Compile tests
g++ -std=c++17 -pthread -o test tests/bol_test.cpp -I./shared
```

### Running

#### Start the Server
```bash
./server [port]
# Example: ./server 8080
```

#### Start the Client
```bash
./client [server_ip] [port]
# Example: ./client 127.0.0.1 8080
```

#### Run Tests
```bash
./test
```

## Usage

### Client Commands
- `/join <username>` - Join the chat with a username
- `/create <group>` - Create a new group
- `/join_group <group>` - Join an existing group
- `/leave_group <group>` - Leave a group
- `/list` - List all groups
- `/quit` - Exit the application
- Any other text is sent as a message

## Implementation Details

### Thread Pool
- Fixed size thread pool for handling multiple clients
- Task queue with condition variables for synchronization
- Automatic work distribution among worker threads

### LRU Cache
- Template-based implementation
- Thread-safe operations with mutex protection
- O(1) get and put operations

### Protocol
- Binary message format with headers
- Message types: TEXT, JOIN, LEAVE, CREATE_GROUP, etc.
- Fixed header size with variable payload

### Group Management
- Create and manage chat groups
- Group membership tracking
- Targeted message broadcasting to group members

## Testing
The test harness (`bol_test.cpp`) validates:
- Thread pool functionality
- LRU cache operations
- Utility functions
- Basic integration scenarios

## Documentation
For detailed design decisions, implementation details, and performance analysis, see `final_report.docx`.

## Authors
CS375 - Operating Systems Final Project

## License
Educational use only