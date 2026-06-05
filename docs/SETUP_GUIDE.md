# Complete Setup Guide
## Step-by-Step Installation and Configuration

---

## 📋 Prerequisites Checklist

Before starting, ensure you have:
- [ ] 2x ESP8266 NodeMCU boards
- [ ] 1x 4-Channel Relay Module
- [ ] 2x 7.4V Li-ion batteries (optional)
- [ ] Computer with Windows/Mac/Linux
- [ ] WiFi network (2.4GHz - ESP8266 doesn't support 5GHz)
- [ ] Internet connection
- [ ] USB cables for ESP8266 programming

---

## PART 1: ARDUINO IDE SETUP

### Step 1.1: Install Arduino IDE

1. Download Arduino IDE from: https://www.arduino.cc/en/software
2. Choose version 2.x (recommended) or 1.8.x
3. Install following the installer instructions
4. Launch Arduino IDE

### Step 1.2: Add ESP8266 Board Support

1. Open Arduino IDE
2. Go to: **File → Preferences**
3. In "Additional Board Manager URLs", add:
   ```
   http://arduino.esp8266.com/stable/package_esp8266com_index.json
   ```
4. Click **OK**
5. Go to: **Tools → Board → Boards Manager**
6. Search for: `esp8266`
7. Install: **ESP8266 by ESP8266 Community** (latest version)
8. Wait for installation to complete
9. Close Boards Manager

### Step 1.3: Install Required Libraries

1. Go to: **Sketch → Include Library → Manage Libraries**
2. Search for: `PubSubClient`
3. Install: **PubSubClient by Nick O'Leary** (latest version)
4. Close Library Manager

**Note**: ESP8266WiFi library is built-in, no installation needed.

### Step 1.4: Configure Board Settings

1. Go to: **Tools → Board → ESP8266 Boards**
2. Select: **NodeMCU 1.0 (ESP-12E Module)**
3. Configure settings:
   - **Upload Speed**: 115200
   - **CPU Frequency**: 80 MHz
   - **Flash Size**: 4MB (FS:2MB OTA:~1019KB)
   - **Port**: Select your ESP8266's COM port

**Finding COM Port:**
- Windows: Device Manager → Ports (COM & LPT)
- Mac: /dev/cu.usbserial-*
- Linux: /dev/ttyUSB*

---

## PART 2: ESP8266 FIRMWARE UPLOAD

### Step 2.1: Prepare ESP8266 #1 (Trigger Unit)

1. Connect ESP8266 #1 to computer via USB
2. Open: `esp8266_trigger/esp8266_trigger.ino`
3. **IMPORTANT**: Update WiFi credentials (lines 35-36):
   ```cpp
   const char* ssid = "YOUR_WIFI_SSID";        // Your WiFi name
   const char* password = "YOUR_WIFI_PASSWORD"; // Your WiFi password
   ```
4. Verify MQTT broker settings (line 39):
   ```cpp
   const char* mqtt_server = "test.mosquitto.org"; // Public broker
   ```
5. Click **Verify** (✓) button to compile
6. Wait for "Done compiling" message
7. Click **Upload** (→) button
8. Wait for "Done uploading" message

### Step 2.2: Test ESP8266 #1

1. Open: **Tools → Serial Monitor**
2. Set baud rate to: **115200**
3. You should see:
   ```
   ============================================
     ESP8266 EMERGENCY TRIGGER UNIT
   ============================================
   Device ID: ESP1
   --- Connecting to WiFi ---
   ✓ WiFi Connected!
   IP Address: 192.168.x.x
   ✓ MQTT Connected!
   --- READY TO SEND ALERTS ---
   ```

4. Test commands by typing in Serial Monitor:
   - Type: `FIRE` and press Enter
   - You should see: "🚨 EMERGENCY ALERT" message
   - Type: `STATUS` to check device status

**Troubleshooting:**
- If WiFi fails: Check SSID and password
- If MQTT fails: Check internet connection
- If upload fails: Check COM port selection

### Step 2.3: Prepare ESP8266 #2 (Receiver Unit)

1. Disconnect ESP8266 #1
2. Connect ESP8266 #2 to computer via USB
3. Open: `esp8266_receiver/esp8266_receiver.ino`
4. Update WiFi credentials (same as ESP8266 #1)
5. Verify MQTT broker settings
6. Click **Verify** (✓) button
7. Click **Upload** (→) button
8. Wait for upload completion

### Step 2.4: Test ESP8266 #2

1. Open Serial Monitor (115200 baud)
2. You should see:
   ```
   ============================================
     ESP8266 RECEIVER & RELAY CONTROLLER
   ============================================
   Device ID: ESP2
   ✓ WiFi Connected!
   ✓ MQTT Connected!
   ✓ Subscribed to: building/alert
   ✓ Subscribed to: building/reset
   --- READY TO RECEIVE ALERTS ---
   ```

3. Keep Serial Monitor open for testing

---

## PART 3: HARDWARE WIRING

### Step 3.1: Wire ESP8266 #2 to Relay Module

**IMPORTANT**: Disconnect power before wiring!

1. **Control Connections:**
   - ESP8266 D1 (GPIO5) → Relay IN1
   - ESP8266 D2 (GPIO4) → Relay IN2
   - ESP8266 D5 (GPIO14) → Relay IN3
   - ESP8266 D6 (GPIO12) → Relay IN4
   - ESP8266 GND → Relay GND

2. **Relay Power:**
   - 5V Power Supply (+) → Relay VCC
   - 5V Power Supply (-) → Relay GND
   - **Connect all grounds together** (ESP8266 GND, Relay GND, 5V GND)

3. **ESP8266 Power:**
   - Option A: USB cable (for testing)
   - Option B: 7.4V battery to VIN pin

### Step 3.2: Verify Wiring

Use a multimeter to check:
- [ ] No short circuits between VCC and GND
- [ ] All control pins connected correctly
- [ ] Common ground established
- [ ] Relay module powered separately

### Step 3.3: Test Relay Operation

1. Power on ESP8266 #2
2. Wait for WiFi and MQTT connection
3. From ESP8266 #1 Serial Monitor, send: `FIRE`
4. You should hear relay click (Relay 1 activates)
5. Check Serial Monitor of ESP8266 #2:
   ```
   📨 MESSAGE RECEIVED
   Topic: building/alert
   Payload: FIRE
   🚨 ALERT TRIGGERED: FIRE
   → Activating FIRE pattern (Continuous siren)
   ```

---

## PART 4: BACKEND SERVER SETUP

### Step 4.1: Install Node.js

1. Download Node.js from: https://nodejs.org/
2. Choose LTS version (recommended)
3. Install following the installer
4. Verify installation:
   ```bash
   node --version
   npm --version
   ```
   Should show version numbers

### Step 4.2: Install Dependencies

1. Open terminal/command prompt
2. Navigate to backend directory:
   ```bash
   cd backend
   ```
3. Install npm packages:
   ```bash
   npm install
   ```
4. Wait for installation (may take 1-2 minutes)
5. You should see: "added X packages"

### Step 4.3: Start Server

1. In backend directory, run:
   ```bash
   npm start
   ```
2. You should see:
   ```
   ╔════════════════════════════════════════════╗
   ║  IoT Emergency Alert System - Backend     ║
   ╚════════════════════════════════════════════╝
   ✓ Server running on http://localhost:3000
   ✓ Login page: http://localhost:3000/login
   ✓ Dashboard: http://localhost:3000/dashboard
   
   --- Connecting to MQTT Broker ---
   ✓ MQTT Connected!
   ✓ Subscribed to: building/alert
   ✓ Subscribed to: building/status
   ```

**Troubleshooting:**
- Port 3000 already in use: Change PORT in server.js
- MQTT connection failed: Check internet connection
- Module not found: Run `npm install` again

---

## PART 5: WEB DASHBOARD ACCESS

### Step 5.1: Open Dashboard

1. Open web browser (Chrome, Firefox, Edge)
2. Navigate to: http://localhost:3000
3. You'll be redirected to login page

### Step 5.2: Login

1. Enter credentials:
   - **Username**: `admin`
   - **Password**: `admin123`
2. Click **Login**
3. You'll be redirected to dashboard

### Step 5.3: Dashboard Overview

You should see:
- **Current Alert Status**: Shows "No Active Alerts" (green)
- **Device Status**: Shows ESP1 and ESP2 status
- **Alert History**: Table of past alerts
- **Control Panel**: Acknowledge and Reset buttons

---

## PART 6: SYSTEM TESTING

### Test 1: Fire Alert

1. Open ESP8266 #1 Serial Monitor
2. Type: `FIRE` and press Enter
3. **Expected Results:**
   - ESP8266 #1: Shows "Alert published successfully"
   - ESP8266 #2: Relay 1 activates (continuous)
   - Dashboard: Shows "🔥 FIRE ALERT" in red
   - Dashboard: Alert appears in history table
   - Browser notification (if enabled)

### Test 2: Medical Alert

1. Type: `MEDICAL` in ESP8266 #1 Serial Monitor
2. **Expected Results:**
   - ESP8266 #2: Relay 2 pulses (500ms ON/OFF)
   - Dashboard: Shows "🏥 MEDICAL ALERT" in blue
   - Alert logged in history

### Test 3: Security Alert

1. Type: `SECURITY` in ESP8266 #1 Serial Monitor
2. **Expected Results:**
   - ESP8266 #2: Relays 3 & 4 alternate (200ms)
   - Dashboard: Shows "🔒 SECURITY ALERT" in yellow
   - Alert logged in history

### Test 4: Reset System

1. On dashboard, click **Reset System**
2. Confirm the action
3. **Expected Results:**
   - All relays turn OFF
   - Dashboard shows "No Active Alerts"
   - ESP8266 #2 Serial: "RESET COMMAND RECEIVED"

### Test 5: Device Health Monitoring

1. Disconnect ESP8266 #1 from power
2. Wait 30 seconds
3. **Expected Results:**
   - Dashboard shows ESP1 as "Offline"
   - Last seen timestamp displayed
4. Reconnect ESP8266 #1
5. **Expected Results:**
   - Dashboard shows ESP1 as "Online" within 10 seconds

---

## PART 7: CONFIGURATION OPTIONS

### Changing WiFi Credentials

Edit in both ESP8266 sketches:
```cpp
const char* ssid = "YOUR_NEW_SSID";
const char* password = "YOUR_NEW_PASSWORD";
```
Re-upload firmware to both ESP8266s.

### Changing MQTT Broker

To use a different MQTT broker:
1. Edit in both ESP8266 sketches:
   ```cpp
   const char* mqtt_server = "your.broker.com";
   ```
2. Edit in backend/server.js:
   ```javascript
   const MQTT_BROKER = 'mqtt://your.broker.com';
   ```
3. Re-upload firmware and restart server

### Changing Server Port

Edit in backend/server.js:
```javascript
const PORT = 3000; // Change to desired port
```
Restart server.

### Changing User Credentials

Edit in backend/server.js:
```javascript
const USERS = {
  admin: 'newpassword',
  user: 'user123'
};
```
Restart server.

---

## PART 8: RUNNING IN PRODUCTION

### For 24/7 Operation:

1. **Use Private MQTT Broker:**
   - Install Mosquitto locally
   - Configure authentication
   - Use MQTT over TLS

2. **Use Process Manager:**
   ```bash
   npm install -g pm2
   pm2 start server.js --name iot-alert
   pm2 save
   pm2 startup
   ```

3. **Use Database:**
   - Replace JSON file with MongoDB/MySQL
   - Implement proper data persistence

4. **Security Hardening:**
   - Use HTTPS for web dashboard
   - Implement strong password policies
   - Add rate limiting
   - Use environment variables for secrets

---

## ✅ Setup Complete!

Your IoT Emergency Alert System is now fully operational!

**Next Steps:**
- Read ARCHITECTURE.md for system details
- Read MQTT_EXPLAINED.md to understand the protocol
- Check TROUBLESHOOTING.md if you encounter issues
- Customize the system for your specific needs

---

**Need Help?**
- Check Serial Monitor for debug messages
- Review TROUBLESHOOTING.md
- Verify all connections
- Ensure internet connectivity
