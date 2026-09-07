# 🔄 Evil Twin Attack Tool - Complete Workflow

## 📋 Table of Contents
1. [System Overview](#system-overview)
2. [Component Workflows](#component-workflows)
3. [Attack Execution Flow](#attack-execution-flow)
4. [Data Flow Diagrams](#data-flow-diagrams)
5. [User Interaction Flow](#user-interaction-flow)
6. [ESP32 Firmware Flow](#esp32-firmware-flow)
7. [Error Handling Flow](#error-handling-flow)
8. [Credential Capture Flow](#credential-capture-flow)

---

## System Overview

```
┌─────────────────────────────────────────────────────────────────────────────────────┐
│                                                                                     │
│                          EVIL TWIN ATTACK SYSTEM                                    │
│                                                                                     │
│  ┌─────────────────────────────────────────────────────────────────────────────┐   │
│  │                          USER INTERFACE LAYER                               │   │
│  │                                                                             │   │
│  │  ┌──────────┐  ┌──────────┐  ┌──────────┐  ┌──────────┐  ┌──────────┐   │   │
│  │  │  Menu    │  │  Scan    │  │  Attack  │  │  Deauth  │  │  View    │   │   │
│  │  │  System  │  │  Display │  │  Control │  │  Control │  │  Logs    │   │   │
│  │  └──────────┘  └──────────┘  └──────────┘  └──────────┘  └──────────┘   │   │
│  └─────────────────────────────────────────────────────────────────────────────┘   │
│                                      │                                              │
│                                      ▼                                              │
│  ┌─────────────────────────────────────────────────────────────────────────────┐   │
│  │                          APPLICATION LOGIC LAYER                             │   │
│  │                                                                             │   │
│  │  ┌──────────┐  ┌──────────┐  ┌──────────┐  ┌──────────┐  ┌──────────┐   │   │
│  │  │  WiFi    │  │  Serial  │  │  Captive │  │  Logger  │  │  Attack  │   │   │
│  │  │  Scanner │  │  Comm    │  │  Portal  │  │  Module  │  │  Manager │   │   │
│  │  └──────────┘  └──────────┘  └──────────┘  └──────────┘  └──────────┘   │   │
│  └─────────────────────────────────────────────────────────────────────────────┘   │
│                                      │                                              │
│                                      ▼                                              │
│  ┌─────────────────────────────────────────────────────────────────────────────┐   │
│  │                          HARDWARE LAYER                                     │   │
│  │                                                                             │   │
│  │  ┌──────────────────────────┐      ┌─────────────────────────────────┐   │   │
│  │  │   ESP32                   │      │   WiFi Adapter (Windows)        │   │   │
│  │  │                           │      │                                 │   │   │
│  │  │  - Fake AP                │      │  - Network Scanning             │   │   │
│  │  │  - DNS Spoofer            │      │  - BSS List Retrieval           │   │   │
│  │  │  - Web Server             │      │  - Signal Detection             │   │   │
│  │  │  - Deauth Engine          │      │                                 │   │   │
│  │  └──────────────────────────┘      └─────────────────────────────────┘   │   │
│  └─────────────────────────────────────────────────────────────────────────────┘   │
│                                      │                                              │
│                                      ▼                                              │
│  ┌─────────────────────────────────────────────────────────────────────────────┐   │
│  │                          TARGET LAYER                                       │   │
│  │                                                                             │   │
│  │  ┌─────────────────────────────────────────────────────────────────────┐   │   │
│  │  │                         VICTIM DEVICES                              │   │   │
│  │  │                                                                     │   │   │
│  │  │  ┌──────────┐  ┌──────────┐  ┌──────────┐  ┌──────────┐         │   │   │
│  │  │  │  Phone   │  │  Laptop  │  │  Tablet  │  │  IoT     │         │   │   │
│  │  │  └──────────┘  └──────────┘  └──────────┘  └──────────┘         │   │   │
│  │  └─────────────────────────────────────────────────────────────────────┘   │   │
│  └─────────────────────────────────────────────────────────────────────────────┘   │
│                                                                                     │
└─────────────────────────────────────────────────────────────────────────────────────┘
```

---

## Component Workflows

### 1. WiFi Scanner Workflow

```
┌─────────────────────────────────────────────────────────────────────────────────────┐
│                          WIFI SCANNER WORKFLOW                                      │
└─────────────────────────────────────────────────────────────────────────────────────┘

     ┌──────────────────┐
     │  User presses 1  │
     └────────┬─────────┘
              │
              ▼
     ┌──────────────────────────────────────────────────────────────┐
     │  Step 1: Initialize WLAN Client                              │
     │  ─────────────────────────────                              │
     │  • WlanOpenHandle() → Get handle to WiFi service            │
     │  • WlanEnumInterfaces() → List all WiFi adapters            │
     │  • Check if any interfaces found                             │
     └──────────────┬───────────────────────────────────────────────┘
                    │
                    ▼
     ┌──────────────────────────────────────────────────────────────┐
     │  Step 2: Scan Networks                                      │
     │  ───────────────────                                        │
     │  For each WiFi Interface:                                   │
     │    • WlanScan() → Start network scan                        │
     │    • Sleep(3000) → Wait for scan to complete               │
     │    • WlanGetNetworkBssList() → Get BSS list                │
     └──────────────┬───────────────────────────────────────────────┘
                    │
                    ▼
     ┌──────────────────────────────────────────────────────────────┐
     │  Step 3: Parse Results                                      │
     │  ───────────────────                                        │
     │  For each BSS Entry:                                        │
     │    • Extract SSID (Network name)                            │
     │    • Extract BSSID (MAC address)                            │
     │    • Extract Channel (from frequency)                       │
     │    • Extract RSSI (Signal strength)                         │
     │    • Detect Security type (Open/WPA/WPA2)                   │
     └──────────────┬───────────────────────────────────────────────┘
                    │
                    ▼
     ┌──────────────────────────────────────────────────────────────┐
     │  Step 4: Display Results                                    │
     │  ────────────────────                                       │
     │  • Update UI with network list                              │
     │  • Log found networks                                       │
     │  • Show count: "Found X networks"                           │
     └──────────────────────────────────────────────────────────────┘

     ┌──────────────────────────────────────────────────────────────┐
     │  Output: Vector<WiFiNetwork>                                │
     │  ───────────────────────────                                │
     │  struct WiFiNetwork {                                       │
     │      string ssid;      // "HomeWiFi"                       │
     │      string bssid;     // "AA:BB:CC:DD:EE:FF"             │
     │      int channel;      // 6                                │
     │      int rssi;         // -45                              │
     │      string security;  // "WPA2-PSK"                       │
     │      bool isOpen;      // false                            │
     │      bool isWPA2;      // true                             │
     │  }                                                          │
     └──────────────────────────────────────────────────────────────┘
```

### 2. Serial Communication Workflow

```
┌─────────────────────────────────────────────────────────────────────────────────────┐
│                      SERIAL COMMUNICATION WORKFLOW                                   │
└─────────────────────────────────────────────────────────────────────────────────────┘

     ┌──────────────────┐
     │  User presses 7  │
     └────────┬─────────┘
              │
              ▼
     ┌──────────────────────────────────────────────────────────────┐
     │  Step 1: Open COM Port                                      │
     │  ───────────────────                                        │
     │  • CreateFileA("\\\\.\\COM3", ...)                         │
     │  • Configure DCB (Baud: 115200, 8N1)                       │
     │  • Set timeouts (Read: 50ms, Write: 50ms)                  │
     └──────────────┬───────────────────────────────────────────────┘
                    │
                    ▼
     ┌──────────────────────────────────────────────────────────────┐
     │  Step 2: Handshake                                          │
     │  ────────────────                                           │
     │  Send: [0x01] [0x00] [0x00]  // MSG_HELLO                  │
     │  Wait for: [0x0B] [0x00] [0x00] // MSG_ACK                 │
     └──────────────┬───────────────────────────────────────────────┘
                    │
                    ▼
     ┌──────────────────────────────────────────────────────────────┐
     │  Step 3: Start Read Thread                                  │
     │  ─────────────────────                                      │
     │  • Create background thread                                 │
     │  • Continuously read from serial port                       │
     │  • Parse incoming messages                                  │
     │  • Process messages via callbacks                           │
     └──────────────┬───────────────────────────────────────────────┘
                    │
                    ▼
     ┌──────────────────────────────────────────────────────────────┐
     │  Step 4: Message Processing                                 │
     │  ─────────────────────                                      │
     │  When message received:                                     │
     │    • MSG_CREDENTIAL → Call credentialCallback()             │
     │    • MSG_STATUS → Call statusCallback()                     │
     │    • MSG_ERROR → Call logCallback()                         │
     │    • MSG_ACK → Log acknowledgment                           │
     └──────────────────────────────────────────────────────────────┘

     ┌──────────────────────────────────────────────────────────────┐
     │  Message Format:                                            │
     │  ────────────────                                           │
     │  [Type: 1 byte] [Length: 2 bytes] [Data: N bytes]          │
     │                                                             │
     │  Example - HELLO:                                           │
     │  0x01 0x00 0x00                                             │
     │                                                             │
     │  Example - CREDENTIAL:                                      │
     │  0x08 0x00 0x1E "user\0pass\0SSID\0MAC\0"                  │
     └──────────────────────────────────────────────────────────────┘
```

### 3. Captive Portal Workflow

```
┌─────────────────────────────────────────────────────────────────────────────────────┐
│                         CAPTIVE PORTAL WORKFLOW                                     │
└─────────────────────────────────────────────────────────────────────────────────────┘

     ┌──────────────────┐
     │  Attack Starts   │
     └────────┬─────────┘
              │
              ▼
     ┌──────────────────────────────────────────────────────────────┐
     │  Step 1: Start HTTP Server                                  │
     │  ──────────────────────                                     │
     │  • socket() → Create listening socket                       │
     │  • bind() → Bind to port 80                                │
     │  • listen() → Start listening                               │
     │  • accept() → Accept connections                            │
     └──────────────┬───────────────────────────────────────────────┘
                    │
                    ▼
     ┌──────────────────────────────────────────────────────────────┐
     │  Step 2: Handle HTTP Requests                               │
     │  ─────────────────────────                                  │
     │  Client connects → GET /                                    │
     │    → Serve portal.html                                      │
     │                                                             │
     │  Client connects → GET /style.css                           │
     │    → Serve style.css                                        │
     │                                                             │
     │  Client connects → GET /script.js                           │
     │    → Serve script.js                                        │
     └──────────────┬───────────────────────────────────────────────┘
                    │
                    ▼
     ┌──────────────────────────────────────────────────────────────┐
     │  Step 3: Handle Form Submission                             │
     │  ──────────────────────────                                 │
     │  Client connects → POST /login                              │
     │    • Parse POST data                                        │
     │    • Extract username and password                          │
     │    • Save to credentials vector                             │
     │    • Call credentialCallback()                              │
     │    • Serve success page                                     │
     └──────────────┬───────────────────────────────────────────────┘
                    │
                    ▼
     ┌──────────────────────────────────────────────────────────────┐
     │  Step 4: Credential Flow                                    │
     │  ───────────────────                                        │
     │  1. Credentials captured                                    │
     │  2. Added to vector<Credential>                             │
     │  3. Callback triggered                                       │
     │  4. Logged to file                                          │
     │  5. Displayed in UI                                         │
     └──────────────────────────────────────────────────────────────┘
```

---

## Attack Execution Flow

```
┌─────────────────────────────────────────────────────────────────────────────────────┐
│                         ATTACK EXECUTION FLOW                                        │
└─────────────────────────────────────────────────────────────────────────────────────┘

┌─────────────────────────────────────────────────────────────────────────────────────┐
│                                                                                     │
│  ┌─────────────────────────────────────────────────────────────────────────────┐   │
│  │                                                                             │   │
│  │                          PHASE 1: PREPARATION                                │   │
│  │                          ──────────────────                                │   │
│  │                                                                             │   │
│  │  ┌─────────────┐    ┌─────────────┐    ┌─────────────┐    ┌─────────────┐   │   │
│  │  │  Scan for   │    │  Select    │    │  Connect    │    │  Prepare    │   │   │
│  │  │  Networks   │───▶│  Target    │───▶│  ESP32      │───▶│  Attack     │   │   │
│  │  │  (Option 1) │    │  Network   │    │  (Option 7) │    │  Parameters │   │   │
│  │  └─────────────┘    └─────────────┘    └─────────────┘    └─────────────┘   │   │
│  │         │                  │                  │                  │           │   │
│  │         ▼                  ▼                  ▼                  ▼           │   │
│  │  ┌─────────────┐    ┌─────────────┐    ┌─────────────┐    ┌─────────────┐   │   │
│  │  │  Networks   │    │  Target     │    │  COM Port   │    │  SSID,      │   │   │
│  │  │  List       │    │  SSID/BSSID │    │  Opened     │    │  BSSID, CH  │   │   │
│  │  │  Displayed  │    │  Selected   │    │  Connected  │    │  Stored     │   │   │
│  │  └─────────────┘    └─────────────┘    └─────────────┘    └─────────────┘   │   │
│  │                                                                             │   │
│  └─────────────────────────────────────────────────────────────────────────────┘   │
│                                      │                                              │
│                                      ▼                                              │
│  ┌─────────────────────────────────────────────────────────────────────────────┐   │
│  │                                                                             │   │
│  │                          PHASE 2: ATTACK INITIATION                         │   │
│  │                          ───────────────────────                           │   │
│  │                                                                             │   │
│  │  ┌─────────────────────────────────────────────────────────────────────┐   │   │
│  │  │                        WINDOWS APP                                   │   │   │
│  │  │                                                                     │   │   │
│  │  │  User presses 2 → StartAttack()                                     │   │   │
│  │  │                    ↓                                                 │   │   │
│  │  │  SerialComm::StartAttack(ssid, bssid)                               │   │   │
│  │  │                    ↓                                                 │   │   │
│  │  │  Send MSG_ATTACK_START with SSID and BSSID                         │   │   │
│  │  │                    ↓                                                 │   │   │
│  │  │  Wait for ACK from ESP32                                            │   │   │
│  │  │                    ↓                                                 │   │   │
│  │  │  Update UI: Attack Running                                          │   │   │
│  │  └─────────────────────────────────────────────────────────────────────┘   │   │
│  │                                      │                                      │   │
│  │                                      ▼                                      │   │
│  │  ┌─────────────────────────────────────────────────────────────────────┐   │   │
│  │  │                        ESP32 FIRMWARE                               │   │   │
│  │  │                                                                     │   │   │
│  │  │  Receive MSG_ATTACK_START                                           │   │   │
│  │  │                    ↓                                                 │   │   │
│  │  │  WiFi.mode(WIFI_AP)                                                 │   │   │
│  │  │                    ↓                                                 │   │   │
│  │  │  WiFi.softAP(ssid.c_str())  // Create fake AP                      │   │   │
│  │  │                    ↓                                                 │   │   │
│  │  │  dnsServer.start(DNS_PORT, "*", apIP)  // DNS spoofing             │   │   │
│  │  │                    ↓                                                 │   │   │
│  │  │  webServer.begin()  // Start HTTP server                           │   │   │
│  │  │                    ↓                                                 │   │   │
│  │  │  Send ACK back to Windows                                           │   │   │
│  │  └─────────────────────────────────────────────────────────────────────┘   │   │
│  │                                                                             │   │
│  └─────────────────────────────────────────────────────────────────────────────┘   │
│                                      │                                              │
│                                      ▼                                              │
│  ┌─────────────────────────────────────────────────────────────────────────────┐   │
│  │                                                                             │   │
│  │                          PHASE 3: ACTIVE ATTACK                             │   │
│  │                          ───────────────────                               │   │
│  │                                                                             │   │
│  │  ┌─────────────────────────────────────────────────────────────────────┐   │   │
│  │  │                    VICTIM PERSPECTIVE                               │   │   │
│  │  │                                                                     │   │   │
│  │  │  1. Victim sees WiFi network: [Target SSID]                        │   │   │
│  │  │  2. Victim connects to fake AP                                     │   │   │
│  │  │  3. Victim tries to open browser                                   │   │   │
│  │  │  4. DNS spoofing redirects to captive portal                       │   │   │
│  │  │  5. Victim sees login page                                         │   │   │
│  │  │  6. Victim enters credentials                                      │   │   │
│  │  │  7. Credentials sent to ESP32                                      │   │   │
│  │  └─────────────────────────────────────────────────────────────────────┘   │   │
│  │                                                                             │   │
│  │  ┌─────────────────────────────────────────────────────────────────────┐   │   │
│  │  │                    ESP32 PROCESSING                                 │   │   │
│  │  │                                                                     │   │   │
│  │  │  1. Receive POST /login with credentials                           │   │   │
│  │  │  2. Extract username and password                                  │   │   │
│  │  │  3. Save to EEPROM                                                  │   │   │
│  │  │  4. Send MSG_CREDENTIAL to Windows via serial                      │   │   │
│  │  │  5. Serve success page to victim                                   │   │   │
│  │  └─────────────────────────────────────────────────────────────────────┘   │   │
│  │                                                                             │   │
│  │  ┌─────────────────────────────────────────────────────────────────────┐   │   │
│  │  │                    WINDOWS PROCESSING                               │   │   │
│  │  │                                                                     │   │   │
│  │  │  1. Receive MSG_CREDENTIAL via serial                              │   │   │
│  │  │  2. Parse credential data                                           │   │   │
│  │  │  3. Add to credentials vector                                       │   │   │
│  │  │  4. Log to file                                                     │   │   │
│  │  │  5. Update UI display                                               │   │   │
│  │  │  6. Increment credential count                                      │   │   │
│  │  └─────────────────────────────────────────────────────────────────────┘   │   │
│  │                                                                             │   │
│  └─────────────────────────────────────────────────────────────────────────────┘   │
│                                      │                                              │
│                                      ▼                                              │
│  ┌─────────────────────────────────────────────────────────────────────────────┐   │
│  │                                                                             │   │
│  │                          PHASE 4: DEAUTH ATTACK (Optional)                  │   │
│  │                          ─────────────────────────────                      │   │
│  │                                                                             │   │
│  │  ┌─────────────────────────────────────────────────────────────────────┐   │   │
│  │  │  User presses 4 → StartDeauth()                                     │   │   │
│  │  │                    ↓                                                 │   │   │
│  │  │  SerialComm::StartDeauth(bssid)                                     │   │   │
│  │  │                    ↓                                                 │   │   │
│  │  │  Send MSG_DEAUTH_START with BSSID                                  │   │   │
│  │  │                    ↓                                                 │   │   │
│  │  │  ESP32 receives command                                             │   │   │
│  │  │                    ↓                                                 │   │   │
│  │  │  Enter promiscuous mode                                             │   │   │
│  │  │                    ↓                                                 │   │   │
│  │  │  Get list of connected clients                                      │   │   │
│  │  │                    ↓                                                 │   │   │
│  │  │  For each client: Send deauth packet                                │   │   │
│  │  │                    ↓                                                 │   │   │
│  │  │  Loop every 1 second                                                │   │   │
│  │  └─────────────────────────────────────────────────────────────────────┘   │   │
│  │                                                                             │   │
│  └─────────────────────────────────────────────────────────────────────────────┘   │
│                                      │                                              │
│                                      ▼                                              │
│  ┌─────────────────────────────────────────────────────────────────────────────┐   │
│  │                                                                             │   │
│  │                          PHASE 5: ATTACK TERMINATION                        │   │
│  │                          ───────────────────────                           │   │
│  │                                                                             │   │
│  │  ┌─────────────────────────────────────────────────────────────────────┐   │   │
│  │  │  User presses 3 → StopAttack()                                      │   │   │
│  │  │                    ↓                                                 │   │   │
│  │  │  SerialComm::StopAttack()                                            │   │   │
│  │  │                    ↓                                                 │   │   │
│  │  │  Send MSG_ATTACK_STOP                                               │   │   │
│  │  │                    ↓                                                 │   │   │
│  │  │  ESP32 receives command                                             │   │   │
│  │  │                    ↓                                                 │   │   │
│  │  │  webServer.stop()  // Stop HTTP server                             │   │   │
│  │  │                    ↓                                                 │   │   │
│  │  │  dnsServer.stop()  // Stop DNS server                              │   │   │
│  │  │                    ↓                                                 │   │   │
│  │  │  WiFi.softAPdisconnect()  // Stop AP mode                          │   │   │
│  │  │                    ↓                                                 │   │   │
│  │  │  WiFi.mode(WIFI_STA)  // Switch to station mode                    │   │   │
│  │  │                    ↓                                                 │   │   │
│  │  │  Send ACK back to Windows                                           │   │   │
│  │  │                    ↓                                                 │   │   │
│  │  │  Windows updates UI: Attack stopped                                 │   │   │
│  │  └─────────────────────────────────────────────────────────────────────┘   │   │
│  │                                                                             │   │
│  └─────────────────────────────────────────────────────────────────────────────┘   │
│                                                                                     │
└─────────────────────────────────────────────────────────────────────────────────────┘
```

---

## Data Flow Diagrams

### 1. Full System Data Flow

```
┌─────────────────────────────────────────────────────────────────────────────────────┐
│                          SYSTEM DATA FLOW                                            │
└─────────────────────────────────────────────────────────────────────────────────────┘

┌─────────────┐                              ┌─────────────┐
│             │                              │             │
│    USER     │                              │   VICTIM    │
│   (Admin)   │                              │  (Device)   │
│             │                              │             │
└──────┬──────┘                              └──────┬──────┘
       │                                            │
       │ 1. Press keys                              │ 6. Connects to fake AP
       │ 2. View output                            │ 7. Sees captive portal
       │                                            │ 8. Enters credentials
       ▼                                            ▼
┌─────────────────────────────────────────────────────────────────────┐
│                                                                     │
│  ┌─────────────────────────────────────────────────────────────┐   │
│  │                    WINDOWS APPLICATION                      │   │
│  │                                                             │   │
│  │  ┌───────────┐     ┌───────────┐     ┌───────────┐       │   │
│  │  │    UI     │────▶│  Attack   │────▶│  Serial   │       │   │
│  │  │  Module   │◄────│  Manager  │◄────│   Comm    │       │   │
│  │  └───────────┘     └───────────┘     └───────────┘       │   │
│  │        │                 │                 │               │   │
│  │        ▼                 ▼                 ▼               │   │
│  │  ┌───────────┐     ┌───────────┐     ┌───────────┐       │   │
│  │  │   WiFi    │     │  Logger   │     │ Captive   │       │   │
│  │  │  Scanner  │     │  Module   │     │  Portal   │       │   │
│  │  └───────────┘     └───────────┘     └───────────┘       │   │
│  │        │                 │                 │               │   │
│  └────────┼─────────────────┼─────────────────┼───────────────┘   │
│           │                 │                 │                    │
│           ▼                 ▼                 ▼                    │
└─────────────────────────────────────────────────────────────────────┘
       │                                            │
       │ 3. Scan results                           │ 9. Credentials
       │ 4. Status updates                         │
       │ 5. Attack commands                        │
       ▼                                            ▼
┌─────────────┐                              ┌─────────────┐
│             │                              │             │
│   ESP32     │                              │   Captured  │
│  Hardware   │                              │ Credentials │
│             │                              │             │
└─────────────┘                              └─────────────┘
       │
       │ 8. Deauth packets
       │
       ▼
┌─────────────┐
│             │
│   Target    │
│  Networks   │
│             │
└─────────────┘
```

### 2. Message Flow Between Components

```
┌─────────────────────────────────────────────────────────────────────────────────────┐
│                        MESSAGE FLOW DIAGRAM                                          │
└─────────────────────────────────────────────────────────────────────────────────────┘

     WINDOWS APP                              ESP32
         │                                      │
         │  1. MSG_HELLO (0x01)                │
         │─────────────────────────────────────▶│
         │                                      │
         │  2. MSG_ACK (0x0B)                  │
         │◀─────────────────────────────────────│
         │                                      │
         │  3. MSG_SCAN_START (0x02)           │
         │─────────────────────────────────────▶│
         │                                      │
         │  4. MSG_SCAN_RESULT (0x03)          │
         │◀─────────────────────────────────────│
         │  (Multiple results)                  │
         │                                      │
         │  5. MSG_ATTACK_START (0x04)         │
         │─────────────────────────────────────▶│
         │                                      │
         │  6. MSG_ACK (0x0B)                  │
         │◀─────────────────────────────────────│
         │                                      │
         │  7. MSG_DEAUTH_START (0x06)         │
         │─────────────────────────────────────▶│
         │                                      │
         │  8. MSG_ACK (0x0B)                  │
         │◀─────────────────────────────────────│
         │                                      │
         │  9. MSG_CREDENTIAL (0x08)           │
         │◀─────────────────────────────────────│
         │  (Username, Password, SSID, MAC)     │
         │                                      │
         │  10. MSG_STATUS (0x09)              │
         │◀─────────────────────────────────────│
         │  (Running, Clients, Creds, Deauth)   │
         │                                      │
         │  11. MSG_ATTACK_STOP (0x05)         │
         │─────────────────────────────────────▶│
         │                                      │
         │  12. MSG_ACK (0x0B)                 │
         │◀─────────────────────────────────────│
         │                                      │
```

### 3. Credential Flow

```
┌─────────────────────────────────────────────────────────────────────────────────────┐
│                         CREDENTIAL FLOW                                              │
└─────────────────────────────────────────────────────────────────────────────────────┘

     VICTIM DEVICE                          ESP32                           WINDOWS APP
         │                                    │                                  │
         │  1. Victim visits any URL          │                                  │
         │───────────────────────────────────▶│                                  │
         │                                    │                                  │
         │  2. DNS spoofing redirects         │                                  │
         │◀───────────────────────────────────│                                  │
         │  to captive portal                 │                                  │
         │                                    │                                  │
         │  3. Victim sees login page         │                                  │
         │◀───────────────────────────────────│                                  │
         │                                    │                                  │
         │  4. Victim enters credentials      │                                  │
         │  (username + password)             │                                  │
         │───────────────────────────────────▶│                                  │
         │                                    │                                  │
         │  5. ESP32 receives POST /login     │                                  │
         │     Extracts credentials           │                                  │
         │     Saves to EEPROM                │                                  │
         │                                    │                                  │
         │  6. ESP32 sends MSG_CREDENTIAL     │                                  │
         │───────────────────────────────────────────────────────────────────────▶│
         │                                    │                                  │
         │  7. ESP32 serves success page      │   8. Windows receives            │
         │◀───────────────────────────────────│      MSG_CREDENTIAL              │
         │                                    │                                  │
         │  9. Victim thinks they're          │   9. Parses credential data      │
         │     connected to internet          │                                  │
         │                                    │  10. Adds to vector             │
         │                                    │                                  │
         │                                    │  11. Logs to file               │
         │                                    │                                  │
         │                                    │  12. Updates UI display         │
         │                                    │                                  │
         │                                    │  13. Increments count           │
```

---

## User Interaction Flow

```
┌─────────────────────────────────────────────────────────────────────────────────────┐
│                         USER INTERACTION FLOW                                        │
└─────────────────────────────────────────────────────────────────────────────────────┘

┌─────────────────────────────────────────────────────────────────────────────────────┐
│                                                                                     │
│  ┌─────────────────────────────────────────────────────────────────────────────┐   │
│  │                                                                             │   │
│  │                          MAIN MENU                                          │   │
│  │                          ────────                                          │   │
│  │                                                                             │   │
│  │  ┌─────────────┐  ┌─────────────┐  ┌─────────────┐  ┌─────────────┐       │   │
│  │  │  Option 1   │  │  Option 2   │  │  Option 3   │  │  Option 4   │       │   │
│  │  │  Scan       │  │  Start      │  │  Stop       │  │  Deauth     │       │   │
│  │  │  Networks   │  │  Attack     │  │  Attack     │  │  Attack     │       │   │
│  │  └─────────────┘  └─────────────┘  └─────────────┘  └─────────────┘       │   │
│  │                                                                             │   │
│  │  ┌─────────────┐  ┌─────────────┐  ┌─────────────┐  ┌─────────────┐       │   │
│  │  │  Option 5   │  │  Option 6   │  │  Option 7   │  │  Option 8   │       │   │
│  │  │  View       │  │  Settings   │  │  Connect    │  │  Help       │       │   │
│  │  │  Creds      │  │             │  │  ESP32      │  │             │       │   │
│  │  └─────────────┘  └─────────────┘  └─────────────┘  └─────────────┘       │   │
│  │                                                                             │   │
│  │  ┌─────────────┐                                                          │   │
│  │  │  Option 9   │                                                          │   │
│  │  │  Exit       │                                                          │   │
│  │  └─────────────┘                                                          │   │
│  │                                                                             │   │
│  └─────────────────────────────────────────────────────────────────────────────┘   │
│                                      │                                              │
│                                      ▼                                              │
│  ┌─────────────────────────────────────────────────────────────────────────────┐   │
│  │                                                                             │   │
│  │                     OPTION 1: SCAN NETWORKS                                  │   │
│  │                     ────────────────────                                  │   │
│  │                                                                             │   │
│  │  User:    Press 1                                                          │   │
│  │            ↓                                                                │   │
│  │  System:  ScanNetworks()                                                    │   │
│  │            ↓                                                                │   │
│  │  System:  Display list of networks                                         │   │
│  │            ↓                                                                │   │
│  │  User:    Review networks                                                  │   │
│  │            ↓                                                                │   │
│  │  User:    Press any key to continue                                        │   │
│  │                                                                             │   │
│  └─────────────────────────────────────────────────────────────────────────────┘   │
│                                      │                                              │
│                                      ▼                                              │
│  ┌─────────────────────────────────────────────────────────────────────────────┐   │
│  │                                                                             │   │
│  │                     OPTION 2: START ATTACK                                   │   │
│  │                     ────────────────────                                  │   │
│  │                                                                             │   │
│  │  User:    Press 2                                                          │   │
│  │            ↓                                                                │   │
│  │  System:  Check prerequisites:                                             │   │
│  │            - Networks scanned?                                             │   │
│  │            - Attack not running?                                           │   │
│  │            - ESP32 connected?                                              │   │
│  │            ↓                                                                │   │
│  │  System:  Prompt: "Select target (1-X): "                                  │   │
│  │            ↓                                                                │   │
│  │  User:    Enter number                                                    │   │
│  │            ↓                                                                │   │
│  │  System:  StartAttack(ssid, bssid)                                        │   │
│  │            ↓                                                                │   │
│  │  System:  Display: "Attack started on [SSID]"                              │   │
│  │            ↓                                                                │   │
│  │  System:  Update status display                                            │   │
│  │                                                                             │   │
│  └─────────────────────────────────────────────────────────────────────────────┘   │
│                                      │                                              │
│                                      ▼                                              │
│  ┌─────────────────────────────────────────────────────────────────────────────┐   │
│  │                                                                             │   │
│  │                     OPTION 3: STOP ATTACK                                    │   │
│  │                     ────────────────────                                  │   │
│  │                                                                             │   │
│  │  User:    Press 3                                                          │   │
│  │            ↓                                                                │   │
│  │  System:  Check if attack is running                                       │   │
│  │            ↓                                                                │   │
│  │  System:  StopAttack()                                                     │   │
│  │            ↓                                                                │   │
│  │  System:  Display: "Attack stopped"                                        │   │
│  │            ↓                                                                │   │
│  │  System:  Update status display                                            │   │
│  │                                                                             │   │
│  └─────────────────────────────────────────────────────────────────────────────┘   │
│                                      │                                              │
│                                      ▼                                              │
│  ┌─────────────────────────────────────────────────────────────────────────────┐   │
│  │                                                                             │   │
│  │                     OPTION 4: DEAUTH ATTACK                                  │   │
│  │                     ────────────────────                                  │   │
│  │                                                                             │   │
│  │  User:    Press 4                                                          │   │
│  │            ↓                                                                │   │
│  │  System:  Check attack is running                                          │   │
│  │            ↓                                                                │   │
│  │  System:  Check if deauth already running                                  │   │
│  │            ↓                                                                │   │
│  │  System:  Prompt: "Start deauth on [BSSID]? (y/n)"                        │   │
│  │            ↓                                                                │   │
│  │  User:    Enter y or n                                                    │   │
│  │            ↓                                                                │   │
│  │  System:  If y: StartDeauth(bssid)                                        │   │
│  │            ↓                                                                │   │
│  │  System:  Display: "Deauth started"                                       │   │
│  │                                                                             │   │
│  └─────────────────────────────────────────────────────────────────────────────┘   │
│                                      │                                              │
│                                      ▼                                              │
│  ┌─────────────────────────────────────────────────────────────────────────────┐   │
│  │                                                                             │   │
│  │                     OPTION 5: VIEW CREDENTIALS                               │   │
│  │                     ─────────────────────                                  │   │
│  │                                                                             │   │
│  │  User:    Press 5                                                          │   │
│  │            ↓                                                                │   │
│  │  System:  Display table of credentials                                     │   │
│  │            ↓                                                                │   │
│  │  User:    Press 'e' to export                                             │   │
│  │            or 'c' to clear                                                 │   │
│  │            or any key to close                                             │   │
│  │            ↓                                                                │   │
│  │  System:  If 'e': Export to file                                          │   │
│  │            If 'c': Clear all credentials                                  │   │
│  │                                                                             │   │
│  └─────────────────────────────────────────────────────────────────────────────┘   │
│                                                                                     │
└─────────────────────────────────────────────────────────────────────────────────────┘
```

---

## ESP32 Firmware Flow

```
┌─────────────────────────────────────────────────────────────────────────────────────┐
│                         ESP32 FIRMWARE FLOW                                          │
└─────────────────────────────────────────────────────────────────────────────────────┘

┌─────────────────────────────────────────────────────────────────────────────────────┐
│                                                                                     │
│                          SETUP()                                                    │
│                          ───────                                                    │
│                                                                                     │
│  ┌─────────────────────────────────────────────────────────────────────────────┐   │
│  │  1. Initialize Serial (115200)                                              │   │
│  │  2. Initialize SPIFFS (File system)                                         │   │
│  │  3. Initialize EEPROM (Credential storage)                                  │   │
│  │  4. Setup LED pin                                                           │   │
│  │  5. WiFi.mode(WIFI_STA)                                                     │   │
│  │  6. Send MSG_HELLO to Windows                                               │   │
│  │  7. Wait for MSG_ACK                                                        │   │
│  │                                                                             │   │
│  │  ┌─────────────────────────────────────────────────────────────────────┐   │   │
│  │  │  Result: ESP32 ready and waiting for commands                      │   │   │
│  │  └─────────────────────────────────────────────────────────────────────┘   │   │
│  └─────────────────────────────────────────────────────────────────────────────┘   │
│                                                                                     │
└─────────────────────────────────────────────────────────────────────────────────────┘
                                      │
                                      ▼
┌─────────────────────────────────────────────────────────────────────────────────────┐
│                                                                                     │
│                          LOOP()                                                     │
│                          ────                                                     │
│                                                                                     │
│  ┌─────────────────────────────────────────────────────────────────────────────┐   │
│  │  1. Check Serial for incoming messages                                      │   │
│  │  2. Process any received messages                                           │   │
│  │  3. Handle Web Server requests                                              │   │
│  │  4. Handle DNS requests                                                     │   │
│  │  5. If attack running: Send status updates                                  │   │
│  │  6. If deauth running: Send deauth packets                                  │   │
│  └─────────────────────────────────────────────────────────────────────────────┘   │
│                                                                                     │
└─────────────────────────────────────────────────────────────────────────────────────┘
                                      │
                                      ▼
┌─────────────────────────────────────────────────────────────────────────────────────┐
│                                                                                     │
│                     SERIAL COMMAND HANDLING                                         │
│                     ──────────────────────                                         │
│                                                                                     │
│  ┌─────────────────────────────────────────────────────────────────────────────┐   │
│  │  Received: MSG_HELLO (0x01)                                                 │   │
│  │  → Send: MSG_ACK (0x0B)                                                    │   │
│  └─────────────────────────────────────────────────────────────────────────────┘   │
│                                      │                                              │
│                                      ▼                                              │
│  ┌─────────────────────────────────────────────────────────────────────────────┐   │
│  │  Received: MSG_SCAN_START (0x02)                                            │   │
│  │  → WiFi.scanNetworks(true)                                                  │   │
│  │  → Wait for scan complete                                                   │   │
│  │  → For each network: Send MSG_SCAN_RESULT                                  │   │
│  │  → Format: SSID\0BSSID\0Channel\0RSSI\0Security\0                         │   │
│  └─────────────────────────────────────────────────────────────────────────────┘   │
│                                      │                                              │
│                                      ▼                                              │
│  ┌─────────────────────────────────────────────────────────────────────────────┐   │
│  │  Received: MSG_ATTACK_START (0x04)                                          │   │
│  │  → Parse SSID and BSSID from data                                           │   │
│  │  → WiFi.mode(WIFI_AP)                                                       │   │
│  │  → WiFi.softAP(ssid.c_str())                                               │   │
│  │  → dnsServer.start(DNS_PORT, "*", apIP)                                    │   │
│  │  → webServer.begin()                                                        │   │
│  │  → Set attackRunning = true                                                │   │
│  │  → Send MSG_ACK                                                             │   │
│  └─────────────────────────────────────────────────────────────────────────────┘   │
│                                      │                                              │
│                                      ▼                                              │
│  ┌─────────────────────────────────────────────────────────────────────────────┐   │
│  │  Received: MSG_ATTACK_STOP (0x05)                                           │   │
│  │  → webServer.stop()                                                         │   │
│  │  → dnsServer.stop()                                                         │   │
│  │  → WiFi.softAPdisconnect()                                                  │   │
│  │  → WiFi.mode(WIFI_STA)                                                      │   │
│  │  → Set attackRunning = false                                               │   │
│  │  → Send MSG_ACK                                                             │   │
│  └─────────────────────────────────────────────────────────────────────────────┘   │
│                                      │                                              │
│                                      ▼                                              │
│  ┌─────────────────────────────────────────────────────────────────────────────┐   │
│  │  Received: MSG_DEAUTH_START (0x06)                                          │   │
│  │  → Parse BSSID from data                                                    │   │
│  │  → Set deauthRunning = true                                                │   │
│  │  → Send MSG_ACK                                                             │   │
│  └─────────────────────────────────────────────────────────────────────────────┘   │
│                                      │                                              │
│                                      ▼                                              │
│  ┌─────────────────────────────────────────────────────────────────────────────┐   │
│  │  Received: MSG_DEAUTH_STOP (0x07)                                           │   │
│  │  → Set deauthRunning = false                                               │   │
│  │  → Send MSG_ACK                                                             │   │
│  └─────────────────────────────────────────────────────────────────────────────┘   │
│                                      │                                              │
│                                      ▼                                              │
│  ┌─────────────────────────────────────────────────────────────────────────────┐   │
│  │  Received: MSG_STATUS (0x09)                                                │   │
│  │  → Build status: Running\0Clients\0Credentials\0DeauthPackets\0            │   │
│  │  → Send MSG_STATUS with data                                                │   │
│  └─────────────────────────────────────────────────────────────────────────────┘   │
│                                                                                     │
└─────────────────────────────────────────────────────────────────────────────────────┘
                                      │
                                      ▼
┌─────────────────────────────────────────────────────────────────────────────────────┐
│                                                                                     │
│                     CAPTIVE PORTAL HANDLING                                         │
│                     ─────────────────────                                         │
│                                                                                     │
│  ┌─────────────────────────────────────────────────────────────────────────────┐   │
│  │  Web Server Routes:                                                          │   │
│  │                                                                             │   │
│  │  GET / → Serve portal.html                                                  │   │
│  │  GET /style.css → Serve CSS                                                │   │
│  │  GET /script.js → Serve JavaScript                                          │   │
│  │  POST /login → Handle credentials                                          │   │
│  │  404 → Redirect to /                                                        │   │
│  └─────────────────────────────────────────────────────────────────────────────┘   │
│                                      │                                              │
│                                      ▼                                              │
│  ┌─────────────────────────────────────────────────────────────────────────────┐   │
│  │  POST /login Handling:                                                       │   │
│  │                                                                             │   │
│  │  1. Extract username from POST data                                         │   │
│  │  2. Extract password from POST data                                         │   │
│  │  3. Get client MAC address                                                  │   │
│  │  4. Save credential to EEPROM                                               │   │
│  │  5. Send MSG_CREDENTIAL via serial                                          │   │
│  │  6. Serve success page                                                      │   │
│  └─────────────────────────────────────────────────────────────────────────────┘   │
│                                                                                     │
└─────────────────────────────────────────────────────────────────────────────────────┘
                                      │
                                      ▼
┌─────────────────────────────────────────────────────────────────────────────────────┐
│                                                                                     │
│                     DEAUTH ENGINE                                                   │
│                     ─────────────                                                  │
│                                                                                     │
│  ┌─────────────────────────────────────────────────────────────────────────────┐   │
│  │  if (deauthRunning) {                                                        │   │
│  │                                                                             │   │
│  │    1. Get list of connected stations                                        │   │
│  │       wifi_sta_list_t stationList;                                         │   │
│  │       esp_wifi_ap_get_sta_list(&stationList);                              │   │
│  │                                                                             │   │
│  │    2. For each station:                                                     │   │
│  │       Build deauth frame:                                                   │   │
│  │         - Frame Control: 0xC0 (Deauth)                                    │   │
│  │         - Duration: 0                                                       │   │
│  │         - Destination: Client MAC                                           │   │
│  │         - Source: AP MAC                                                    │   │
│  │         - BSSID: AP MAC                                                     │   │
│  │         - Reason: 0x0004 (Inactivity)                                      │   │
│  │                                                                             │   │
│  │    3. Send packet via esp_wifi_80211_tx()                                  │   │
│  │                                                                             │   │
│  │    4. Increment deauth packet counter                                      │   │
│  │                                                                             │   │
│  │    5. Wait 1 second before next loop                                       │   │
│  │  }                                                                          │   │
│  └─────────────────────────────────────────────────────────────────────────────┘   │
│                                                                                     │
└─────────────────────────────────────────────────────────────────────────────────────┘
                                      │
                                      ▼
┌─────────────────────────────────────────────────────────────────────────────────────┐
│                                                                                     │
│                     CREDENTIAL STORAGE                                              │
│                     ─────────────────                                              │
│                                                                                     │
│  ┌─────────────────────────────────────────────────────────────────────────────┐   │
│  │  EEPROM Structure:                                                           │   │
│  │  Address 0: Credential count (max 10)                                       │   │
│  │  Address 4: StoredCredential[0]                                             │   │
│  │  Address 4+sizeof(StoredCredential): StoredCredential[1]                   │   │
│  │  ...                                                                         │   │
│  │                                                                             │   │
│  │  struct StoredCredential {                                                  │   │
│  │      char ssid[32];                                                         │   │
│  │      char username[32];                                                     │   │
│  │      char password[32];                                                     │   │
│  │      char clientMac[18];                                                    │   │
│  │      bool valid;                                                            │   │
│  │  };                                                                          │   │
│  └─────────────────────────────────────────────────────────────────────────────┘   │
│                                                                                     │
└─────────────────────────────────────────────────────────────────────────────────────┘
```

---

## Error Handling Flow

```
┌─────────────────────────────────────────────────────────────────────────────────────┐
│                         ERROR HANDLING FLOW                                          │
└─────────────────────────────────────────────────────────────────────────────────────┘

┌─────────────────────────────────────────────────────────────────────────────────────┐
│                                                                                     │
│                          ERROR OCCURS                                               │
│                          ───────────                                               │
│                                                                                     │
│  ┌─────────────────────────────────────────────────────────────────────────────┐   │
│  │  Error Types:                                                                │   │
│  │                                                                             │   │
│  │  1. Network Errors                                                          │   │
│  │     - No WiFi adapter                                                       │   │
│  │     - Scan failed                                                           │   │
│  │     - No networks found                                                     │   │
│  │                                                                             │   │
│  │  2. Serial Errors                                                           │   │
│  │     - COM port not found                                                    │   │
│  │     - Connection timeout                                                    │   │
│  │     - Data corruption                                                       │   │
│  │                                                                             │   │
│  │  3. ESP32 Errors                                                            │   │
│  │     - Firmware not responding                                               │   │
│  │     - Attack failed                                                         │   │
│  │     - Deauth failed                                                         │   │
│  │                                                                             │   │
│  │  4. System Errors                                                           │   │
│  │     - Out of memory                                                         │   │
│  │     - File system errors                                                    │   │
│  │     - Permission denied                                                     │   │
│  └─────────────────────────────────────────────────────────────────────────────┘   │
│                                      │                                              │
│                                      ▼                                              │
│  ┌─────────────────────────────────────────────────────────────────────────────┐   │
│  │                                                                             │   │
│  │                     ERROR HANDLING                                          │   │
│  │                     ─────────────                                          │   │
│  │                                                                             │   │
│  │  ┌─────────────────────────────────────────────────────────────────────┐   │   │
│  │  │  1. Log Error                                                        │   │   │
│  │  │     - Write to log file                                              │   │   │
│  │  │     - Include timestamp                                              │   │   │
│  │  │     - Include error code                                             │   │   │
│  │  │     - Include context                                                │   │   │
│  │  └─────────────────────────────────────────────────────────────────────┘   │   │
│  │                                      │                                      │   │
│  │                                      ▼                                      │   │
│  │  ┌─────────────────────────────────────────────────────────────────────┐   │   │
│  │  │  2. Display Error                                                    │   │   │
│  │  │     - Show user-friendly message                                     │   │   │
│  │  │     - Use UI_DisplayError()                                          │   │   │
│  │  │     - Wait for user input                                            │   │   │
│  │  └─────────────────────────────────────────────────────────────────────┘   │   │
│  │                                      │                                      │   │
│  │                                      ▼                                      │   │
│  │  ┌─────────────────────────────────────────────────────────────────────┐   │   │
│  │  │  3. Clean Up                                                        │   │   │
│  │  │     - Close open handles                                            │   │   │
│  │  │     - Free allocated memory                                         │   │   │
│  │  │     - Reset state variables                                         │   │   │
│  │  └─────────────────────────────────────────────────────────────────────┘   │   │
│  │                                      │                                      │   │
│  │                                      ▼                                      │   │
│  │  ┌─────────────────────────────────────────────────────────────────────┐   │   │
│  │  │  4. Return to Safe State                                            │   │   │
│  │  │     - Return to main menu                                           │   │   │
│  │  │     - Update UI status                                              │   │   │
│  │  │     - Allow retry                                                   │   │   │
│  │  └─────────────────────────────────────────────────────────────────────┘   │   │
│  │                                                                             │   │
│  └─────────────────────────────────────────────────────────────────────────────┘   │
│                                                                                     │
└─────────────────────────────────────────────────────────────────────────────────────┘

┌─────────────────────────────────────────────────────────────────────────────────────┐
│                                                                                     │
│                     SPECIFIC ERROR HANDLING                                         │
│                     ────────────────────                                          │
│                                                                                     │
│  ┌─────────────────────────────────────────────────────────────────────────────┐   │
│  │  Error: "No WiFi adapter found"                                             │   │
│  │  → Message: "No wireless adapters detected. Please check your hardware."    │   │
│  │  → Action: Return to menu                                                   │   │
│  └─────────────────────────────────────────────────────────────────────────────┘   │
│                                                                                     │
│  ┌─────────────────────────────────────────────────────────────────────────────┐   │
│  │  Error: "Failed to open COM port"                                           │   │
│  │  → Message: "Could not open COM port [port]. Check connection."             │   │
│  │  → Action: Ask user to check USB connection                                 │   │
│  └─────────────────────────────────────────────────────────────────────────────┘   │
│                                                                                     │
│  ┌─────────────────────────────────────────────────────────────────────────────┐   │
│  │  Error: "ESP32 not responding"                                              │   │
│  │  → Message: "ESP32 did not respond. Check firmware and power."             │   │
│  │  → Action: Disconnect and retry                                             │   │
│  └─────────────────────────────────────────────────────────────────────────────┘   │
│                                                                                     │
│  ┌─────────────────────────────────────────────────────────────────────────────┐   │
│  │  Error: "Attack failed to start"                                            │   │
│  │  → Message: "Attack could not be started. Check ESP32 connection."         │   │
│  │  → Action: Return to menu                                                   │   │
│  └─────────────────────────────────────────────────────────────────────────────┘   │
│                                                                                     │
│  ┌─────────────────────────────────────────────────────────────────────────────┐   │
│  │  Error: "No networks found"                                                 │   │
│  │  → Message: "No networks found. Make sure WiFi is enabled."                │   │
│  │  → Action: Allow user to retry scan                                         │   │
│  └─────────────────────────────────────────────────────────────────────────────┘   │
│                                                                                     │
└─────────────────────────────────────────────────────────────────────────────────────┘
```

---

## Credential Capture Flow

```
┌─────────────────────────────────────────────────────────────────────────────────────┐
│                         CREDENTIAL CAPTURE FLOW                                      │
└─────────────────────────────────────────────────────────────────────────────────────┘

┌─────────────────────────────────────────────────────────────────────────────────────┐
│                                                                                     │
│  ┌─────────────────────────────────────────────────────────────────────────────┐   │
│  │                                                                             │   │
│  │                     1. VICTIM SUBMITS FORM                                  │   │
│  │                     ────────────────────                                  │   │
│  │                                                                             │   │
│  │  ┌─────────────────────────────────────────────────────────────────────┐   │   │
│  │  │  Victim Device                                                        │   │   │
│  │  │  ────────────────                                                    │   │   │
│  │  │                                                                     │   │   │
│  │  │  POST /login                                                         │   │   │
│  │  │  Content-Type: application/x-www-form-urlencoded                    │   │   │
│  │  │                                                                     │   │   │
│  │  │  username=john_doe&password=password123                            │   │   │
│  │  └─────────────────────────────────────────────────────────────────────┘   │   │
│  │                                                                             │   │
│  └─────────────────────────────────────────────────────────────────────────────┘   │
│                                      │                                              │
│                                      ▼                                              │
│  ┌─────────────────────────────────────────────────────────────────────────────┐   │
│  │                                                                             │   │
│  │                     2. ESP32 PROCESSES DATA                                 │   │
│  │                     ─────────────────────                                  │   │
│  │                                                                             │   │
│  │  ┌─────────────────────────────────────────────────────────────────────┐   │   │
│  │  │  ESP32 Firmware                                                       │   │   │
│  │  │  ───────────────                                                    │   │   │
│  │  │                                                                     │   │   │
│  │  │  void handleLogin() {                                               │   │   │
│  │  │                                                                     │   │   │
│  │  │    1. String username = webServer.arg("username");                 │   │   │
│  │  │    2. String password = webServer.arg("password");                 │   │   │
│  │  │    3. String clientMac = webServer.client().remoteIP().toString();│   │   │
│  │  │                                                                     │   │   │
│  │  │    4. saveCredential(targetSSID, username, password, clientMac);  │   │   │
│  │  │    5. sendCredential(username, password);                          │   │   │
│  │  │    6. webServer.send(200, "text/html", successPage);              │   │   │
│  │  │  }                                                                  │   │   │
│  │  └─────────────────────────────────────────────────────────────────────┘   │   │
│  │                                                                             │   │
│  └─────────────────────────────────────────────────────────────────────────────┘   │
│                                      │                                              │
│                                      ▼                                              │
│  ┌─────────────────────────────────────────────────────────────────────────────┐   │
│  │                                                                             │   │
│  │                     3. ESP32 SENDS TO WINDOWS                               │   │
│  │                     ───────────────────────                               │   │
│  │                                                                             │   │
│  │  ┌─────────────────────────────────────────────────────────────────────┐   │   │
│  │  │  Serial Message                                                       │   │   │
│  │  │  ───────────────                                                    │   │   │
│  │  │                                                                     │   │   │
│  │  │  [0x08] [0x00] [0x1E] [Data]                                       │   │   │
│  │  │                                                                     │   │   │
│  │  │  Data Format:                                                       │   │   │
│  │  │  SSID\0Username\0Password\0ClientMAC\0                             │   │   │
│  │  │                                                                     │   │   │
│  │  │  Example:                                                           │   │   │
│  │  │  "HomeWiFi\0john_doe\0password123\0AA:BB:CC:DD:EE:FF\0"           │   │   │
│  │  └─────────────────────────────────────────────────────────────────────┘   │   │
│  │                                                                             │   │
│  └─────────────────────────────────────────────────────────────────────────────┘   │
│                                      │                                              │
│                                      ▼                                              │
│  ┌─────────────────────────────────────────────────────────────────────────────┐   │
│  │                                                                             │   │
│  │                     4. WINDOWS RECEIVES DATA                                │   │
│  │                     ──────────────────────                                │   │
│  │                                                                             │   │
│  │  ┌─────────────────────────────────────────────────────────────────────┐   │   │
│  │  │  Windows Application                                                  │   │   │
│  │  │  ──────────────────                                                 │   │   │
│  │  │                                                                     │   │   │
│  │  │  void processMessage(SerialMessage msg) {                           │   │   │
│  │  │    if (msg.type == MSG_CREDENTIAL) {                                │   │   │
│  │  │      1. Parse data: SSID, Username, Password, MAC                  │   │   │
│  │  │      2. Create Credential object                                    │   │   │
│  │  │      3. Add to credentials vector                                   │   │   │
│  │  │      4. Call credentialCallback(cred)                               │   │   │
│  │  │    }                                                                 │   │   │
│  │  │  }                                                                  │   │   │
│  │  └─────────────────────────────────────────────────────────────────────┘   │   │
│  │                                                                             │   │
│  └─────────────────────────────────────────────────────────────────────────────┘   │
│                                      │                                              │
│                                      ▼                                              │
│  ┌─────────────────────────────────────────────────────────────────────────────┐   │
│  │                                                                             │   │
│  │                     5. WINDOWS PROCESSES DATA                               │   │
│  │                     ──────────────────────                                │   │
│  │                                                                             │   │
│  │  ┌─────────────────────────────────────────────────────────────────────┐   │   │
│  │  │  Windows Application                                                  │   │   │
│  │  │  ──────────────────                                                 │   │   │
│  │  │                                                                     │   │   │
│  │  │  credentialCallback(cred) {                                         │   │   │
│  │  │                                                                     │   │   │
│  │  │    1. credentials.push_back(cred);                                  │   │   │
│  │  │    2. logger->LogCredential(cred);                                  │   │   │
│  │  │    3. attackStatus.credentialsCaptured++;                          │   │   │
│  │  │    4. UI_AddCredential(cred);                                      │   │   │
│  │  │    5. UI_UpdateStatus(attackStatus);                               │   │   │
│  │  │                                                                     │   │   │
│  │  │  }                                                                  │   │   │
│  │  └─────────────────────────────────────────────────────────────────────┘   │   │
│  │                                                                             │   │
│  └─────────────────────────────────────────────────────────────────────────────┘   │
│                                      │                                              │
│                                      ▼                                              │
│  ┌─────────────────────────────────────────────────────────────────────────────┐   │
│  │                                                                             │   │
│  │                     6. CREDENTIAL STORAGE & DISPLAY                         │   │
│  │                     ─────────────────────────────                         │   │
│  │                                                                             │   │
│  │  ┌─────────────────────────────────────────────────────────────────────┐   │   │
│  │  │  Storage:                                                             │   │   │
│  │  │  ─────────                                                           │   │   │
│  │  │                                                                     │   │   │
│  │  │  1. Memory: vector<Credential> credentials                         │   │   │
│  │  │  2. File: logs/evil_twin_YYYYMMDD_HHMMSS.log                       │   │   │
│  │  │  3. Export: credentials_YYYYMMDD_HHMMSS.txt                        │   │   │
│  │  └─────────────────────────────────────────────────────────────────────┘   │   │
│  │                                                                             │   │
│  │  ┌─────────────────────────────────────────────────────────────────────┐   │   │
│  │  │  Display:                                                             │   │   │
│  │  │  ────────                                                           │   │   │
│  │  │                                                                     │   │   │
│  │  │  ┌──────────────────────────────────────────────────────────────┐   │   │   │
│  │  │  │ Captured Credentials (3 total)                               │   │   │   │
│  │  │  ├──────────────────────────────────────────────────────────────┤   │   │   │
│  │  │  │ 1. john_doe    : password123  @ HomeWiFi  12:00            │   │   │   │
│  │  │  │ 2. jane_smith  : qwerty789    @ HomeWiFi  12:05            │   │   │   │
│  │  │  │ 3. admin       : admin2024    @ OfficeNet 12:10            │   │   │   │
│  │  │  └──────────────────────────────────────────────────────────────┘   │   │   │
│  │  └─────────────────────────────────────────────────────────────────────┘   │   │
│  │                                                                             │   │
│  └─────────────────────────────────────────────────────────────────────────────┘   │
│                                                                                     │
└─────────────────────────────────────────────────────────────────────────────────────┘
```

---

## Summary Flow (Simplified)

```
┌─────────────────────────────────────────────────────────────────────────────────────┐
│                         SIMPLIFIED OVERALL FLOW                                      │
└─────────────────────────────────────────────────────────────────────────────────────┘

     START
       │
       ▼
┌─────────────────────────────────────────────────────────────────────────────────────┐
│  1. User launches application                                                      │
│  2. Application initializes all modules                                           │
│  3. Main menu displayed                                                           │
└─────────────────────────────────────────────────────────────────────────────────────┘
       │
       ▼
┌─────────────────────────────────────────────────────────────────────────────────────┐
│  USER ACTION: Press 7 → Connect ESP32                                             │
│                                                                                     │
│  System: Opens COM port                                                            │
│  System: Sends HELLO message                                                       │
│  System: Waits for ACK                                                             │
│  System: Connection established                                                    │
└─────────────────────────────────────────────────────────────────────────────────────┘
       │
       ▼
┌─────────────────────────────────────────────────────────────────────────────────────┐
│  USER ACTION: Press 1 → Scan Networks                                             │
│                                                                                     │
│  System: WlanOpenHandle()                                                          │
│  System: WlanEnumInterfaces()                                                      │
│  System: For each interface: WlanScan() + WlanGetNetworkBssList()                 │
│  System: Parse and display networks                                                │
└─────────────────────────────────────────────────────────────────────────────────────┘
       │
       ▼
┌─────────────────────────────────────────────────────────────────────────────────────┐
│  USER ACTION: Press 2 → Start Attack                                              │
│                                                                                     │
│  System: Prompt: Select target network                                             │
│  User: Enter network number                                                        │
│  System: SerialComm::StartAttack(ssid, bssid)                                     │
│  System: ESP32 creates fake AP + DNS spoofer + web server                         │
│  System: Update status: "Attack Running"                                          │
└─────────────────────────────────────────────────────────────────────────────────────┘
       │
       ▼
┌─────────────────────────────────────────────────────────────────────────────────────┐
│  VICTIM CONNECTS                                                                   │
│                                                                                     │
│  Victim sees fake AP with target SSID                                              │
│  Victim connects to fake AP                                                        │
│  Victim opens browser                                                              │
│  DNS spoofing redirects to captive portal                                          │
│  Victim sees login page                                                            │
│  Victim enters credentials                                                         │
└─────────────────────────────────────────────────────────────────────────────────────┘
       │
       ▼
┌─────────────────────────────────────────────────────────────────────────────────────┐
│  CREDENTIALS CAPTURED                                                              │
│                                                                                     │
│  ESP32 receives POST /login                                                        │
│  ESP32 extracts username and password                                              │
│  ESP32 sends MSG_CREDENTIAL via serial                                             │
│  Windows receives MSG_CREDENTIAL                                                   │
│  Windows adds to credentials vector                                                │
│  Windows logs to file                                                              │
│  Windows updates UI display                                                        │
│  ESP32 serves success page to victim                                               │
└─────────────────────────────────────────────────────────────────────────────────────┘
       │
       ▼
┌─────────────────────────────────────────────────────────────────────────────────────┐
│  OPTIONAL: DEAUTH ATTACK                                                          │
│                                                                                     │
│  USER ACTION: Press 4 → Start Deauth                                              │
│  System: SerialComm::StartDeauth(bssid)                                           │
│  System: ESP32 sends deauth packets to connected clients                          │
│  Victims disconnect from real AP                                                  │
│  Victims reconnect to fake AP                                                     │
│  More credentials captured                                                         │
└─────────────────────────────────────────────────────────────────────────────────────┘
       │
       ▼
┌─────────────────────────────────────────────────────────────────────────────────────┐
│  USER ACTION: Press 3 → Stop Attack                                               │
│                                                                                     │
│  System: SerialComm::StopAttack()                                                  │
│  System: ESP32 stops AP, DNS, web server                                          │
│  System: Update status: "Attack Stopped"                                          │
└─────────────────────────────────────────────────────────────────────────────────────┘
       │
       ▼
┌─────────────────────────────────────────────────────────────────────────────────────┐
│  USER ACTION: Press 5 → View Credentials                                          │
│                                                                                     │
│  System: Display table of captured credentials                                    │
│  User: Press 'e' to export or 'c' to clear                                        │
└─────────────────────────────────────────────────────────────────────────────────────┘
       │
       ▼
┌─────────────────────────────────────────────────────────────────────────────────────┐
│  USER ACTION: Press 9 → Exit                                                      │
│                                                                                     │
│  System: Clean up resources                                                        │
│  System: Close COM port                                                            │
│  System: Close log files                                                           │
│  System: Exit application                                                          │
└─────────────────────────────────────────────────────────────────────────────────────┘
       │
       ▼
      END
```
