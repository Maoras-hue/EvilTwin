/**
 * @file UI.cpp
 * @brief User interface implementation
 */

#include "EvilTwin.h"
#include <conio.h>
#include <iomanip>
#include <thread>
#include <chrono>

// ============================================
// UI Class
// ============================================
class UI {
private:
    mutable std::mutex uiMutex;  // mutable so const methods can lock it
    bool running;
    bool showHelp;
    bool showCredentials;
    int selectedNetworkIndex;
    std::vector<WiFiNetwork> networks;
    std::vector<Credential> credentials;
    AttackStatus attackStatus;
    AppConfig config;
    
    bool needsRedrawFlag;  // Renamed to avoid conflict
    std::chrono::steady_clock::time_point lastUpdate;
    
public:
    UI() : running(true), showHelp(false), showCredentials(false),
           selectedNetworkIndex(-1), needsRedrawFlag(true) {
        lastUpdate = std::chrono::steady_clock::now();
    }
    
    ~UI() = default;
    
    void clearScreen() {
        system("cls");
        needsRedrawFlag = true;
    }
    
    void displayHeader() {
        std::cout << "╔══════════════════════════════════════════════════════════════╗\n";
        std::cout << "║              🎭 Evil Twin Attack Tool v1.0                  ║\n";
        std::cout << "║                   Windows Edition                            ║\n";
        std::cout << "╚══════════════════════════════════════════════════════════════╝\n";
        std::cout << "\n";
    }
    
    void displayMenu() {
        std::cout << "┌──────────────────────────────────────────────────────────────┐\n";
        std::cout << "│ [1] Scan Networks    [2] Start Attack    [3] Stop Attack    │\n";
        std::cout << "│ [4] Deauth Attack    [5] View Credentials [6] Settings      │\n";
        std::cout << "│ [7] Connect ESP32    [8] Help            [9] Exit           │\n";
        std::cout << "└──────────────────────────────────────────────────────────────┘\n";
        std::cout << "\n";
    }
    
    void displayNetworks() {
        std::lock_guard<std::mutex> lock(uiMutex);
        
        if (networks.empty()) {
            std::cout << "┌──────────────────────────────────────────────────────────────┐\n";
            std::cout << "│ No networks scanned. Press [1] to scan.                     │\n";
            std::cout << "└──────────────────────────────────────────────────────────────┘\n";
            std::cout << "\n";
            return;
        }
        
        std::cout << "┌──────────────────────────────────────────────────────────────┐\n";
        std::cout << "│ Available Networks (" << networks.size() << " found)                      │\n";
        std::cout << "├──────────────────────────────────────────────────────────────┤\n";
        
        int displayCount = std::min(8, (int)networks.size());
        for (int i = 0; i < displayCount; i++) {
            const auto& net = networks[i];
            std::cout << "│ " << std::setw(2) << (i + 1) << ". ";
            std::cout << std::setw(22) << std::left << truncateString(net.ssid, 22);
            std::cout << " CH:" << std::setw(3) << net.channel;
            std::cout << " RSSI:" << std::setw(4) << net.rssi << "dBm";
            
            if (net.isOpen) std::cout << " 🔓";
            else if (net.isWPA2) std::cout << " 🔒";
            else if (net.isWEP) std::cout << " 🔑";
            std::cout << " │\n";
        }
        
        if (networks.size() > 8) {
            std::cout << "│ ... and " << (networks.size() - 8) << " more networks        │\n";
        }
        
        std::cout << "└──────────────────────────────────────────────────────────────┘\n";
        std::cout << "\n";
    }
    
    void displayAttackStatus() {
        std::lock_guard<std::mutex> lock(uiMutex);
        
        std::cout << "┌──────────────────────────────────────────────────────────────┐\n";
        std::string statusText = attackStatus.isRunning ? "🟢 RUNNING" : "⚫ IDLE";
        std::cout << "│ Attack Status: " << std::setw(40) << std::left << statusText << "│\n";
        
        if (attackStatus.isRunning) {
            std::cout << "│ Target: " << std::setw(43) << std::left 
                      << truncateString(attackStatus.targetSSID, 43) << "│\n";
            
            std::stringstream ss;
            ss << "Clients: " << std::setw(4) << attackStatus.connectedClients
               << "  Credentials: " << std::setw(4) << attackStatus.credentialsCaptured
               << "  Deauth: " << std::setw(6) << attackStatus.deauthPacketsSent;
            std::cout << "│ " << std::setw(48) << std::left << ss.str() << "│\n";
        }
        
        std::cout << "└──────────────────────────────────────────────────────────────┘\n";
        std::cout << "\n";
    }
    
