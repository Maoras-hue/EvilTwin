/**
 * @file WiFiScanner.cpp
 * @brief WiFi network scanning implementation
 */

#include "WiFiScanner.h"

class WiFiScannerImpl : public WiFiScanner {
private:
    HANDLE hClient;
    PWLAN_INTERFACE_INFO_LIST pIfList;
    std::mutex scanMutex;
    NetworkListCallback scanCallback;
    std::atomic<bool> isScanning;
    std::thread scanThread;
    AppConfig* config;

public:
    WiFiScannerImpl() : hClient(nullptr), pIfList(nullptr), 
                        isScanning(false), config(nullptr) {}
    
    ~WiFiScannerImpl() { Cleanup(); }
    
    bool Initialize(AppConfig* cfg = nullptr) override {
        config = cfg;
        return initializeWLAN();
    }
    
    void Cleanup() override {
        isScanning = false;
        if (scanThread.joinable()) scanThread.join();
        if (pIfList) { WlanFreeMemory(pIfList); pIfList = nullptr; }
        if (hClient) { WlanCloseHandle(hClient, nullptr); hClient = nullptr; }
    }
    
    std::vector<WiFiNetwork> ScanNetworks() override {
        if (!hClient || !pIfList) return {};
        
        std::lock_guard<std::mutex> lock(scanMutex);
        std::vector<WiFiNetwork> networks;
        
        for (DWORD i = 0; i < pIfList->dwNumberOfItems; i++) {
            PWLAN_INTERFACE_INFO pIfInfo = &pIfList->InterfaceInfo[i];
            if (pIfInfo->isState != wlan_interface_state_connected) continue;
            
            GUID interfaceGuid = pIfInfo->InterfaceGuid;
            WlanScan(hClient, &interfaceGuid, nullptr, nullptr, nullptr);
            Sleep(2000);
            
            PWLAN_BSS_LIST pBssList = nullptr;
            if (WlanGetNetworkBssList(hClient, &interfaceGuid, nullptr,
                (DOT11_BSS_TYPE)0, FALSE, nullptr, &pBssList) == ERROR_SUCCESS && pBssList) {
                
                for (DWORD j = 0; j < pBssList->dwNumberOfItems; j++) {
                    PWLAN_BSS_ENTRY pBssEntry = &pBssList->wlanBssEntries[j];
                    
                    WiFiNetwork net;
                    // BSSID
                    char mac[18];
                    snprintf(mac, sizeof(mac), "%02X:%02X:%02X:%02X:%02X:%02X",
                        pBssEntry->dot11Bssid[0], pBssEntry->dot11Bssid[1],
                        pBssEntry->dot11Bssid[2], pBssEntry->dot11Bssid[3],
                        pBssEntry->dot11Bssid[4], pBssEntry->dot11Bssid[5]);
                    net.bssid = mac;
                    
                    net.rssi = pBssEntry->lRssi;
                    
                    // Channel from frequency
                    if (pBssEntry->ulChCenterFrequency >= 2412 && 
                        pBssEntry->ulChCenterFrequency <= 2484) {
                        net.channel = (pBssEntry->ulChCenterFrequency - 2412) / 5 + 1;
                    }
                    
                    // SSID
                    if (pBssEntry->dot11Ssid.uSSIDLength > 0) {
                        net.ssid = std::string(
                            (char*)pBssEntry->dot11Ssid.ucSSID,
                            pBssEntry->dot11Ssid.uSSIDLength);
                    }
                    
                    if (!net.ssid.empty()) networks.push_back(net);
                }
                WlanFreeMemory(pBssList);
            }
        }
        return networks;
    }
    
    bool ScanNetworksAsync(NetworkListCallback callback) override {
        scanCallback = callback;
        isScanning = true;
        scanThread = std::thread([this]() {
            while (isScanning) {
                auto nets = ScanNetworks();
                if (scanCallback && !nets.empty()) scanCallback(nets);
                Sleep((config ? config->scanInterval : 5) * 1000);
            }
        });
        return true;
    }
    
    void StopScanning() override {
        isScanning = false;
        if (scanThread.joinable()) scanThread.join();
    }
    
    bool GetNetworkClients(const std::string& bssid, int& clientCount) override {
        (void)bssid; clientCount = 0; return false;
    }

private:
    bool initializeWLAN() {
        DWORD dwResult, dwVersion;
        dwResult = WlanOpenHandle(2, nullptr, &dwVersion, &hClient);
        if (dwResult != ERROR_SUCCESS) return false;
        
        dwResult = WlanEnumInterfaces(hClient, nullptr, &pIfList);
        if (dwResult != ERROR_SUCCESS) {
            WlanCloseHandle(hClient, nullptr);
            hClient = nullptr;
            return false;
        }
        return true;
    }
};

// Factory function implementation
std::unique_ptr<WiFiScanner> CreateWiFiScanner() {
    return std::make_unique<WiFiScannerImpl>();
}
