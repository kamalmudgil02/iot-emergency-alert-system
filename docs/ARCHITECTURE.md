# System Architecture Documentation
## Real-Time Distributed Wireless Emergency Alert & Monitoring System

---

## 🏗️ SYSTEM OVERVIEW

This is a distributed IoT system with 3 main components:
1. **ESP8266 #1** - Emergency Trigger Unit (Publisher)
2. **ESP8266 #2** - Alert Receiver & Relay Controller (Subscriber)
3. **Node.js Backend + Web Dashboard** - Central monitoring and control

---

## 📊 ARCHITECTURE DIAGRAM (Text-Based)

```
┌─────────────────────────────────────────────────────────────────┐
│                         INTERNET / WiFi                          │
└────────┬──────────────────────┬──────────────────────┬──────────┘
         │                      │                      │
         │                      │                      │
    ┌────▼─────┐          ┌────▼─────┐          ┌────▼─────┐
    │ ESP8266  │          │ ESP8266  │          │  Node.js │
    │   #1     │          │   #2     │          │  Server  │
    │ TRIGGER  │          │ RECEIVER │          │ BACKEND  │
    └────┬─────┘          └────┬─────┘          └────┬─────┘
         │                     │                      │
         │                     │                      │
         │                ┌────▼─────┐               │
         │                │  4-CH    │               │
         │                │  RELAY   │               │
         │                │  MODULE  │               │
         │                └──────────┘               │
         │                                            │
         │                                       ┌────▼─────┐
         │                                       │   Web    │
         │                                       │Dashboard │
         │                                       │ (Browser)│
         │                                       └──────────┘
         │
    ┌────▼──────────────────────────────────────────────────┐
    │         MQTT BROKER (test.mosquitto.org)              │
    │                                                        │
    │  Topics:                                              │
    │  • building/alert    - Emergency alerts               │
    │  • building/status   - Device heartbeat               │
    │  • building/reset    - Reset command                  │
    └────────────────────────────────────────────────────────┘
```

---

## 🔄 DATA FLOW DIAGRAM

### Flow 1: Emergency Alert Triggered

```
ESP8266 #1                MQTT Broker              ESP8266 #2              Backend Server
    │                          │                        │                        │
    │ 1. Button Press          │                        │                        │
    │ (FIRE/MEDICAL/SECURITY)  │                        │                        │
    │                          │                        │                        │
    │ 2. Publish Alert         │                        │                        │
    ├─────────────────────────>│                        │                        │
    │ Topic: building/alert    │                        │                        │
    │ Payload: "FIRE"          │                        │                        │
    │                          │                        │                        │
    │                          │ 3. Forward to          │                        │
    │                          │    Subscribers         │                        │
    │                          ├───────────────────────>│                        │
    │                          │                        │                        │
    │                          │                        │ 4. Activate Relay      │
    │                          │                        │    (Continuous Siren)  │
    │                          │                        │                        │
    │                          │ 5. Forward to          │                        │
    │                          │    Backend             │                        │
    │                          ├────────────────────────┼───────────────────────>│
    │                          │                        │                        │
    │                          │                        │ 6. Log Alert           │
    │                          │                        │    Store in JSON       │
    │                          │                        │                        │
    │                          │                        │ 7. Broadcast via       │
    │                          │                        │    WebSocket           │
    │                          │                        │         │              │
    │                          │                        │         ▼              │
    │                          │                        │    Web Dashboard       │
    │                          │                        │    (Real-time Update)  │
```

### Flow 2: Heartbeat Monitoring

```
ESP8266 #1/2              MQTT Broker              Backend Server
    │                          │                        │
    │ Every 10 seconds         │                        │
    │                          │                        │
    │ Publish Status           │                        │
    ├─────────────────────────>│                        │
    │ Topic: building/status   │                        │
    │ Payload: "ESP1:ALIVE"    │                        │
    │                          │                        │
    │                          │ Forward                │
    │                          ├───────────────────────>│
    │                          │                        │
    │                          │                        │ Update Last Seen
    │                          │                        │ Timestamp
    │                          │                        │
    │                          │                        │ Check if > 30s
    │                          │                        │ Mark OFFLINE
    │                          │                        │
    │                          │                        │ Broadcast Status
    │                          │                        │ via WebSocket
    │                          │                        │      │
    │                          │                        │      ▼
    │                          │                        │ Web Dashboard
```

### Flow 3: Alert Reset

