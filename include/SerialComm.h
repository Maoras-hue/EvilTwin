/**
 * @file SerialComm.h
 * @brief Serial communication
 */

#pragma once

#include "EvilTwin.h"

class SerialComm {
public:
    virtual ~SerialComm() = default;
    
    virtual bool Connect(const std::string& port, int baud = 115200) = 0;
    virtual void Disconnect() = 0;
    virtual bool IsConnected() const = 0;
    
    virtual bool SendMessage(const SerialMessage& msg) = 0;
    virtual bool SendCommand(SerialMessageType type, const std::vector<uint8_t>& data = {}) = 0;
    
    virtual bool StartScan() = 0;
    virtual bool StopScan() = 0;
    virtual bool StartAttack(const std::string& ssid, const std::string& bssid) = 0;
    virtual bool StopAttack() = 0;
    virtual bool StartDeauth(const std::string& bssid) = 0;
    virtual bool StopDeauth() = 0;
    virtual bool GetStatus() = 0;
    
    virtual void SetCredentialCallback(CredentialCallback callback) = 0;
    virtual void SetStatusCallback(StatusUpdateCallback callback) = 0;
    virtual void SetLogCallback(LogCallback callback) = 0;
    
    virtual std::vector<std::string> GetAvailablePorts() = 0;
};

// Factory function declaration
std::unique_ptr<SerialComm> CreateSerialComm();
