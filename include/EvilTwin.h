/**
 * @file EvilTwin.h
 * @brief Main header for Evil Twin Attack Tool
 */

#pragma once

// Must be before any includes
#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN
#endif
#ifndef NOMINMAX
#define NOMINMAX
#endif

#include <string>
#include <vector>
#include <memory>
#include <thread>
#include <atomic>
#include <chrono>
#include <functional>
#include <mutex>
#include <queue>
#include <condition_variable>
#include <iostream>
#include <fstream>
#include <sstream>
#include <iomanip>
#include <algorithm>
#include <map>
#include <random>
#include <ctime>

// Windows headers
#include <windows.h>
#include <wlanapi.h>
#include <ws2tcpip.h>
#include <iphlpapi.h>

// ============================================
// Forward declarations
// ============================================
struct WiFiNetwork;
struct Credential;
struct AttackStatus;
struct AppConfig;

// ============================================
// Callback types
// ============================================
using NetworkListCallback = std::function<void(const std::vector<WiFiNetwork>&)>;
using CredentialCallback = std::function<void(const Credential&)>;
using StatusUpdateCallback = std::function<void(const AttackStatus&)>;
using LogCallback = std::function<void(const std::string&)>;

// ============================================
// Network information structure
// ============================================
struct WiFiNetwork {
    std::string ssid;
    std::string bssid;
    std::string security;
    int channel;
    int rssi;
    int clients;
    bool isWPA2;
    bool isWEP;
    bool isOpen;
    
    WiFiNetwork() : channel(0), rssi(0), clients(0), 
                    isWPA2(false), isWEP(false), isOpen(false) {}
};

// ============================================
// Captured credential structure
// ============================================
struct Credential {
    std::string username;
    std::string password;
    std::string ssid;
    std::string bssid;
    std::string clientMac;
    std::chrono::system_clock::time_point timestamp;
    
    Credential() : username(""), password(""), ssid(""), 
                   bssid(""), clientMac("") {}
};

// ============================================
// Attack status structure
// ============================================
struct AttackStatus {
    bool isRunning;
    bool isDeauthRunning;
    int connectedClients;
    int credentialsCaptured;
    int deauthPacketsSent;
    std::string targetSSID;
    std::string targetBSSID;
    std::chrono::system_clock::time_point startTime;
    
    AttackStatus() : isRunning(false), isDeauthRunning(false),
                     connectedClients(0), credentialsCaptured(0),
                     deauthPacketsSent(0) {}
};

// ============================================
// Serial message types
// ============================================
enum class SerialMessageType : uint8_t {
    MSG_HELLO = 0x01,
    MSG_SCAN_START = 0x02,
    MSG_SCAN_RESULT = 0x03,
    MSG_ATTACK_START = 0x04,
    MSG_ATTACK_STOP = 0x05,
    MSG_DEAUTH_START = 0x06,
    MSG_DEAUTH_STOP = 0x07,
    MSG_CREDENTIAL = 0x08,
    MSG_STATUS = 0x09,
    MSG_ERROR = 0x0A,
    MSG_ACK = 0x0B,
    MSG_NACK = 0x0C,
    MSG_SCAN_STOP = 0x0D
};

// ============================================
// Serial message structure
// ============================================
struct SerialMessage {
    SerialMessageType type;
    uint16_t length;
    std::vector<uint8_t> data;
    
    SerialMessage() : type(SerialMessageType::MSG_HELLO), length(0) {}
    
    std::vector<uint8_t> serialize() const {
        std::vector<uint8_t> buffer;
        buffer.push_back(static_cast<uint8_t>(type));
        buffer.push_back(static_cast<uint8_t>(length >> 8));
        buffer.push_back(static_cast<uint8_t>(length & 0xFF));
        buffer.insert(buffer.end(), data.begin(), data.end());
        return buffer;
    }
    
    bool deserialize(const std::vector<uint8_t>& buffer) {
        if (buffer.size() < 3) return false;
        type = static_cast<SerialMessageType>(buffer[0]);
        length = (buffer[1] << 8) | buffer[2];
        if (buffer.size() < 3 + length) return false;
        data.assign(buffer.begin() + 3, buffer.begin() + 3 + length);
        return true;
    }
};

// ============================================
// Application-wide configuration
// ============================================
struct AppConfig {
    std::string serialPort;
    int baudRate;
    int scanInterval;
    std::string logPath;
    bool autoDeauth;
    int maxClients;
    
    AppConfig() : serialPort("COM3"), baudRate(115200),
                  scanInterval(5), logPath("logs/"),
                  autoDeauth(true), maxClients(10) {}
};