    void displayCredentials() {
        std::lock_guard<std::mutex> lock(uiMutex);
        
        if (!showCredentials) return;
        
        clearScreen();
        displayHeader();
        
        std::cout << "┌──────────────────────────────────────────────────────────────┐\n";
        std::cout << "│ Captured Credentials (" << credentials.size() << " total)                  │\n";
        std::cout << "├──────────────────────────────────────────────────────────────┤\n";
        
        if (credentials.empty()) {
            std::cout << "│ No credentials captured yet.                             │\n";
        } else {
            for (size_t i = 0; i < std::min((size_t)10, credentials.size()); i++) {
                const auto& cred = credentials[i];
                std::cout << "│ " << std::setw(2) << (i + 1) << ". ";
                std::cout << std::setw(12) << truncateString(cred.username, 12);
                std::cout << ":" << std::setw(12) << truncateString(cred.password, 12);
                std::cout << " @" << std::setw(12) << truncateString(cred.ssid, 12);
                
                auto time = std::chrono::system_clock::to_time_t(cred.timestamp);
                std::cout << " " << std::put_time(std::localtime(&time), "%H:%M");
                std::cout << " │\n";
            }
            
            if (credentials.size() > 10) {
                std::cout << "│ ... and " << (credentials.size() - 10) << " more credentials     │\n";
            }
        }
        
        std::cout << "└──────────────────────────────────────────────────────────────┘\n";
        std::cout << "\nPress 'e' to export, 'c' to clear, any other key to close\n";
    }
    
    void displayHelp() {
        clearScreen();
        displayHeader();
        
        std::cout << "╔══════════════════════════════════════════════════════════════╗\n";
        std::cout << "║                       HELP MENU                              ║\n";
        std::cout << "╚══════════════════════════════════════════════════════════════╝\n\n";
        
        std::cout << "┌──────────────────────────────────────────────────────────────┐\n";
        std::cout << "│ 1. Scan Networks  - Scan for nearby WiFi networks           │\n";
        std::cout << "│ 2. Start Attack   - Begin Evil Twin attack on target        │\n";
        std::cout << "│ 3. Stop Attack    - Stop the current attack                 │\n";
        std::cout << "│ 4. Deauth Attack  - Send deauth packets to disconnect users │\n";
        std::cout << "│ 5. View Creds     - See captured credentials                │\n";
        std::cout << "│ 6. Settings       - Configure application settings          │\n";
        std::cout << "│ 7. Connect ESP32  - Connect to ESP32 hardware               │\n";
        std::cout << "│ 8. Help           - Display this help menu                  │\n";
        std::cout << "│ 9. Exit           - Exit the application                    │\n";
        std::cout << "└──────────────────────────────────────────────────────────────┘\n\n";
        
        std::cout << "┌──────────────────────────────────────────────────────────────┐\n";
        std::cout << "│ ⚠️  LEGAL DISCLAIMER                                       │\n";
        std::cout << "│ This tool is for EDUCATIONAL PURPOSES only.                │\n";
        std::cout << "└──────────────────────────────────────────────────────────────┘\n";
        
        std::cout << "\nPress any key to continue...\n";
        _getch();
    }
    
    void displaySettings() {
        clearScreen();
        displayHeader();
        
        std::cout << "┌──────────────────────────────────────────────────────────────┐\n";
        std::cout << "│ Settings                                                     │\n";
        std::cout << "├──────────────────────────────────────────────────────────────┤\n";
        std::cout << "│ 1. Serial Port:  " << std::setw(34) << std::left << config.serialPort << "│\n";
        std::cout << "│ 2. Baud Rate:    " << std::setw(36) << std::left << config.baudRate << "│\n";
        std::cout << "│ 3. Scan Interval:" << std::setw(33) << std::left << config.scanInterval << "s│\n";
        std::cout << "│ 4. Auto Deauth:  " << std::setw(35) << std::left 
                  << (config.autoDeauth ? "✅ On" : "❌ Off") << "│\n";
        std::cout << "│ 5. Log Path:     " << std::setw(37) << std::left << config.logPath << "│\n";
        std::cout << "│ 6. Max Clients:  " << std::setw(36) << std::left << config.maxClients << "│\n";
        std::cout << "└──────────────────────────────────────────────────────────────┘\n";
        std::cout << "\nEnter setting number to change (0 to exit): ";
    }
    
    // ============================================
    // Input Methods
    // ============================================
    
    char getChoice() {
        std::cout << "Enter choice: ";
        char choice = _getch();
        std::cout << choice << "\n\n";
        return choice;
    }
    