```
Web Dashboard            Backend Server           MQTT Broker              ESP8266 #2
    │                          │                        │                        │
    │ 1. Admin clicks RESET    │                        │                        │
    ├─────────────────────────>│                        │                        │
    │ POST /api/alerts/reset   │                        │                        │
    │                          │                        │                        │
    │                          │ 2. Publish Reset       │                        │
    │                          ├───────────────────────>│                        │
    │                          │ Topic: building/reset  │                        │
    │                          │ Payload: "RESET"       │                        │
    │                          │                        │                        │
    │                          │                        │ 3. Forward             │
    │                          │                        ├───────────────────────>│
    │                          │                        │                        │
    │                          │                        │                        │ 4. Turn OFF Relay
    │                          │                        │                        │    Clear Alert State
    │                          │                        │                        │
    │                          │ 5. Update Alert Status │                        │
    │                          │    Mark as Acknowledged│                        │
    │                          │                        │                        │
    │ 6. Broadcast Update      │                        │                        │
    │<─────────────────────────┤                        │                        │
    │ via WebSocket            │                        │                        │
```

---

## 📡 MQTT TOPIC STRUCTURE

### Topic: `building/alert`
- **Publisher**: ESP8266 #1 (Trigger Unit)
- **Subscribers**: ESP8266 #2, Backend Server
- **Payload Format**: Plain text
- **Possible Values**:
  - `FIRE` - Fire emergency
  - `MEDICAL` - Medical emergency
  - `SECURITY` - Security breach
- **QoS Level**: 1 (At least once delivery)
- **Retain**: false

### Topic: `building/status`
- **Publishers**: ESP8266 #1, ESP8266 #2
- **Subscribers**: Backend Server
- **Payload Format**: `DEVICE_ID:STATUS`
- **Examples**:
  - `ESP1:ALIVE`
  - `ESP2:ALIVE`
- **QoS Level**: 0 (Fire and forget)
- **Retain**: false
- **Frequency**: Every 10 seconds

### Topic: `building/reset`
- **Publisher**: Backend Server
- **Subscribers**: ESP8266 #2 (Receiver)
- **Payload Format**: Plain text
- **Value**: `RESET`
- **QoS Level**: 1 (At least once delivery)
- **Retain**: false

---

## 🔌 HARDWARE ARCHITECTURE

### ESP8266 #1 - Trigger Unit

```
┌─────────────────────────────┐
│      ESP8266 NodeMCU        │
│                             │
│  GPIO Pins Used:            │
│  • D0 (GPIO16) - LED Status │
│  • Serial - Button Input    │
│    (Simulated via Serial    │
│     Monitor commands)       │
│                             │
│  Power: USB or Battery      │
│  (7.4V Li-ion via VIN)      │
└─────────────────────────────┘
```

### ESP8266 #2 - Receiver + Relay Controller

```
┌─────────────────────────────┐
│      ESP8266 NodeMCU        │
│                             │
│  GPIO Pins Used:            │
│  • D1 (GPIO5)  - Relay IN1  │
│  • D2 (GPIO4)  - Relay IN2  │
│  • D5 (GPIO14) - Relay IN3  │
│  • D6 (GPIO12) - Relay IN4  │
│  • D0 (GPIO16) - LED Status │
│                             │
│  Power: USB or Battery      │
└──────────┬──────────────────┘
           │
           │ Control Signals
           ▼
┌─────────────────────────────┐
│   4-Channel Relay Module    │
│                             │
│  VCC  - 5V (External)       │
│  GND  - Common Ground       │
│  IN1  - D1 (GPIO5)          │
│  IN2  - D2 (GPIO4)          │
│  IN3  - D5 (GPIO14)         │
│  IN4  - D6 (GPIO12)         │
│                             │
│  Relay Outputs:             │
│  • NO/NC/COM terminals      │
│  • Connect siren/buzzer     │
└─────────────────────────────┘
```

---

## 💾 DATA STORAGE STRUCTURE

### alerts.json (Backend)

```json
{
  "alerts": [
    {
      "id": "alert_1708502400000",
      "type": "FIRE",
      "timestamp": "2026-02-21T07:20:00.000Z",
      "acknowledged": false,
      "acknowledgedAt": null,
      "device": "ESP1"
    },
    {
      "id": "alert_1708502460000",
      "type": "MEDICAL",
      "timestamp": "2026-02-21T07:21:00.000Z",
      "acknowledged": true,
      "acknowledgedAt": "2026-02-21T07:22:00.000Z",
      "device": "ESP1"
    }
  ]
}
```

### Device Status (In-Memory)

