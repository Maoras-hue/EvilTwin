/**
 * @file Logger.h
 * @brief Logging functionality
 */

#pragma once

#include "EvilTwin.h"
#include <filesystem>

namespace fs = std::filesystem;

class Logger {
public:
    virtual ~Logger() = default;
    
    virtual bool Initialize(const std::string& path = "logs/") = 0;
    virtual void Shutdown() = 0;
    
    virtual void Log(const std::string& message, bool toConsole = true) = 0;
    virtual void LogCredential(const Credential& cred) = 0;
    virtual void LogNetwork(const WiFiNetwork& network) = 0;
    virtual void LogError(const std::string& error) = 0;
    virtual void LogInfo(const std::string& info) = 0;
    virtual void LogWarning(const std::string& warning) = 0;
    
    virtual void SetCallback(LogCallback callback) = 0;
    virtual std::string GetLogFilePath() = 0;
    virtual std::vector<std::string> GetRecentLogs(int count = 100) = 0;
};

// Factory function declaration
std::unique_ptr<Logger> CreateLogger();