    int getNetworkSelection() {
        std::cout << "Select target network (1-" << networks.size() << "): ";
        int index;
        std::cin >> index;
        std::cin.ignore();
        return index;
    }
    
    bool getConfirmation(const std::string& prompt) {
        std::cout << prompt << " (y/n): ";
        char confirm;
        std::cin >> confirm;
        std::cin.ignore();
        return tolower(confirm) == 'y';
    }
    
    std::string getInput(const std::string& prompt) {
        std::cout << prompt;
        std::string input;
        std::getline(std::cin, input);
        return input;
    }
    
    int getIntInput(const std::string& prompt) {
        std::cout << prompt;
        int value;
        std::cin >> value;
        std::cin.ignore();
        return value;
    }
    
    // ============================================
    // Display Methods
    // ============================================
    
    void displayMessage(const std::string& msg) {
        std::cout << "\n" << msg << "\n";
        std::cout << "Press any key to continue...\n";
        _getch();
    }
    
    void displayError(const std::string& error) {
        std::cout << "\n❌ " << error << "\n";
        std::cout << "Press any key to continue...\n";
        _getch();
    }
    
    void displaySuccess(const std::string& msg) {
        std::cout << "\n✅ " << msg << "\n";
        std::cout << "Press any key to continue...\n";
        _getch();
    }
    
    void displayInfo(const std::string& info) {
        std::cout << "\nℹ️  " << info << "\n";
        std::cout << "Press any key to continue...\n";
        _getch();
    }
    
    // ============================================
    // Update Methods
    // ============================================
    
    void updateNetworks(const std::vector<WiFiNetwork>& newNetworks) {
        std::lock_guard<std::mutex> lock(uiMutex);
        networks = newNetworks;
        needsRedrawFlag = true;
    }
    
    void updateStatus(const AttackStatus& status) {
        std::lock_guard<std::mutex> lock(uiMutex);
        attackStatus = status;
        needsRedrawFlag = true;
    }
    
    void addCredential(const Credential& cred) {
        std::lock_guard<std::mutex> lock(uiMutex);
        credentials.push_back(cred);
        needsRedrawFlag = true;
    }
    
    void clearCredentials() {
        std::lock_guard<std::mutex> lock(uiMutex);
        credentials.clear();
        needsRedrawFlag = true;
    }
    
    void updateConfig(const AppConfig& newConfig) {
        std::lock_guard<std::mutex> lock(uiMutex);
        config = newConfig;
    }
    
    // ============================================
    // Getters
    // ============================================
    
    bool isRunning() const { return running; }
    void setRunning(bool run) { running = run; }
    
    bool getShowCredentials() const { return showCredentials; }
    void setShowCredentials(bool show) { showCredentials = show; }
    
    int getSelectedNetwork() const { return selectedNetworkIndex; }
    void setSelectedNetwork(int idx) { selectedNetworkIndex = idx; }
    
    std::vector<WiFiNetwork> getNetworks() const {
        std::lock_guard<std::mutex> lock(uiMutex);
        return networks;
    }
    
    std::vector<Credential> getCredentials() const {
        std::lock_guard<std::mutex> lock(uiMutex);
        return credentials;
    }
    
    AppConfig getConfig() const {
        std::lock_guard<std::mutex> lock(uiMutex);
        return config;
    }
    
    bool needsRedraw() const { return needsRedrawFlag; }
    void resetRedraw() { needsRedrawFlag = false; }
    
    // ============================================
    // Export
    // ============================================
    
    bool exportCredentials(const std::string& filename = "") {
        std::lock_guard<std::mutex> lock(uiMutex);
        
        if (credentials.empty()) {
            displayError("No credentials to export");
            return false;
        }
        
        std::string outFile = filename;
        if (outFile.empty()) {
            auto now = std::chrono::system_clock::now();
            auto time = std::chrono::system_clock::to_time_t(now);
            std::stringstream ss;
            ss << "credentials_" << std::put_time(std::localtime(&time), "%Y%m%d_%H%M%S") << ".txt";
            outFile = ss.str();
        }
        
        std::ofstream file(outFile);
        if (!file.is_open()) {
            displayError("Failed to create file: " + outFile);
            return false;
        }
        
        file << "🎭 Evil Twin Attack Tool - Captured Credentials\n";
        file << "================================================\n\n";
        file << "Total Captured: " << credentials.size() << "\n\n";
        
        for (size_t i = 0; i < credentials.size(); i++) {
            const auto& cred = credentials[i];
            auto time = std::chrono::system_clock::to_time_t(cred.timestamp);
            file << std::setw(4) << (i + 1) << ". ";
            file << std::put_time(std::localtime(&time), "%Y-%m-%d %H:%M:%S");
            file << " | SSID: " << cred.ssid;
            file << " | User: " << cred.username;
            file << " | Pass: " << cred.password;
            file << " | MAC: " << cred.clientMac << "\n";
        }
        
        file.close();
        displaySuccess("Credentials exported to: " + outFile);
        return true;
    }
    
private:
    std::string truncateString(const std::string& str, size_t maxLen) {
        if (str.length() <= maxLen) return str;
        return str.substr(0, maxLen - 3) + "...";
    }
};

