/**
 * @file CaptivePortal.h
 * @brief Captive portal HTTP server
 */

#pragma once

#include "EvilTwin.h"

class CaptivePortal {
public:
    virtual ~CaptivePortal() = default;
    
    virtual bool Start(const std::string& ip, int port = 80) = 0;
    virtual void Stop() = 0;
    virtual bool IsRunning() const = 0;
    
    virtual bool SetContent(const std::string& html) = 0;
    virtual void SetCredentialCallback(CredentialCallback callback) = 0;
    virtual void SetLogCallback(LogCallback callback) = 0;
    
    virtual std::vector<Credential> GetCredentials() = 0;
    virtual void ClearCredentials() = 0;
    
    virtual std::string GetIPAddress() const = 0;
    virtual int GetPort() const = 0;
};

// Factory function declaration
std::unique_ptr<CaptivePortal> CreateCaptivePortal();
