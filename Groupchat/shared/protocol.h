// shared/protocol.h
#pragma once

#include <cstdint>
#include <cstring>
#include <string>
#include <chrono>
#include <arpa/inet.h> // htons, htonl, ntohs, ntohl

// Message types
enum MessageType : uint8_t {
    MSG_JOIN        = 1,
    MSG_TEXT        = 2,
    MSG_SWITCH      = 3,
    MSG_LIST_GROUPS = 4
};

// NOTE: raw struct that will be sent over the wire as bytes
struct ChatPacket {
    uint8_t  type;          // MessageType
    uint16_t groupID;       // group id
    uint32_t timestamp;     // unix time
    char     payload[256];  // text content or username
};

// Get current timestamp (seconds since epoch)
inline uint32_t current_timestamp() {
    using namespace std::chrono;
    auto now  = system_clock::now();
    auto secs = duration_cast<seconds>(now.time_since_epoch());
    return static_cast<uint32_t>(secs.count());
}

// Utility to zero-init and set payload from string
inline ChatPacket make_packet(MessageType type,
                              uint16_t groupID,
                              const std::string &text) {
    ChatPacket pkt{};
    pkt.type      = type;
    pkt.groupID   = groupID;
    pkt.timestamp = current_timestamp();
    std::memset(pkt.payload, 0, sizeof(pkt.payload));
    std::strncpy(pkt.payload, text.c_str(), sizeof(pkt.payload) - 1);
    return pkt;
}

// Convert host-order packet to network-order fields for sending
inline ChatPacket to_network(const ChatPacket &hostPkt) {
    ChatPacket net = hostPkt;
    net.groupID    = htons(hostPkt.groupID);
    net.timestamp  = htonl(hostPkt.timestamp);
    return net;
}

// Convert received network-order packet to host-order
inline ChatPacket to_host(const ChatPacket &netPkt) {
    ChatPacket host = netPkt;
    host.groupID    = ntohs(netPkt.groupID);
    host.timestamp  = ntohl(netPkt.timestamp);
    return host;
}

