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
    uint16_t senderID;      // client socket/ID
    uint32_t timestamp;     // unix time
    char     senderName[32];// username
    char     payload[224];  // text content (reduced to fit senderName)
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
                              const std::string &text,
                              uint16_t senderID = 0,
                              const std::string &senderName = "") {
    ChatPacket pkt{};
    pkt.type      = type;
    pkt.groupID   = groupID;
    pkt.senderID  = senderID;
    pkt.timestamp = current_timestamp();
    std::memset(pkt.senderName, 0, sizeof(pkt.senderName));
    std::memset(pkt.payload, 0, sizeof(pkt.payload));
    std::strncpy(pkt.senderName, senderName.c_str(), sizeof(pkt.senderName) - 1);
    std::strncpy(pkt.payload, text.c_str(), sizeof(pkt.payload) - 1);
    return pkt;
}

// Convert host-order packet to network-order fields for sending
inline ChatPacket to_network(const ChatPacket &hostPkt) {
    ChatPacket net = hostPkt;
    net.groupID    = htons(hostPkt.groupID);
    net.senderID   = htons(hostPkt.senderID);
    net.timestamp  = htonl(hostPkt.timestamp);
    return net;
}

// Convert received network-order packet to host-order
inline ChatPacket to_host(const ChatPacket &netPkt) {
    ChatPacket host = netPkt;
    host.groupID    = ntohs(netPkt.groupID);
    host.senderID   = ntohs(netPkt.senderID);
    host.timestamp  = ntohl(netPkt.timestamp);
    return host;
}

