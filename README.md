# ESP32 Captive Portal Web Server

This project is a **custom ESP32 firmware** that creates a WiFi access point with a captive portal.  
When users connect to the network, they are automatically redirected to a local webpage hosted by the ESP32.

The system supports multiple webpage modes, interaction logging, credential storage, and configurable settings.

---

## Features

- 📡 Creates a WiFi Access Point
- 🌐 Captive portal (automatic redirect to hosted page)
- 🎧 Audio playback page
- 🔐 Login portal page
- 😈 Prank page
- 📊 Tracks button clicks from connected devices
- 📝 Stores submitted login credentials
- ⚙️ Web settings panel
- 💾 Persistent storage using ESP32 Preferences

---

## Available Sites

### 1. Audio Page
A WiFi connection page with buttons that play an audio file when clicked.

Features:
- Large UI buttons
- Audio playback from ESP32 flash
- Button click tracking

---

### 2. Login Portal
A styled login page simulating a university network portal.

Features:
- Username and password form
- Credentials saved in ESP32 memory
- View stored credentials via `/acc`

---

### 3. Prank Page
A simple prank webpage displaying a fake "system hacked" screen.

---

## Web Routes

| Route | Description |
|------|-------------|
| `/` | Main captive portal page |
| `/login` | Handles login form submission |
| `/acc` | View stored login credentials |
| `/list` | View logged button clicks |
| `/track` | Records button presses |
| `/audio` | Streams audio file |
| `/settings` | WiFi and site configuration panel |

---

## Settings Page

Accessible via:
