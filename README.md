# CS375 - Final Project: Multi-Group Chat System

## Overview
A fully-featured C++ group chat application implementing multi-threaded client-server architecture with advanced OS concepts including scheduling, virtual memory simulation, caching with TTL, and performance monitoring.

## Features
- ✅ **Multi-Group Chat**: Create and switch between multiple chat groups
- ✅ **Username Support**: Messages display sender username and timestamp
- ✅ **Message History**: New joiners receive recent cached messages
- ✅ **Priority Scheduling**: ThreadPool with SJF (Shortest Job First) task scheduling
- ✅ **Virtual Memory Simulation**: Paging system with page fault handling
- ✅ **Smart Caching**: LRU cache with TTL (5-minute expiration)
- ✅ **Performance Metrics**: Real-time tracking of message rate, cache hits, thread usage
- ✅ **Signal Handling**: Graceful shutdown with SIGINT/SIGTERM
- ✅ **Binary Protocol**: Efficient structured packet communication

## Project Structure
```
GroupChat/
├── client/
│   ├── main.cpp                    # Client entry point
│   ├── chat_client.cpp/.h          # Client implementation with username
│   └── audio_client.cpp            # Audio streaming client (optional)
├── server/
│   ├── main.cpp                    # Server with signal handlers
│   ├── chat_server.cpp/.h          # Server with history & list groups
│   ├── group_manager.cpp/.h        # Multi-group management
│   ├── thread_pool.cpp/.h          # Priority-based thread pool (SJF)
├── shared/
│   ├── protocol.h                  # Binary protocol with sender info
│   ├── cache.h/.cpp                # TTL-based circular cache
│   ├── metrics.h                   # Performance monitoring
│   ├── virtual_memory.h            # Virtual memory simulator with paging
│   └── utils.h                     # Utility functions
├── tests/
│   └── bot_test.cpp                # Test harness
├── logs/
│   ├── chat_log.txt                # Timestamped message logs
│   └── performance.txt             # Performance metrics
├── diagrams/
│   ├── thread_architecture.png     # Thread architecture
│   ├── message_flow.png            # Message flow
│   ├── virtual_memory.png          # Memory management
│   └── task_queue.png              # Task scheduling
├── README.md                       # This file
├── CMakeLists.txt                  # Build configuration
├── final_report.docx               # Comprehensive project report
└── IMPLEMENTATION_SUMMARY.md       # Technical implementation details
```

## Key OS Concepts Implemented

### 1. IPC (Inter-Process Communication)
- **Binary Protocol**: Custom `ChatPacket` struct with type, groupID, senderID, timestamp, username, and payload
- **TCP Sockets**: Reliable client-server communication
- **Network Byte Order**: Proper serialization with htons/htonl

### 2. Scheduling
- **Priority Queue**: Min-heap based task queue for SJF scheduling
- **ThreadPool**: Fixed-size worker threads with priority-based task assignment
- **Configurable Priorities**: Tasks can be assigned different priorities

### 3. Virtual Memory
- **Paging System**: 16 pages × 256 bytes = 4KB memory pool
- **Page Table**: Virtual-to-physical address translation
- **Page Faults**: FIFO eviction when memory is full
- **Metrics Tracking**: Page fault counter integrated

### 4. Synchronization
- **Mutexes**: Protect shared data structures (GroupManager, cache)
- **Condition Variables**: ThreadPool worker coordination
- **Deadlock Prevention**: Consistent lock ordering

### 5. Caching
- **LRU Circular Buffer**: Recent message storage per group
- **TTL Expiration**: Messages expire after 300 seconds
- **Cache Metrics**: Hit/miss tracking for performance analysis

### 6. Signal Handling (Interrupts)
- **SIGINT/SIGTERM**: Graceful shutdown handlers
- **Resource Cleanup**: Proper socket closure and metric logging
### 7. File I/O
- **Chat Logging**: All messages logged with timestamp and sender info
- **Performance Logging**: Metrics saved on shutdown
- **Persistent Storage**: File-based log management

### 8. Performance Monitoring
- **Message Rate**: Tracks messages per second
- **Cache Analytics**: Hit rate and miss tracking
- **Thread Utilization**: Active thread monitoring
- **Memory Metrics**: Page fault counting

## Building the Project

### Prerequisites
- C++17 compatible compiler (g++ or clang++)
- CMake 3.10 or higher
- Linux/Unix environment
- POSIX threads (pthread)

### Build Steps
```bash
# Navigate to project root
cd /workspaces/cs375-fa25-final-project

# Create and enter build directory
mkdir -p build && cd build

# Configure with CMake
cmake ..

# Build all targets
cmake --build .

# This creates executables:
#   - server (chat server)
#   - client (chat client)
#   - bot_test (test harness)
#   - audio_client (optional audio client)
```

### Running the System

#### Start the Server
```bash
cd build
./server [port]

# Example:
./server 8080
```

#### Start Clients
```bash
cd build
./client [server_ip] [port]

# Example (same machine):
./client 127.0.0.1 8080

# Example (remote):
./client 192.168.1.100 8080
```

#### Run Tests
```bash
cd build
./bot_test
```

