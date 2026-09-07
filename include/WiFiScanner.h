/**
 * @file WiFiScanner.h
 * @brief WiFi network scanning
 */

#pragma once

#include "EvilTwin.h"

class WiFiScanner {
public:
    virtual ~WiFiScanner() = default;
    
    virtual bool Initialize(AppConfig* cfg = nullptr) = 0;
    virtual void Cleanup() = 0;
    virtual std::vector<WiFiNetwork> ScanNetworks() = 0;
    virtual bool ScanNetworksAsync(NetworkListCallback callback) = 0;
    virtual void StopScanning() = 0;
    virtual bool GetNetworkClients(const std::string& bssid, int& clientCount) = 0;
};

// Factory function declaration - NOT a class method
std::unique_ptr<WiFiScanner> CreateWiFiScanner();