```javascript
{
  "ESP1": {
    "id": "ESP1",
    "name": "Trigger Unit",
    "status": "online",
    "lastSeen": 1708502400000,
    "ip": "192.168.1.100"
  },
  "ESP2": {
    "id": "ESP2",
    "name": "Receiver Unit",
    "status": "online",
    "lastSeen": 1708502400000,
    "ip": "192.168.1.101"
  }
}
```

---

## 🔐 AUTHENTICATION FLOW

```
Browser                  Backend Server
   │                          │
   │ 1. GET /login.html       │
   ├─────────────────────────>│
   │                          │
   │ 2. Serve Login Page      │
   │<─────────────────────────┤
   │                          │
   │ 3. POST /api/login       │
   │    {username, password}  │
   ├─────────────────────────>│
   │                          │
   │                          │ 4. Validate Credentials
   │                          │    Create Session
   │                          │
   │ 5. Session Cookie        │
   │<─────────────────────────┤
   │    {success: true}       │
   │                          │
   │ 6. GET /dashboard.html   │
   ├─────────────────────────>│
   │                          │
   │                          │ 7. Check Session
   │                          │    Middleware
   │                          │
   │ 8. Serve Dashboard       │
   │<─────────────────────────┤
```

---

## 🌐 WEBSOCKET COMMUNICATION

### Events from Server to Client

1. **newAlert**
   ```javascript
   {
     type: 'FIRE',
     timestamp: '2026-02-21T07:20:00.000Z',
     device: 'ESP1'
   }
   ```

2. **deviceStatus**
   ```javascript
   {
     ESP1: { status: 'online', lastSeen: 1708502400000 },
     ESP2: { status: 'offline', lastSeen: 1708502370000 }
   }
   ```

3. **alertAcknowledged**
   ```javascript
   {
     alertId: 'alert_1708502400000',
     acknowledgedAt: '2026-02-21T07:22:00.000Z'
   }
   ```

4. **alertReset**
   ```javascript
   {
     message: 'All alerts reset',
     timestamp: '2026-02-21T07:25:00.000Z'
   }
   ```

---

## ⚡ RELAY CONTROL PATTERNS

### FIRE Alert
- **Pattern**: Continuous ON
- **Relays**: IN1 ON, others OFF
- **Duration**: Until reset
- **Use Case**: Continuous siren

### MEDICAL Alert
- **Pattern**: Short beeps (500ms ON, 500ms OFF)
- **Relays**: IN2 pulsing
- **Duration**: Until reset
- **Use Case**: Medical attention needed

### SECURITY Alert
- **Pattern**: Fast flashing (200ms ON, 200ms OFF)
- **Relays**: IN3 and IN4 alternating
- **Duration**: Until reset
- **Use Case**: Security breach alarm

---

## 🔄 SYSTEM STATES

### Normal State
- All relays OFF
- Both ESP8266 sending heartbeats
- Dashboard shows "No Active Alerts"
- Devices shown as ONLINE

### Alert State
- Relay activated based on alert type
- Dashboard shows alert details
- Alert logged in database
- Visual/audio notification on dashboard

### Offline State
- Device not sending heartbeat for >30s
- Dashboard shows device as OFFLINE
- Admin notified
- System continues with available devices

### Reset State
- All relays turned OFF
- Alert marked as acknowledged
- System returns to Normal State
- Event logged

---

## 📈 SCALABILITY CONSIDERATIONS

### Current Design
- 2 ESP8266 devices
- Single building monitoring
- JSON file storage
- Public MQTT broker

### Future Enhancements
- Add more ESP8266 devices (multiple floors)
- Database integration (MongoDB/MySQL)
- Private MQTT broker
- Mobile app integration
- SMS/Email notifications
- Historical data analytics
- Multi-building support

---

## 🛡️ SECURITY CONSIDERATIONS

### Current Implementation
- Session-based authentication
- Public MQTT broker (no encryption)
- Local network recommended

### Production Recommendations
- Use MQTT over TLS (port 8883)
- Implement MQTT username/password
- Use private MQTT broker
- HTTPS for web dashboard
- Strong password policies
- Rate limiting on API endpoints
- Input validation and sanitization

---

## 📊 PERFORMANCE METRICS

### Expected Performance
- **Alert Latency**: < 2 seconds (trigger to relay activation)
- **Dashboard Update**: < 1 second (real-time via WebSocket)
- **Heartbeat Interval**: 10 seconds
- **Offline Detection**: 30 seconds
- **MQTT QoS**: Level 1 for critical messages
- **Concurrent Users**: 10+ on dashboard

---

This architecture provides a solid foundation for a final year IoT project with real-world applicability and room for future enhancements.