## Usage Guide

### Client Commands
- **At startup**: Enter your username when prompted
- `/switch <group_number>` - Switch to a different group (e.g., `/switch 2`)
- `/list` - Display all active groups with members
- `/quit` - Gracefully exit the client
- **Any other text** - Send as a message to your current group

### Message Display Format
```
[G<group>][<username>] <message>

Examples:
[G1][Alice] Hello everyone!
[G2][Bob] Anyone here?
```

### Server Shutdown
- Press `Ctrl+C` to trigger graceful shutdown
- Performance metrics automatically logged to `Groupchat/logs/performance.txt`

## Log Files

### Chat Log (`Groupchat/logs/chat_log.txt`)
Format: `<timestamp> | group <groupID> | <username>: <message>`
```
1733097654 | group 1 | Alice: Hello!
1733097660 | group 1 | Bob: Hi Alice!
1733097665 | group 2 | Charlie: Testing group 2
```

### Performance Log (`Groupchat/logs/performance.txt`)
Generated on server shutdown with metrics:
- Uptime (seconds)
- Total messages sent
- Message rate (msg/sec)
- Cache hit/miss statistics
- Cache hit rate percentage
- Active thread count
- Page fault count

## Implementation Highlights

### Priority-Based ThreadPool
- Uses `std::priority_queue` for SJF scheduling
- Tasks assigned priorities (lower = higher priority)
- Worker threads pick highest priority task first
- Demonstrates OS scheduling algorithms

### Virtual Memory Simulator
- 16 physical pages × 256 bytes each = 4KB total
- Page table maps virtual to physical pages
- Page faults trigger FIFO eviction
- Allocate/read/write/deallocate operations
- Integrated metrics tracking

### TTL-Based Cache
- Circular buffer with time-to-live (5-minute default)
- Automatic expiration of stale messages
- Per-group message history storage
- Thread-safe with mutex protection
- Cache hit/miss tracking for analytics

### Binary Protocol
- Custom `ChatPacket` structure (fixed size)
- Fields: type, groupID, senderID, timestamp, senderName, payload
- Network byte order conversion (htons/htonl)
- Message types: MSG_JOIN, MSG_TEXT, MSG_SWITCH, MSG_LIST_GROUPS

### Group Management
- Multi-group support with per-group member tracking
- Dynamic group creation on first join
- Group switching with message history replay
- Active group listing
- Thread-safe operations

## Testing

### Test Harness (`bot_test.cpp`)
Validates:
- ThreadPool task execution and priority handling
- Cache add/retrieve operations with TTL
- Utility function correctness
- Basic integration scenarios

### Manual Testing
1. Start server in one terminal
2. Start 2+ clients in separate terminals
3. Test scenarios:
   - Send messages within same group
   - Switch groups and verify message routing
   - List active groups
   - Check message history on new joins
   - Test graceful shutdown with Ctrl+C

## Performance Characteristics

### Scalability
- ThreadPool handles multiple concurrent clients
- O(1) cache operations
- Efficient group broadcasting
- Low-latency message delivery

### Resource Usage
- Fixed memory pool (virtual memory simulation)
- Configurable cache size per group
- Thread pool size tunable (default: 4 threads)
- Minimal CPU overhead per message

## Architecture Overview

### Server Architecture
```
┌─────────────────────────────────────┐
│         ChatServer (main)           │
│  - Signal handlers (SIGINT/SIGTERM) │
│  - Socket setup and accept loop     │
└───────────────┬─────────────────────┘
                │
        ┌───────▼────────┐
        │   ThreadPool   │
        │  (Priority SJF)│
        └───────┬────────┘
                │
        ┌───────▼────────────┐
        │  handle_client()   │
        │  - Receive packets │
        │  - Route messages  │
        └───────┬────────────┘
                │
    ┌───────────┴───────────┐
    │                       │
┌───▼──────────┐   ┌───────▼─────────┐
│ GroupManager │   │ VirtualMemory   │
│ - Groups     │   │ - Paging        │
│ - Cache      │   │ - Page faults   │
│ - Broadcast  │   │                 │
└──────────────┘   └─────────────────┘
```

### Client Architecture
```
┌──────────────────────────┐
│   ChatClient (main)      │
│   - Connect to server    │
│   - Username input       │
└────────┬─────────────────┘
         │
    ┌────┴────┐
    │         │
┌───▼────┐ ┌─▼─────────┐
│ Send   │ │ Receive   │
│ Thread │ │ Thread    │
│        │ │           │
│ Input  │ │ Display   │
│ Loop   │ │ Messages  │
└────────┘ └───────────┘
```

## Project Documentation

### Design Document
See `final_report.docx` for:
- Complete architecture design
- OS concept implementations
- Algorithm analysis
- Performance benchmarks
- Design decisions and trade-offs

### Implementation Summary
See `IMPLEMENTATION_SUMMARY.md` for:
- Technical implementation details
- Feature-by-feature breakdown
- Requirements coverage checklist
- Build and test instructions

## Contributors
Cameron Cleveland and Bryce Coleman
CS375 - Operating Systems Final Project
Fall 2025

## License
Educational use only - Academic project for CS375