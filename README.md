# IoT Emergency Alert & Monitoring System

![Platform](https://img.shields.io/badge/platform-ESP8266-blue)
![Protocol](https://img.shields.io/badge/protocol-MQTT-orange)
![Language](https://img.shields.io/badge/language-C%2FC%2B%2B-red)
![License](https://img.shields.io/badge/license-MIT-green)
![Status](https://img.shields.io/badge/status-complete-brightgreen)

A distributed IoT emergency alert system built with dual ESP8266 microcontrollers communicating over MQTT. One device publishes emergency alerts; the second receives them and activates a 4-channel relay module — simulating a real-world building alarm system.

---

## System Overview

```
ESP8266 #1 (Trigger)          MQTT Broker            ESP8266 #2 (Receiver)
   Serial command        →   building/alert    →      Relay activation
   Heartbeat status      →   building/status   →      Health monitoring
   Dashboard reset       ←   building/reset    ←      Web dashboard
                                   ↕
                          Node.js Backend
                          WebSocket Dashboard
```

### Alert Patterns

| Alert Type | Relay | Pattern | Latency |
|---|---|---|---|
| FIRE | IN1 | Continuous ON | < 2 sec |
| MEDICAL | IN2 | 500ms ON / 500ms OFF | < 2 sec |
| SECURITY | IN3 + IN4 | 200ms alternating flash | < 2 sec |

---

## Hardware

- 2x ESP8266 NodeMCU
- 1x 4-channel 5V relay module
- 2x 7.4V 1200mAh Li-ion battery
- Breadboard, jumper wires, USB cables
- Optional: buzzer or siren on relay output

---

## Tech Stack

| Layer | Technology |
|---|---|
| Firmware | C/C++, Arduino IDE, ESP8266WiFi, PubSubClient |
| Protocol | MQTT (QoS 0 heartbeat, QoS 1 alerts) |
| Backend | Node.js, Express.js, Socket.io, mqtt npm |
| Frontend | HTML5, CSS3, JavaScript ES6, Bootstrap 5 |
| Auth | Session-based (express-session) |
| Storage | JSON file system |

---

## Project Structure

```
iot-emergency-alert-system/
├── esp8266_trigger/
│   └── esp8266_trigger.ino       # Trigger unit firmware (400+ lines)
├── esp8266_receiver/
│   └── esp8266_receiver.ino      # Receiver unit firmware (450+ lines)
├── backend/
│   ├── server.js                 # Node.js backend (500+ lines)
│   ├── package.json
│   └── public/
│       ├── login.html
│       ├── dashboard.html
│       ├── css/style.css
│       └── js/dashboard.js
├── docs/
│   ├── ARCHITECTURE.md
│   ├── WIRING_GUIDE.md
│   ├── SETUP_GUIDE.md
│   ├── MQTT_EXPLAINED.md
│   └── TROUBLESHOOTING.md
└── README.md
```

---

## Quick Start

### Step 1 — Hardware
Wire components following `docs/WIRING_GUIDE.md`. Ensure common ground between ESP8266 boards and relay module.

### Step 2 — Flash ESP8266 Firmware
```bash
# Open Arduino IDE
# Install: ESP8266 board package + PubSubClient library
# Open esp8266_trigger.ino → update WiFi credentials (lines 35-36) → upload to ESP8266 #1
# Open esp8266_receiver.ino → update WiFi credentials → upload to ESP8266 #2
```

### Step 3 — Start Backend Server
```bash
cd backend
npm install
npm start
# Server runs at http://localhost:3000
```

### Step 4 — Access Dashboard
```
URL:      http://localhost:3000
Username: admin
Password: admin123
```

---

## Triggering Alerts

Open Serial Monitor at **115200 baud** on ESP8266 #1 and type:

```
FIRE      → activates relay IN1 (continuous)
MEDICAL   → activates relay IN2 (pulsing)
SECURITY  → activates relay IN3 + IN4 (fast alternating)
STATUS    → prints device status
```

---

## Serial Monitor Output (ESP8266 #1)

```
[BOOT]  ESP8266 Trigger Unit starting...
[WiFi]  Connecting to network...
[WiFi]  Connected. IP: 192.168.1.101
[MQTT]  Connected to broker: test.mosquitto.org
[BEAT]  Heartbeat sent → building/status
[ALERT] FIRE command received
[MQTT]  Published → building/alert : {"type":"FIRE","device":"ESP1","ts":1720000000}
[BEAT]  Heartbeat sent → building/status
```

---

## MQTT Topics

| Topic | Publisher | Subscriber | QoS |
|---|---|---|---|
| `building/alert` | ESP8266 #1 | ESP8266 #2, Backend | 1 |
| `building/status` | ESP8266 #1 & #2 | Backend | 0 |
| `building/reset` | Backend | ESP8266 #2 | 1 |

---

## Key Technical Decisions

**MQTT QoS 1 for alerts** — guarantees delivery at least once, critical for emergency events. Heartbeats use QoS 0 since occasional loss is acceptable.

**Watchdog timer on ESP8266** — prevents firmware hangs in production. Auto-restarts if main loop stalls beyond timeout.

**30-second offline detection** — backend tracks last heartbeat timestamp per device. Missing for 30 seconds marks device offline on dashboard.

**Separate relay patterns per alert type** — allows physical differentiation of emergencies without a display device.

---

## Performance

| Metric | Value |
|---|---|
| Alert trigger to relay activation | < 2 seconds |
| Dashboard WebSocket update | < 1 second |
| Heartbeat interval | 10 seconds |
| Offline detection timeout | 30 seconds |
| Concurrent dashboard users | 10+ |

---

## Security Notes

Default credentials are for local development only. For production:
- Change `admin123` and `user123` passwords
- Use a private MQTT broker (HiveMQ, Mosquitto self-hosted)
- Enable MQTT over TLS (port 8883)
- Add HTTPS to the dashboard
- Implement rate limiting on API endpoints

---

## Documentation

| File | Contents |
|---|---|
| `docs/ARCHITECTURE.md` | Full system design and data flow diagrams |
| `docs/WIRING_GUIDE.md` | Hardware connections with pin mappings |
| `docs/SETUP_GUIDE.md` | Step-by-step installation guide |
| `docs/MQTT_EXPLAINED.md` | MQTT protocol explanation for beginners |
| `docs/TROUBLESHOOTING.md` | Common issues and fixes |

---

## Future Enhancements

- SMS / email notifications via Twilio / SendGrid
- Mobile app (React Native)
- Private MQTT broker with TLS authentication
- MongoDB integration for historical alert analytics
- Multi-building support with location tagging
- Camera feed integration on security alerts

---

## Author

**Kamal Sharma**
B.Tech — Electronics and Communication Engineering, VIT Bhopal (2026)
GitHub: [github.com/kamalmudgil02](https://github.com/kamalmudgil02)
LinkedIn: [linkedin.com/in/kamal-sharma-9659b6250](https://linkedin.com/in/kamal-sharma-9659b6250)

---
