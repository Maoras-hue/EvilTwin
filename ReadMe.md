# Evil Twin Attack Tool

## Comprehensive Documentation

---

## Table of Contents

1. [Overview](#overview)
2. [Features](#features)
3. [Architecture](#architecture)
4. [System Requirements](#system-requirements)
5. [Hardware Requirements](#hardware-requirements)
6. [Installation Guide](#installation-guide)
7. [Building from Source](#building-from-source)
8. [Configuration](#configuration)
9. [Usage Guide](#usage-guide)
10. [ESP32 Firmware](#esp32-firmware)
11. [API Reference](#api-reference)
12. [Troubleshooting](#troubleshooting)
13. [Legal Disclaimer](#legal-disclaimer)
14. [Contributing](#contributing)
15. [License](#license)

---

## Overview

The Evil Twin Attack Tool is a comprehensive security testing application designed for educational and authorized penetration testing purposes. It implements a WiFi Evil Twin attack where a malicious access point is created with the same SSID as a legitimate network to capture user credentials through a captive portal.

### What is an Evil Twin Attack?

An Evil Twin attack is a type of wireless network attack where a rogue access point is set up to mimic a legitimate WiFi network. When users connect to the fake network, they are presented with a captive portal page that requests their credentials. The tool captures these credentials and logs them for analysis.

### Use Cases

- Security auditing of wireless networks
- Educational demonstrations of WiFi vulnerabilities
- Penetration testing with explicit authorization
- Security awareness training
- Network security research

---

## Features

### Core Features

| Feature | Description | Status |
|---------|-------------|--------|
| WiFi Network Scanning | Detects nearby wireless networks using Windows Native WiFi API | Implemented |
| Fake Access Point Creation | Creates a rogue AP with same SSID as target network | Implemented |
| Captive Portal | Serves a fake login page to capture credentials | Implemented |
| Deauthentication Attack | Disconnects clients from legitimate network | Implemented |
| Credential Logging | Captures and stores submitted credentials | Implemented |
| ESP32 Hardware Integration | Controls ESP32-based attack hardware | Implemented |
| Serial Communication | Communicates with ESP32 via USB serial | Implemented |
| Cross-Platform Build | Compiles for Windows from Linux | Implemented |

### User Interface Features

- Interactive console menu system
- Real-time attack status display
- Network scanning with detailed information
- Credential viewing and export
- Configuration management
- Help system with legal disclaimer
- Color-coded status indicators
- Progress indicators for long operations

### Security Features

- Credential encryption in transit
- Secure logging with timestamps
- Access control warnings
- Legal disclaimer display
- Unauthorized use prevention notices

---

## Architecture

### System Architecture Diagram

```
┌─────────────────────────────────────────────────────────────────────────────────────┐
│                                                                                     │
│                              EVIL TWIN ATTACK TOOL                                  │
│                                                                                     │
│  ┌─────────────────────────────────────────────────────────────────────────────┐   │
│  │                          WINDOWS APPLICATION                                │   │
│  │                                                                             │   │
│  │  ┌──────────────┐  ┌──────────────┐  ┌──────────────┐  ┌──────────────┐   │   │
│  │  │     UI       │  │   WiFi       │  │   Serial     │  │   Captive    │   │   │
│  │  │   Module     │  │   Scanner    │  │   Comm       │  │   Portal     │   │   │
│  │  └──────────────┘  └──────────────┘  └──────────────┘  └──────────────┘   │   │
│  │                                                                             │   │
│  │  ┌──────────────┐  ┌──────────────┐  ┌──────────────┐                     │   │
│  │  │   Logger     │  │   Attack     │  │   Config     │                     │   │
│  │  │   Module     │  │   Manager    │  │   Manager    │                     │   │
│  │  └──────────────┘  └──────────────┘  └──────────────┘                     │   │
│  └─────────────────────────────────────────────────────────────────────────────┘   │
│                                     │                                               │
│                                     ▼                                               │
│                          ┌──────────────────────┐                                  │
│                          │   Serial Connection   │                                  │
│                          │   (USB / COM Port)   │                                  │
│                          └──────────────────────┘                                  │
│                                     │                                               │
│                                     ▼                                               │
│  ┌─────────────────────────────────────────────────────────────────────────────┐   │
│  │                             ESP32 HARDWARE                                  │   │
│  │                                                                             │   │
│  │  ┌──────────────┐  ┌──────────────┐  ┌──────────────┐  ┌──────────────┐   │   │
│  │  │   WiFi AP    │  │   DNS        │  │   Web        │  │   Deauth     │   │   │
│  │  │   Mode       │  │   Spoofer    │  │   Server     │  │   Engine     │   │   │
│  │  └──────────────┘  └──────────────┘  └──────────────┘  └──────────────┘   │   │
│  └─────────────────────────────────────────────────────────────────────────────┘   │
│                                                                                     │
└─────────────────────────────────────────────────────────────────────────────────────┘
```

### Component Interaction Flow

```
[User] → [UI Module] → [Attack Manager] → [ESP32] → [Victim Devices]
                  ↓
           [WiFi Scanner] ← [Networks]
                  ↓
           [Captive Portal] ← [Credentials]
                  ↓
           [Logger] ← [Log Storage]
```

### Directory Structure

```
EvilTwin/
├── src/
│   ├── main.cpp              # Application entry point
│   ├── WiFiScanner.cpp       # WiFi scanning implementation
│   ├── SerialComm.cpp        # Serial communication
│   ├── CaptivePortal.cpp     # Captive portal server
│   ├── UI.cpp                # User interface
│   └── Logger.cpp            # Logging functionality
├── include/
│   ├── EvilTwin.h            # Main header file
│   ├── WiFiScanner.h         # WiFi scanner interface
│   ├── SerialComm.h          # Serial comm interface
│   ├── CaptivePortal.h       # Captive portal interface
│   ├── UI.h                  # UI interface
│   └── Logger.h              # Logger interface
├── firmware/
│   └── esp32/
│       ├── EvilTwin.ino      # ESP32 firmware
│       └── data/
│           ├── portal.html   # Captive portal HTML
│           ├── style.css     # CSS styles
│           └── script.js     # JavaScript
├── build/                    # Build output directory
├── bin/                      # Executable output
├── logs/                     # Log files
├── CMakeLists.txt            # CMake build configuration
├── Makefile                  # Make build configuration
└── README.md                 # This documentation
```

---

## System Requirements

### Windows Target System

| Component | Minimum | Recommended |
|-----------|---------|-------------|
| Operating System | Windows 7 SP1 | Windows 10/11 |
| Architecture | x64 | x64 |
| RAM | 2 GB | 4 GB |
| Storage | 50 MB | 100 MB |
| WiFi Adapter | 802.11n | 802.11ac |
| .NET Framework | 4.5+ | 4.8+ |
| Admin Privileges | Required | Required |

### Build System (Linux/Kali)

| Component | Minimum | Recommended |
|-----------|---------|-------------|
| Operating System | Ubuntu 20.04 | Kali Linux 2023+ |
| Architecture | x64 | x64 |
| RAM | 2 GB | 4 GB |
| Storage | 1 GB | 2 GB |
| Build Tools | CMake 3.10+ | CMake 3.20+ |
| Compiler | GCC 9+ | GCC 12+ |

### Development Tools

- CMake 3.10 or higher
- Make 4.0 or higher
- MinGW-w64 8.0 or higher
- GCC 9.0 or higher
- Git (optional)

---

## Hardware Requirements

### ESP32 Hardware

| Component | Specification |
|-----------|---------------|
| Microcontroller | ESP32 (any variant) |
| Flash Memory | 4 MB minimum |
| RAM | 520 KB SRAM |
| WiFi | 802.11 b/g/n |
| Bluetooth | BLE 4.2+ |
| USB | USB-to-Serial (CP2102, CH340) |
| Power | 5V via USB |

### Recommended ESP32 Boards

- ESP32-DevKitC
- ESP32-WROOM-32
- ESP32-WROVER
- NodeMCU-32S
- DOIT ESP32 DEVKIT V1

### Additional Hardware

| Component | Purpose |
|-----------|---------|
| USB Cable | Connect ESP32 to computer |
| WiFi Adapter | Network scanning (Windows) |
| External Antenna | Improved WiFi range (optional) |
| Battery Pack | Portable operation (optional) |

---

## Installation Guide

### Windows Installation

1. **Download the Executable**
   ```
   Download EvilTwin.exe from the release page
   ```

2. **Place in Directory**
   ```
   Create a folder: C:\Program Files\EvilTwin\
   Copy EvilTwin.exe to this folder
   ```

3. **Run as Administrator**
   ```
   Right-click EvilTwin.exe → Run as administrator
   ```

4. **First Time Setup**
   ```
   - Allow Windows firewall access
   - The program will create a logs/ folder automatically
   ```

### Linux Build Installation (for cross-compilation)

1. **Install Dependencies**
   ```bash
   sudo apt update
   sudo apt install -y g++-mingw-w64-x86-64 \
                       gcc-mingw-w64-x86-64 \
                       mingw-w64-tools \
                       cmake \
                       make \
                       git
   ```

2. **Clone Repository**
   ```bash
   git clone https://github.com/yourusername/EvilTwin.git
   cd EvilTwin
   ```

3. **Build the Tool**
   ```bash
   chmod +x build.sh
   ./build.sh
   ```

4. **Find the Executable**
   ```bash
   ls -l bin/EvilTwin.exe
   ```

---

## Building from Source

### Using CMake (Recommended)

```bash
# Clean previous builds
rm -rf build bin obj

# Create build directory
mkdir -p build
cd build

# Configure with CMake
cmake .. \
    -DCMAKE_SYSTEM_NAME=Windows \
    -DCMAKE_CXX_COMPILER=x86_64-w64-mingw32-g++ \
    -DCMAKE_C_COMPILER=x86_64-w64-mingw32-gcc \
    -DCMAKE_RC_COMPILER=x86_64-w64-mingw32-windres

# Build
make -j$(nproc)

# Copy executable
cp bin/EvilTwin.exe ../bin/
```

### Using Makefile

```bash
# Build all
make

# Build with debug symbols
make debug

# Build with optimizations
make release

# Clean build artifacts
make clean

# Test with Wine
make run
```

### Using the Build Script

```bash
# Standard build
./build.sh

# Clean build
./build.sh -c

# Quick build (no CMake)
./build.sh -q

# Build and test
./build.sh -c -t

# Show help
./build.sh -h
```

### Build Options

| Option | Description |
|--------|-------------|
| -c, --clean | Clean build directories before building |
| -m, --manual | Build using manual compilation (not CMake) |
| -q, --quick | Quick build (one command, no CMake) |
| -t, --test | Test the executable after building |
| -v, --verbose | Verbose output |
| -h, --help | Show help message |

---

## Configuration

### Application Settings

| Setting | Description | Default |
|---------|-------------|---------|
| Serial Port | COM port for ESP32 connection | COM3 |
| Baud Rate | Serial communication speed | 115200 |
| Scan Interval | WiFi scan interval in seconds | 5 |
| Auto Deauth | Automatic deauthentication | On |
| Log Path | Directory for log files | logs/ |
| Max Clients | Maximum clients to track | 10 |

### Configuration File

The tool uses a configuration file `evil_twin.conf` in the application directory:

```ini
[Serial]
Port = COM3
BaudRate = 115200

[Scan]
Interval = 5

[Attack]
AutoDeauth = true
MaxClients = 10

[Logging]
Path = logs/
Level = Info
```

### Environment Variables

| Variable | Description |
|----------|-------------|
| EVIL_TWIN_PORT | Override serial port |
| EVIL_TWIN_BAUD | Override baud rate |
| EVIL_TWIN_LOG | Override log path |
| EVIL_TWIN_DEBUG | Enable debug output |

---

## Usage Guide

### Quick Start

1. **Connect ESP32 Hardware**
   ```
   - Connect ESP32 via USB
   - Ensure ESP32 firmware is flashed
   - Note the COM port (e.g., COM3)
   ```

2. **Launch the Application**
   ```
   - Run EvilTwin.exe as Administrator
   - The main menu will appear
   ```

3. **Connect to ESP32 (Option 7)**
   ```
   - Enter the COM port (e.g., COM3)
   - Wait for connection confirmation
   ```

4. **Scan for Networks (Option 1)**
   ```
   - Press 1 to scan
   - Wait for scan to complete
   - Review detected networks
   ```

5. **Start Attack (Option 2)**
   ```
   - Select target network number
   - Confirm attack start
   - Monitor status display
   ```

6. **View Credentials (Option 5)**
   ```
   - Press 5 to view captured credentials
   - Export or clear as needed
   ```

### Main Menu Options

#### Option 1: Scan Networks
Scans for nearby WiFi networks using the Windows Native WiFi API.

**Usage:**
```
Press 1
Wait for scan completion
Review network list with details:
  - SSID (Network name)
  - BSSID (MAC address)
  - Channel
  - RSSI (Signal strength)
  - Security type
```

#### Option 2: Start Attack
Initiates the Evil Twin attack on a selected network.

**Prerequisites:**
- ESP32 must be connected
- Networks must be scanned
- No attack currently running

**Usage:**
```
Press 2
Select target network number
Confirm attack start
Monitor attack status
```

#### Option 3: Stop Attack
Stops the currently running attack.

**Usage:**
```
Press 3
Attack is stopped immediately
ESP32 returns to station mode
```

#### Option 4: Deauth Attack
Sends deauthentication packets to disconnect clients from the legitimate network.

**Usage:**
```
Press 4
Confirm deauth start
Clients are disconnected
Press 4 again to stop deauth
```

#### Option 5: View Credentials
Displays captured credentials in a table format.

**Usage:**
```
Press 5
View captured credentials
Press 'e' to export to file
Press 'c' to clear all
Press any other key to close
```

#### Option 6: Settings
Configure application settings.

**Usage:**
```
Press 6
Select setting number to change
Enter new value
Settings are saved automatically
```

#### Option 7: Connect ESP32
Connect or disconnect the ESP32 hardware.

**Usage:**
```
Press 7
Enter COM port (if disconnected)
Confirm connection
OR
Press 7 to disconnect
```

#### Option 8: Help
Display help menu with legal disclaimer.

**Usage:**
```
Press 8
Read help information
Press any key to return
```

#### Option 9: Exit
Exit the application.

**Usage:**
```
Press 9
Application cleans up and exits
```

### Attack Flow

1. **Scan Phase**
   ```
   User scans for networks
   Target network is identified
   Network details are logged
   ```

2. **Attack Phase**
   ```
   ESP32 creates fake AP
   Captive portal is served
   Victims connect to fake AP
   Credentials are captured
   ```

3. **Deauth Phase (Optional)**
   ```
   ESP32 sends deauth packets
   Victims disconnect from real AP
   Victims reconnect to fake AP
   More credentials are captured
   ```

4. **Credential Phase**
   ```
   Victims enter credentials
   Credentials are captured
   Credentials are logged
   Credentials can be exported
   ```

---

## ESP32 Firmware

### Flashing Instructions

#### Using PlatformIO (Recommended)

```bash
# Navigate to firmware directory
cd firmware/esp32

# Install dependencies
pio lib install

# Build firmware
pio run

# Upload to ESP32
pio run -t upload

# Upload filesystem (HTML, CSS, JS)
pio run -t uploadfs
```

#### Using Arduino IDE

1. **Install ESP32 Board Package**
   ```
   File → Preferences → Additional Boards Manager URLs
   Add: https://espressif.github.io/arduino-esp32/package_esp32_index.json
   Tools → Board → Boards Manager → Install "esp32"
   ```

2. **Select Board**
   ```
   Tools → Board → ESP32 Dev Module
   ```

3. **Upload Firmware**
   ```
   Open EvilTwin.ino
   Select port
   Click Upload
   ```

4. **Upload Filesystem**
   ```
   Tools → ESP32 Sketch Data Upload
   ```

### Firmware Features

| Feature | Description |
|---------|-------------|
| SoftAP Mode | Creates fake access point |
| Captive Portal | Serves login page |
| DNS Spoofing | Redirects all DNS requests |
| Web Server | Handles HTTP requests |
| Deauth Engine | Sends deauth packets |
| Serial Protocol | Communicates with Windows app |
| EEPROM Storage | Saves captured credentials |
| LED Indicator | Shows attack status |

### Serial Protocol

The ESP32 communicates with the Windows application using a custom binary protocol over USB serial.

#### Message Format

```
[Type: 1 byte] [Length: 2 bytes] [Data: N bytes]
```

#### Message Types

| Type | Value | Direction | Description |
|------|-------|-----------|-------------|
| HELLO | 0x01 | Both | Handshake message |
| SCAN_START | 0x02 | PC→ESP | Start WiFi scan |
| SCAN_RESULT | 0x03 | ESP→PC | Scan results |
| ATTACK_START | 0x04 | PC→ESP | Start attack |
| ATTACK_STOP | 0x05 | PC→ESP | Stop attack |
| DEAUTH_START | 0x06 | PC→ESP | Start deauth |
| DEAUTH_STOP | 0x07 | PC→ESP | Stop deauth |
| CREDENTIAL | 0x08 | ESP→PC | Captured credential |
| STATUS | 0x09 | ESP→PC | Status update |
| ERROR | 0x0A | ESP→PC | Error report |
| ACK | 0x0B | ESP→PC | Command acknowledged |
| NACK | 0x0C | ESP→PC | Command rejected |
| SCAN_STOP | 0x0D | PC→ESP | Stop scanning |

#### Example Message

```
HELLO Message:
[0x01] [0x00] [0x00]

SCAN_RESULT Message:
[0x03] [0x00] [0x1A] "HomeWiFi\0AA:BB:CC:DD:EE:FF\06\0-45\0WPA2-PSK\0"

CREDENTIAL Message:
[0x08] [0x00] [0x1E] "HomeWiFi\0user123\0pass456\0AA:BB:CC:DD:EE:FF\0"
```

---

## API Reference

### WiFiScanner Class

```cpp
class WiFiScanner {
public:
    virtual bool Initialize(AppConfig* cfg = nullptr) = 0;
    virtual void Cleanup() = 0;
    virtual std::vector<WiFiNetwork> ScanNetworks() = 0;
    virtual bool ScanNetworksAsync(NetworkListCallback callback) = 0;
    virtual void StopScanning() = 0;
    virtual bool GetNetworkClients(const std::string& bssid, int& clientCount) = 0;
};
```

#### Methods

| Method | Description | Parameters | Returns |
|--------|-------------|------------|---------|
| Initialize | Initialize WiFi scanner | AppConfig* cfg | bool |
| Cleanup | Clean up resources | None | void |
| ScanNetworks | Perform a network scan | None | vector<WiFiNetwork> |
| ScanNetworksAsync | Scan continuously | NetworkListCallback | bool |
| StopScanning | Stop continuous scanning | None | void |
| GetNetworkClients | Get connected clients | bssid, clientCount | bool |

### SerialComm Class

```cpp
class SerialComm {
public:
    virtual bool Connect(const std::string& port, int baud = 115200) = 0;
    virtual void Disconnect() = 0;
    virtual bool IsConnected() const = 0;
    virtual bool SendMessage(const SerialMessage& msg) = 0;
    virtual bool SendCommand(SerialMessageType type, const std::vector<uint8_t>& data = {}) = 0;
    virtual void SetCredentialCallback(CredentialCallback callback) = 0;
    virtual void SetStatusCallback(StatusUpdateCallback callback) = 0;
    virtual void SetLogCallback(LogCallback callback) = 0;
};
```

### CaptivePortal Class

```cpp
class CaptivePortal {
public:
    virtual bool Start(const std::string& ip, int port = 80) = 0;
    virtual void Stop() = 0;
    virtual bool IsRunning() const = 0;
    virtual bool SetContent(const std::string& html) = 0;
    virtual void SetCredentialCallback(CredentialCallback callback) = 0;
    virtual std::vector<Credential> GetCredentials() = 0;
    virtual void ClearCredentials() = 0;
};
```

### Logger Class

```cpp
class Logger {
public:
    virtual bool Initialize(const std::string& path = "logs/") = 0;
    virtual void Shutdown() = 0;
    virtual void Log(const std::string& message, bool toConsole = true) = 0;
    virtual void LogCredential(const Credential& cred) = 0;
    virtual void LogNetwork(const WiFiNetwork& network) = 0;
    virtual void LogError(const std::string& error) = 0;
    virtual void LogInfo(const std::string& info) = 0;
    virtual void LogWarning(const std::string& warning) = 0;
};
```

---

## Troubleshooting

### Common Issues

#### Issue 1: WiFi Scanner Returns No Networks

**Symptoms:**
- Option 1 shows "No networks scanned"
- Scan completes but network list is empty
- Debug output shows 0 networks found

**Solutions:**
1. Ensure you're running on actual Windows (not Wine/Linux)
2. Run the application as Administrator
3. Check if WiFi adapter is enabled
4. Verify WiFi adapter supports scanning
5. Try disabling and re-enabling WiFi adapter

#### Issue 2: ESP32 Not Detected

**Symptoms:**
- Option 7 fails to connect
- "Failed to open COM port" error
- ESP32 not responding

**Solutions:**
1. Check USB cable connection
2. Verify correct COM port number
3. Install USB-to-Serial drivers (CP2102, CH340)
4. Power cycle ESP32
5. Check if ESP32 is in bootloader mode
6. Verify firmware is correctly flashed

#### Issue 3: Attack Not Starting

**Symptoms:**
- Option 2 shows error
- ESP32 not creating AP
- Captive portal not accessible

**Solutions:**
1. Ensure ESP32 is connected
2. Verify target network is valid
3. Check ESP32 has WiFi capabilities
4. Verify ESP32 firmware is correct
5. Check serial communication

#### Issue 4: Captive Portal Not Working

**Symptoms:**
- No login page displayed
- DNS not redirecting
- Credentials not captured

**Solutions:**
1. Verify ESP32 is in AP mode
2. Check if DNS server is running
3. Verify web server is running
4. Ensure captive portal content is loaded
5. Check SPIFFS filesystem

#### Issue 5: Compilation Errors

**Symptoms:**
- Build fails with errors
- Missing headers
- Undefined references

**Solutions:**
1. Install required packages:
   ```bash
   sudo apt install g++-mingw-w64-x86-64 gcc-mingw-w64-x86-64 mingw-w64-tools cmake make
   ```
2. Clean build directory:
   ```bash
   make clean
   ```
3. Check MinGW version:
   ```bash
   x86_64-w64-mingw32-g++ --version
   ```

### Debug Output

Enable debug output to diagnose issues:

```bash
# Enable debug in Makefile
make debug

# Or build with debug flag
./build.sh -c -t

# On Windows, run with debug output
EvilTwin.exe --debug
```

### Log Files

Logs are stored in the `logs/` directory:

```
logs/
├── evil_twin_20240101_120000.log  # Main log file
├── credentials_20240101_120000.txt  # Credential export
└── error_20240101_120000.log      # Error log
```

### Windows Event Logs

The application logs to Windows Event Viewer:

```
Event Viewer → Windows Logs → Application
Source: EvilTwin
```

---

## Legal Disclaimer

### Important Notice

This software is provided for EDUCATIONAL PURPOSES ONLY. It is designed to demonstrate WiFi security vulnerabilities in controlled environments with explicit authorization.

### Authorized Uses

The following uses are permitted:

1. Testing your own wireless networks
2. Penetration testing with written permission
3. Security research in controlled environments
4. Educational demonstrations
5. Security awareness training

### Unauthorized Uses

The following uses are PROHIBITED:

1. Accessing networks without permission
2. Stealing credentials or personal information
3. Any illegal activity
4. Use on networks you do not own
5. Use without explicit written authorization

### Liability

THE AUTHOR IS NOT RESPONSIBLE FOR ANY MISUSE OF THIS SOFTWARE. By using this software, you agree to:

1. Use it only for lawful purposes
2. Obtain proper authorization before testing
3. Accept full responsibility for your actions
4. Comply with all applicable laws

### Warning

Unauthorized use of this tool may violate:

- Computer Fraud and Abuse Act (CFAA)
- Electronic Communications Privacy Act (ECPA)
- Local and international cybercrime laws
- Terms of service agreements
- Privacy laws and regulations

---

## Contributing

### Guidelines

1. **Fork the Repository**
   ```
   Click "Fork" on GitHub
   ```

2. **Create a Feature Branch**
   ```bash
   git checkout -b feature/your-feature
   ```

3. **Make Changes**
   - Follow coding standards
   - Add comments for clarity
   - Update documentation
   - Test thoroughly

4. **Commit Changes**
   ```bash
   git add .
   git commit -m "Description of changes"
   ```

5. **Push to Fork**
   ```bash
   git push origin feature/your-feature
   ```

6. **Create Pull Request**
   ```
   Click "New Pull Request" on GitHub
   ```

### Coding Standards

#### Naming Conventions

| Type | Convention | Example |
|------|------------|---------|
| Classes | PascalCase | WiFiScanner |
| Methods | PascalCase | ScanNetworks |
| Variables | camelCase | networkList |
| Constants | UPPER_SNAKE | MAX_RETRIES |
| Enums | PascalCase | SerialMessageType |

#### Code Style

```cpp
// Header guards
#pragma once

// Include order
#include <system>
#include "local"

// Class structure
class ClassName {
private:
    // Private members
    int privateVar;
    
public:
    // Public methods
    void publicMethod();
    
protected:
    // Protected members
    int protectedVar;
};
```

### Testing

#### Unit Tests

```bash
# Build and run tests
make test
```

#### Integration Tests

```bash
# Test with Wine
make run
```

---

## License

### Educational Use License

This software is provided under the following terms:

1. **Educational Use Only**: The software may only be used for educational purposes, security research, or authorized testing.

2. **No Commercial Use**: The software may not be used for commercial purposes without explicit permission.

3. **No Warranty**: The software is provided "as is" without warranty of any kind.

4. **Attribution**: Appropriate attribution must be given when using or referencing this software.

5. **No Liability**: The author is not liable for any damages arising from the use of this software.

### Third-Party Licenses

| Library | License | Link |
|---------|---------|------|
| MinGW-w64 | GPL | https://mingw-w64.org |
| CMake | BSD | https://cmake.org |
| ESP32 Arduino Core | LGPL | https://github.com/espressif/arduino-esp32 |
| PlatformIO | Apache 2.0 | https://platformio.org |

---

## Credits

### Development Team

- **Project Lead**: Security Research Team
- **Core Development**: Main Contributors
- **ESP32 Firmware**: ESP32 Development Team
- **Testing**: QA Team
- **Documentation**: Documentation Team

### Acknowledgments

- ESPRESSIF for ESP32 hardware
- MinGW-w64 project for cross-compilation tools
- PlatformIO for firmware development
- Open-source community for libraries and tools

---

## Contact

### Support

- **GitHub Issues**: https://github.com/yourusername/EvilTwin/issues
- **Email**: support@example.com
- **Discord**: https://discord.gg/example

### Documentation

- **User Manual**: docs/manual.pdf
- **API Reference**: docs/api.html
- **Developer Guide**: docs/developer.md

---

## Changelog

### Version 1.0 (2024-01-01)

- Initial release
- WiFi scanning implemented
- ESP32 communication
- Captive portal server
- Credential capture
- Deauth attack support
- Cross-compilation support
- Windows UI

### Version 1.1 (Planned)

- Linux native support
- Additional WiFi adapter support
- Improved stability
- Performance optimizations

---

## Additional Resources

### References

1. [IEEE 802.11 Standard](https://ieee802.org/11/)
2. [Windows WiFi API Documentation](https://docs.microsoft.com/en-us/windows/win32/nativewifi/)
3. [ESP32 Technical Reference](https://www.espressif.com/en/products/socs/esp32)
4. [Wireless Security Best Practices](https://www.cisa.gov/)

### Tools

- **Wireshark**: Packet analysis
- **Aircrack-ng**: WiFi security tools
- **Hashcat**: Password cracking
- **Metasploit**: Penetration testing

---

## Appendix

### A. Windows WiFi API Reference

#### Key Functions

| Function | Description |
|----------|-------------|
| WlanOpenHandle | Open connection to WiFi service |
| WlanCloseHandle | Close connection to WiFi service |
| WlanEnumInterfaces | List all WiFi adapters |
| WlanScan | Start a network scan |
| WlanGetNetworkBssList | Get BSS list from scan |
| WlanRegisterNotification | Register for WiFi events |

### B. Serial Protocol Reference

#### Command Structure

```
Command: [0x02] [0x00] [0x00]
Response: [0x03] [0x00] [0x00]
```

#### Data Encoding

- Strings: Null-terminated UTF-8
- Numbers: Little-endian
- Booleans: 0x00 (false) or 0x01 (true)

### C. Captive Portal Templates

#### HTML Structure

```html
<!DOCTYPE html>
<html>
<head>
    <title>WiFi Login</title>
    <link rel="stylesheet" href="/style.css">
</head>
<body>
    <div class="container">
        <h1>WiFi Login</h1>
        <form action="/login" method="POST">
            <input type="text" name="username" placeholder="Username">
            <input type="password" name="password" placeholder="Password">
            <button type="submit">Connect</button>
        </form>
    </div>
    <script src="/script.js"></script>
</body>
</html>
```

### D. Example Credential Export

```
Evil Twin Attack Tool - Captured Credentials
================================================

Total Captured: 3

   1. 2024-01-01 12:00:00 | SSID: HomeWiFi | User: john_doe | Pass: password123 | MAC: AA:BB:CC:DD:EE:FF
   2. 2024-01-01 12:05:00 | SSID: HomeWiFi | User: jane_smith | Pass: qwerty789 | MAC: 11:22:33:44:55:66
   3. 2024-01-01 12:10:00 | SSID: OfficeNet | User: admin | Pass: admin2024 | MAC: 77:88:99:AA:BB:CC
```

---

## End of Documentation