// ============================================
// Global UI Instance
// ============================================

static std::unique_ptr<UI> g_ui;

// ============================================
// Public UI Functions
// ============================================

void UI_Init() {
    g_ui = std::make_unique<UI>();
}

void UI_Cleanup() {
    g_ui.reset();
}

void UI_ClearScreen() {
    if (g_ui) g_ui->clearScreen();
}

void UI_DisplayHeader() {
    if (g_ui) g_ui->displayHeader();
}

void UI_DisplayMenu() {
    if (g_ui) g_ui->displayMenu();
}

void UI_DisplayNetworks() {
    if (g_ui) g_ui->displayNetworks();
}

void UI_DisplayStatus() {
    if (g_ui) g_ui->displayAttackStatus();
}

void UI_DisplayCredentials() {
    if (g_ui) g_ui->displayCredentials();
}

void UI_DisplayHelp() {
    if (g_ui) g_ui->displayHelp();
}

void UI_DisplaySettings() {
    if (g_ui) g_ui->displaySettings();
}

char UI_GetChoice() {
    return g_ui ? g_ui->getChoice() : 0;
}

int UI_GetNetworkSelection() {
    return g_ui ? g_ui->getNetworkSelection() : -1;
}

bool UI_GetConfirmation(const std::string& prompt) {
    return g_ui ? g_ui->getConfirmation(prompt) : false;
}

std::string UI_GetInput(const std::string& prompt) {
    return g_ui ? g_ui->getInput(prompt) : "";
}

int UI_GetIntInput(const std::string& prompt) {
    return g_ui ? g_ui->getIntInput(prompt) : 0;
}

void UI_DisplayMessage(const std::string& msg) {
    if (g_ui) g_ui->displayMessage(msg);
}

void UI_DisplayError(const std::string& error) {
    if (g_ui) g_ui->displayError(error);
}

void UI_DisplaySuccess(const std::string& msg) {
    if (g_ui) g_ui->displaySuccess(msg);
}

void UI_DisplayInfo(const std::string& info) {
    if (g_ui) g_ui->displayInfo(info);
}

void UI_UpdateNetworks(const std::vector<WiFiNetwork>& networks) {
    if (g_ui) g_ui->updateNetworks(networks);
}

void UI_UpdateStatus(const AttackStatus& status) {
    if (g_ui) g_ui->updateStatus(status);
}

void UI_AddCredential(const Credential& cred) {
    if (g_ui) g_ui->addCredential(cred);
}

void UI_ClearCredentials() {
    if (g_ui) g_ui->clearCredentials();
}

void UI_UpdateConfig(const AppConfig& config) {
    if (g_ui) g_ui->updateConfig(config);
}

bool UI_IsRunning() {
    return g_ui ? g_ui->isRunning() : false;
}

void UI_SetRunning(bool running) {
    if (g_ui) g_ui->setRunning(running);
}

bool UI_GetShowCredentials() {
    return g_ui ? g_ui->getShowCredentials() : false;
}

void UI_SetShowCredentials(bool show) {
    if (g_ui) g_ui->setShowCredentials(show);
}

int UI_GetSelectedNetwork() {
    return g_ui ? g_ui->getSelectedNetwork() : -1;
}

void UI_SetSelectedNetwork(int idx) {
    if (g_ui) g_ui->setSelectedNetwork(idx);
}

std::vector<WiFiNetwork> UI_GetNetworks() {
    return g_ui ? g_ui->getNetworks() : std::vector<WiFiNetwork>();
}

std::vector<Credential> UI_GetCredentials() {
    return g_ui ? g_ui->getCredentials() : std::vector<Credential>();
}

AppConfig UI_GetConfig() {
    return g_ui ? g_ui->getConfig() : AppConfig();
}

bool UI_ExportCredentials(const std::string& filename) {
    return g_ui ? g_ui->exportCredentials(filename) : false;
}
