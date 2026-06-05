# Configuration Template
## Quick Reference for All Settings

---

## 📡 ESP8266 Configuration

### WiFi Settings (Both ESP8266s)

```cpp
// File: esp8266_trigger.ino and esp8266_receiver.ino
// Lines: 35-36

const char* ssid = "YOUR_WIFI_SSID";           // Your WiFi network name
const char* password = "YOUR_WIFI_PASSWORD";   // Your WiFi password
```

**Example:**
```cpp
const char* ssid = "HomeNetwork";
const char* password = "MySecurePassword123";
```

**Notes:**
- SSID is case-sensitive
- Must be 2.4GHz network (ESP8266 doesn't support 5GHz)
- Avoid special characters in SSID if possible

---

### MQTT Broker Settings (Both ESP8266s)

```cpp
// File: esp8266_trigger.ino and esp8266_receiver.ino
// Lines: 39-41

const char* mqtt_server = "test.mosquitto.org"; // MQTT broker address
const int mqtt_port = 1883;                     // MQTT port
const char* mqtt_client_id = "ESP8266_xxx";     // Unique client ID
```

**Public Brokers (Free):**
```cpp
// Option 1: Eclipse Mosquitto (recommended)
const char* mqtt_server = "test.mosquitto.org";

// Option 2: HiveMQ
const char* mqtt_server = "broker.hivemq.com";

// Option 3: Eclipse IoT
const char* mqtt_server = "mqtt.eclipse.org";
```

**Private Broker:**
```cpp
const char* mqtt_server = "192.168.1.100";  // Your local broker IP
const int mqtt_port = 1883;                  // Standard port
// For TLS: use port 8883
```

**Notes:**
- Each ESP8266 must have unique client ID
- Default port: 1883 (no encryption)
- TLS port: 8883 (encrypted)

---

### MQTT Topics (Both ESP8266s)

```cpp
// File: esp8266_trigger.ino and esp8266_receiver.ino
// Lines: 44-46

const char* topic_alert = "building/alert";     // Alert messages
const char* topic_status = "building/status";   // Heartbeat status
const char* topic_reset = "building/reset";     // Reset commands
```

**Custom Topics:**
```cpp
// For multiple buildings
const char* topic_alert = "building1/floor2/alert";
const char* topic_status = "building1/floor2/status";

// For different zones
const char* topic_alert = "zone_a/alert";
const char* topic_status = "zone_a/status";
```

**Notes:**
- Topics are case-sensitive
- Use forward slashes (/) for hierarchy
- Keep topics short and meaningful

---

### Device Identification

**ESP8266 #1 (Trigger):**
```cpp
// File: esp8266_trigger.ino
// Lines: 49-50

const char* device_id = "ESP1";                 // Device ID
const char* device_name = "Trigger Unit";       // Human-readable name
```

**ESP8266 #2 (Receiver):**
```cpp
// File: esp8266_receiver.ino
// Lines: 49-50

const char* device_id = "ESP2";                 // Device ID
const char* device_name = "Receiver Unit";      // Human-readable name
```

**Custom Names:**
```cpp
// Building 1, Floor 2
const char* device_id = "B1F2_TRIGGER";
const char* device_name = "Building 1 Floor 2 Trigger";

// Zone A
const char* device_id = "ZONE_A_RECEIVER";
const char* device_name = "Zone A Receiver Unit";
```

---

### GPIO Pin Configuration (ESP8266 #2 Only)

```cpp
// File: esp8266_receiver.ino
// Lines: 53-56

const int RELAY_FIRE = D1;      // GPIO5 - Relay 1 for FIRE
const int RELAY_MEDICAL = D2;   // GPIO4 - Relay 2 for MEDICAL
const int RELAY_SECURITY = D5;  // GPIO14 - Relay 3 for SECURITY
const int RELAY_SPARE = D6;     // GPIO12 - Relay 4 for future use
```

**Alternative Pins:**
```cpp
// If using different pins
const int RELAY_FIRE = D3;      // GPIO0
const int RELAY_MEDICAL = D4;   // GPIO2
const int RELAY_SECURITY = D7;  // GPIO13
const int RELAY_SPARE = D8;     // GPIO15
```

**Notes:**
- D0 (GPIO16) has limited functionality
- D3, D4, D8 have pull-up/pull-down resistors
- Check your relay module's active level (HIGH/LOW)

---

### Timing Configuration (Both ESP8266s)

```cpp
// File: esp8266_trigger.ino and esp8266_receiver.ino
// Lines: 59-60

const unsigned long HEARTBEAT_INTERVAL = 10000; // 10 seconds
const unsigned long RECONNECT_INTERVAL = 5000;  // 5 seconds
```

**Custom Timing:**
```cpp
// More frequent heartbeat (5 seconds)
const unsigned long HEARTBEAT_INTERVAL = 5000;

// Faster reconnection (2 seconds)
const unsigned long RECONNECT_INTERVAL = 2000;

// Less frequent heartbeat (30 seconds)
const unsigned long HEARTBEAT_INTERVAL = 30000;
```

---

### Alert Pattern Timing (ESP8266 #2 Only)

```cpp
// File: esp8266_receiver.ino
// Lines: 63-64

const unsigned long MEDICAL_PULSE_INTERVAL = 500;   // 500ms
const unsigned long SECURITY_FLASH_INTERVAL = 200;  // 200ms
```

**Custom Patterns:**
```cpp
// Slower medical pulse (1 second)
const unsigned long MEDICAL_PULSE_INTERVAL = 1000;

// Faster security flash (100ms)
const unsigned long SECURITY_FLASH_INTERVAL = 100;

// Very slow pulse (2 seconds)
const unsigned long MEDICAL_PULSE_INTERVAL = 2000;
```

---

## 🖥️ Backend Server Configuration

### Server Settings

```javascript
// File: backend/server.js
// Lines: 24-27

const PORT = 3000;                              // Web server port
const MQTT_BROKER = 'mqtt://test.mosquitto.org'; // MQTT broker URL
const DEVICE_OFFLINE_TIMEOUT = 30000;           // 30 seconds
```

**Custom Settings:**
```javascript
// Different port
const PORT = 8080;

// Local MQTT broker
const MQTT_BROKER = 'mqtt://192.168.1.100:1883';

// Longer offline timeout (60 seconds)
const DEVICE_OFFLINE_TIMEOUT = 60000;

// Private broker with authentication
const MQTT_BROKER = 'mqtt://username:password@broker.example.com:1883';
```

---

### MQTT Topics (Backend)

```javascript
// File: backend/server.js
// Lines: 28-30

const MQTT_TOPIC_ALERT = 'building/alert';      // Alert topic
const MQTT_TOPIC_STATUS = 'building/status';    // Status topic
const MQTT_TOPIC_RESET = 'building/reset';      // Reset topic
```

**Must Match ESP8266 Topics!**

---

### User Credentials

```javascript
// File: backend/server.js
// Lines: 33-36

const USERS = {
  admin: 'admin123',  // username: password
  user: 'user123'
};
```

**Custom Users:**
```javascript
const USERS = {
  admin: 'SecurePassword123!',
  operator: 'OperatorPass456',
  viewer: 'ViewOnly789',
  john: 'JohnPassword',
  jane: 'JanePassword'
};
```

**Security Notes:**
- Change default passwords immediately
- Use strong passwords (8+ characters, mixed case, numbers, symbols)
- In production, use hashed passwords (bcrypt)
- Store credentials in environment variables

---

### Session Configuration

```javascript
// File: backend/server.js
// Lines: 48-54

app.use(session({
  secret: 'iot-emergency-alert-secret-key-2026',  // Change this!
  resave: false,
  saveUninitialized: false,
  cookie: { 
    maxAge: 24 * 60 * 60 * 1000, // 24 hours
    httpOnly: true
  }
}));
```

**Custom Session:**
```javascript
app.use(session({
  secret: 'your-unique-secret-key-here',  // Use random string
  resave: false,
  saveUninitialized: false,
  cookie: { 
    maxAge: 8 * 60 * 60 * 1000,  // 8 hours
    httpOnly: true,
    secure: true  // Enable for HTTPS
  }
}));
```

---

### File Paths

```javascript
// File: backend/server.js
// Line: 31

const ALERTS_FILE = path.join(__dirname, 'data', 'alerts.json');
```

**Custom Path:**
```javascript
// Different directory
const ALERTS_FILE = path.join(__dirname, 'logs', 'alerts.json');

// Absolute path
const ALERTS_FILE = '/var/log/iot-alerts/alerts.json';

// User home directory
const ALERTS_FILE = path.join(os.homedir(), 'iot-alerts', 'alerts.json');
```

---

## 🌐 Frontend Configuration

### WebSocket Connection

```javascript
// File: backend/public/js/dashboard.js
// Line: 8

const socket = io();  // Auto-connects to same host
```

**Custom Connection:**
```javascript
// Specific server
const socket = io('http://192.168.1.100:3000');

// With options
const socket = io({
  reconnection: true,
  reconnectionDelay: 1000,
  reconnectionAttempts: 10
});
```

---

### Dashboard Refresh Rate

```javascript
// File: backend/public/js/dashboard.js
// Line: 16

setInterval(updateLastUpdateTime, 1000);  // Update every 1 second
```

**Custom Rate:**
```javascript
// Update every 5 seconds
setInterval(updateLastUpdateTime, 5000);

// Update every 500ms
setInterval(updateLastUpdateTime, 500);
```

---

## 📋 Quick Configuration Checklist

### Before First Run

- [ ] Update WiFi SSID in both ESP8266 sketches
- [ ] Update WiFi password in both ESP8266 sketches
- [ ] Verify MQTT broker address (same in all 3 files)
- [ ] Verify MQTT topics match (ESP8266s and backend)
- [ ] Change default user passwords in backend
- [ ] Change session secret in backend
- [ ] Verify GPIO pins match your wiring
- [ ] Set appropriate timing intervals

### For Production Deployment

- [ ] Use private MQTT broker
- [ ] Enable MQTT authentication
- [ ] Use MQTT over TLS (port 8883)
- [ ] Use strong passwords (hashed)
- [ ] Enable HTTPS for web dashboard
- [ ] Use environment variables for secrets
- [ ] Implement rate limiting
- [ ] Add input validation
- [ ] Set up proper logging
- [ ] Configure firewall rules

---

## 🔐 Environment Variables (Production)

Create `.env` file in backend directory:

```bash
# Server Configuration
PORT=3000
NODE_ENV=production

# MQTT Configuration
MQTT_BROKER=mqtt://your-broker.com:1883
MQTT_USERNAME=your_username
MQTT_PASSWORD=your_password

# Session Configuration
SESSION_SECRET=your-random-secret-key-here

# User Credentials (use hashed passwords)
ADMIN_USERNAME=admin
ADMIN_PASSWORD_HASH=$2b$10$...

# Timeouts
DEVICE_OFFLINE_TIMEOUT=30000
SESSION_MAX_AGE=86400000
```

Load in server.js:
```javascript
require('dotenv').config();

const PORT = process.env.PORT || 3000;
const MQTT_BROKER = process.env.MQTT_BROKER;
```

---

## 📝 Configuration Files Summary

| File | Purpose | Key Settings |
|------|---------|--------------|
| `esp8266_trigger.ino` | Trigger unit firmware | WiFi, MQTT, Device ID |
| `esp8266_receiver.ino` | Receiver unit firmware | WiFi, MQTT, GPIO pins |
| `backend/server.js` | Backend server | Port, MQTT, Users |
| `backend/public/js/dashboard.js` | Dashboard logic | WebSocket, Refresh rate |

---

## 🔄 Configuration Change Workflow

### Changing WiFi Network

1. Update SSID and password in both `.ino` files
2. Re-upload firmware to both ESP8266s
3. Restart devices
4. Verify connection in Serial Monitor

### Changing MQTT Broker

1. Update `mqtt_server` in both `.ino` files
2. Update `MQTT_BROKER` in `server.js`
3. Re-upload firmware to both ESP8266s
4. Restart backend server
5. Verify connections

### Adding New User

1. Edit `USERS` object in `server.js`
2. Restart backend server
3. Test login with new credentials

### Changing Relay Pins

1. Update pin definitions in `esp8266_receiver.ino`
2. Update wiring to match new pins
3. Re-upload firmware
4. Test relay activation

---

## ✅ Validation

### Test Configuration

```bash
# Test MQTT broker connectivity
ping test.mosquitto.org

# Test MQTT publish/subscribe
mosquitto_sub -h test.mosquitto.org -t "building/#" -v

# Test backend server
curl http://localhost:3000

# Test WebSocket
# Open browser console and check for connection messages
```

---

**Keep this file handy for quick configuration changes!**
