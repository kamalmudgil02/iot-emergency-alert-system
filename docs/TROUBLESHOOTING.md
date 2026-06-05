# Troubleshooting Guide
## Common Issues and Solutions

---

## 🔧 ESP8266 Issues

### Issue 1: ESP8266 Won't Connect to WiFi

**Symptoms:**
- Serial Monitor shows "WiFi Connection Failed"
- Continuous dots (......) without connection
- Device keeps restarting

**Solutions:**

1. **Check WiFi Credentials**
   ```cpp
   const char* ssid = "YOUR_WIFI_SSID";     // Exact name, case-sensitive
   const char* password = "YOUR_WIFI_PASSWORD"; // Exact password
   ```
   - Verify SSID spelling (case-sensitive)
   - Check for extra spaces
   - Ensure password is correct

2. **Check WiFi Band**
   - ESP8266 only supports 2.4GHz WiFi
   - Does NOT support 5GHz networks
   - Check router settings

3. **Check WiFi Security**
   - Supported: WPA/WPA2
   - Not supported: WPA3, Enterprise
   - Try disabling MAC filtering temporarily

4. **Signal Strength**
   - Move ESP8266 closer to router
   - Check for interference
   - Use external antenna if available

5. **Router Issues**
   - Restart router
   - Check if DHCP is enabled
   - Verify router allows new connections

**Debug Code:**
```cpp
void setupWiFi() {
  Serial.println("Connecting to: " + String(ssid));
  WiFi.begin(ssid, password);
  
  int attempts = 0;
  while (WiFi.status() != WL_CONNECTED && attempts < 30) {
    delay(500);
    Serial.print(".");
    Serial.print(" Status: ");
    Serial.println(WiFi.status());
    attempts++;
  }
  
  if (WiFi.status() == WL_CONNECTED) {
    Serial.println("\nConnected!");
    Serial.println("IP: " + WiFi.localIP().toString());
    Serial.println("Signal: " + String(WiFi.RSSI()) + " dBm");
  }
}
```

---

### Issue 2: MQTT Connection Failed

**Symptoms:**
- "MQTT Connection Failed! Error code: -2"
- WiFi connected but MQTT fails
- Frequent disconnections

**Error Codes:**
- `-4`: Connection timeout
- `-3`: Connection lost
- `-2`: Connect failed
- `-1`: Disconnected
- `0`: Connected
- `1`: Bad protocol
- `2`: ID rejected
- `3`: Server unavailable
- `4`: Bad credentials
- `5`: Not authorized

**Solutions:**

1. **Check Internet Connection**
   ```cpp
   // Test with ping
   WiFiClient client;
   if (client.connect("google.com", 80)) {
     Serial.println("Internet OK");
   } else {
     Serial.println("No Internet");
   }
   ```

2. **Verify Broker Address**
   ```cpp
   const char* mqtt_server = "test.mosquitto.org"; // Check spelling
   const int mqtt_port = 1883; // Standard port
   ```

3. **Check Firewall**
   - Port 1883 must be open
   - Check router firewall
   - Check network firewall

4. **Try Different Broker**
   ```cpp
   // Alternative public brokers
   "broker.hivemq.com"
   "mqtt.eclipse.org"
   "test.mosquitto.org"
   ```

5. **Increase Timeout**
   ```cpp
   mqttClient.setSocketTimeout(30); // 30 seconds
   ```

---

### Issue 3: ESP8266 Keeps Restarting

**Symptoms:**
- Continuous boot messages
- "Soft WDT reset" or "Exception"
- Device never reaches main loop

**Solutions:**

1. **Power Supply Issues**
   - Use quality USB cable
   - Try different USB port
   - Use external 5V power supply
   - Check for voltage drops

2. **Watchdog Timer**
   ```cpp
   void loop() {
     // Add yield() or delay()
     yield(); // Prevents watchdog reset
     
     // Or use delay
     delay(10);
   }
   ```

3. **Memory Issues**
   ```cpp
   void loop() {
     Serial.print("Free Heap: ");
     Serial.println(ESP.getFreeHeap());
     
     // If < 10000, you have memory leak
   }
   ```

4. **Code Issues**
   - Remove blocking code
   - Avoid long delays in loop()
   - Check for infinite loops

---

### Issue 4: Upload Failed

**Symptoms:**
- "espcomm_open failed"
- "espcomm_upload_mem failed"
- "Timed out waiting for packet header"

**Solutions:**

1. **Check COM Port**
   - Verify correct port selected
   - Close Serial Monitor before upload
   - Try different USB port

2. **Install Drivers**
   - CH340/CH341 driver for cheap boards
   - CP2102 driver for official boards
   - Download from manufacturer website

3. **Hold FLASH Button**
   - Hold FLASH button on ESP8266
   - Click Upload in Arduino IDE
   - Release after "Connecting..." appears

4. **Check USB Cable**
   - Use data cable (not charge-only)
   - Try different cable
   - Check for loose connections

5. **Reset Board**
   - Press RST button
   - Disconnect and reconnect power
   - Try upload immediately after reset

---

## 🖥️ Backend Server Issues

### Issue 5: Server Won't Start

**Symptoms:**
- "Error: Cannot find module"
- "Port 3000 already in use"
- Server crashes on startup

**Solutions:**

1. **Missing Dependencies**
   ```bash
   cd backend
   rm -rf node_modules
   rm package-lock.json
   npm install
   ```

2. **Port Already in Use**
   ```bash
   # Windows
   netstat -ano | findstr :3000
   taskkill /PID <PID> /F
   
   # Linux/Mac
   lsof -i :3000
   kill -9 <PID>
   ```
   
   Or change port in server.js:
   ```javascript
   const PORT = 3001; // Use different port
   ```

3. **Node.js Version**
   ```bash
   node --version
   # Should be v14 or higher
   ```
   
   Update Node.js if needed

4. **File Permissions**
   ```bash
   # Linux/Mac
   chmod +x server.js
   ```

---

### Issue 6: MQTT Not Connecting (Backend)

**Symptoms:**
- "MQTT Connection Failed"
- Backend starts but no MQTT messages

**Solutions:**

1. **Check Broker URL**
   ```javascript
   const MQTT_BROKER = 'mqtt://test.mosquitto.org'; // Must include mqtt://
   ```

2. **Firewall Issues**
   - Allow Node.js through firewall
   - Check corporate firewall
   - Try different network

3. **Test MQTT Manually**
   ```bash
   # Install mosquitto-clients
   mosquitto_sub -h test.mosquitto.org -t "building/#" -v
   ```

4. **Check Internet**
   ```bash
   ping test.mosquitto.org
   ```

---

### Issue 7: Dashboard Not Loading

**Symptoms:**
- Blank page
- "Cannot GET /dashboard"
- 404 errors

**Solutions:**

1. **Check Server Running**
   - Verify server is started
   - Check console for errors
   - Restart server

2. **Check URL**
   ```
   Correct: http://localhost:3000/dashboard
   Wrong: http://localhost:3000dashboard
   ```

3. **Clear Browser Cache**
   - Ctrl+Shift+Delete (Chrome)
   - Clear cache and cookies
   - Try incognito mode

4. **Check File Paths**
   ```javascript
   // In server.js
   app.use(express.static(path.join(__dirname, 'public')));
   ```

5. **Check File Structure**
   ```
   backend/
   ├── server.js
   └── public/
       ├── dashboard.html
       ├── login.html
       ├── css/
       └── js/
   ```

---

## 🔌 Hardware Issues

### Issue 8: Relay Not Activating

**Symptoms:**
- No relay click sound
- Relay LED not lighting
- No response to alerts

**Solutions:**

1. **Check Wiring**
   ```
   ESP8266 D1 → Relay IN1 ✓
   ESP8266 D2 → Relay IN2 ✓
   ESP8266 GND → Relay GND ✓
   5V Supply → Relay VCC ✓
   ```

2. **Check Relay Power**
   - Measure voltage at VCC (should be 5V)
   - Check 5V power supply
   - Verify connections

3. **Test GPIO Pins**
   ```cpp
   void setup() {
     pinMode(D1, OUTPUT);
     
     // Test relay
     digitalWrite(D1, HIGH);
     delay(1000);
     digitalWrite(D1, LOW);
   }
   ```

4. **Check Relay Type**
   - Active LOW: HIGH = OFF, LOW = ON
   - Active HIGH: HIGH = ON, LOW = OFF
   - Check relay module documentation

5. **Common Ground**
   - ESP8266 GND must connect to Relay GND
   - All grounds must be common
   - Check with multimeter

---

### Issue 9: Relay Stuck ON

**Symptoms:**
- Relay won't turn off
- Continuous activation
- No response to reset

**Solutions:**

1. **Software Reset**
   - Send RESET command from dashboard
   - Restart ESP8266 #2
   - Check Serial Monitor for errors

2. **Check Code Logic**
   ```cpp
   void handleReset(String message) {
     if (message == "RESET") {
       digitalWrite(RELAY_FIRE, LOW);
       digitalWrite(RELAY_MEDICAL, LOW);
       digitalWrite(RELAY_SECURITY, LOW);
       digitalWrite(RELAY_SPARE, LOW);
     }
   }
   ```

3. **Hardware Issue**
   - Relay may be faulty
   - Check relay with multimeter
   - Try different relay channel

---

### Issue 10: Intermittent Operation

**Symptoms:**
- Works sometimes, fails other times
- Random disconnections
- Unreliable behavior

**Solutions:**

1. **Power Supply**
   - Use stable power source
   - Check battery voltage (should be >7V)
   - Use regulated power supply
   - Add capacitors (100µF) near ESP8266

2. **Loose Connections**
   - Check all jumper wires
   - Solder connections for permanent setup
   - Use quality breadboard

3. **WiFi Interference**
   - Move away from microwave
   - Avoid metal enclosures
   - Check for WiFi congestion
   - Change WiFi channel

4. **Code Issues**
   - Add reconnection logic
   - Implement error handling
   - Add watchdog timer

---

## 🌐 Network Issues

### Issue 11: Devices Online but Not Communicating

**Symptoms:**
- Both ESP8266s connected
- No messages received
- Dashboard shows offline

**Solutions:**

1. **Check MQTT Topics**
   ```cpp
   // Must match exactly
   Publisher: "building/alert"
   Subscriber: "building/alert"
   
   // Wrong:
   Publisher: "building/alert"
   Subscriber: "Building/alert" // Case sensitive!
   ```

2. **Check Callback Function**
   ```cpp
   void callback(char* topic, byte* payload, unsigned int length) {
     Serial.println("Message received!"); // Add debug
     // Process message
   }
   
   void setup() {
     mqttClient.setCallback(callback); // Don't forget this!
   }
   ```

3. **Check Subscription**
   ```cpp
   void connectMQTT() {
     if (mqttClient.connect("ESP8266")) {
       mqttClient.subscribe("building/alert"); // Must subscribe!
       Serial.println("Subscribed");
     }
   }
   ```

4. **Test with MQTT Explorer**
   - Install MQTT Explorer
   - Connect to same broker
   - Verify messages are published
   - Check topic names

---

### Issue 12: High Latency

**Symptoms:**
- Slow response time
- Delays in alert delivery
- Dashboard updates slowly

**Solutions:**

1. **Network Issues**
   - Check internet speed
   - Reduce network congestion
   - Use wired connection for server

2. **Broker Issues**
   - Public broker may be slow
   - Try different broker
   - Consider private broker

3. **QoS Level**
   ```cpp
   // QoS 0 is fastest
   mqttClient.publish("building/alert", "FIRE", false);
   ```

4. **Optimize Code**
   - Remove unnecessary delays
   - Reduce Serial.println() calls
   - Optimize loop() function

---

## 📱 Dashboard Issues

### Issue 13: Real-Time Updates Not Working

**Symptoms:**
- Dashboard doesn't update automatically
- Must refresh page to see changes
- WebSocket not connecting

**Solutions:**

1. **Check WebSocket Connection**
   - Open browser console (F12)
   - Look for WebSocket errors
   - Check for "Connected to server" message

2. **Check Socket.io**
   ```javascript
   // In dashboard.js
   const socket = io(); // Should auto-connect
   
   socket.on('connect', () => {
     console.log('Connected!');
   });
   
   socket.on('disconnect', () => {
     console.log('Disconnected!');
   });
   ```

3. **Firewall/Proxy**
   - WebSocket may be blocked
   - Try different network
   - Check browser extensions

4. **Browser Compatibility**
   - Use modern browser (Chrome, Firefox, Edge)
   - Update browser to latest version
   - Disable ad blockers

---

### Issue 14: Login Not Working

**Symptoms:**
- "Invalid credentials" error
- Can't access dashboard
- Session expires immediately

**Solutions:**

1. **Check Credentials**
   ```javascript
   // In server.js
   const USERS = {
     admin: 'admin123',
     user: 'user123'
   };
   ```
   - Username: `admin`
   - Password: `admin123`
   - Case sensitive!

2. **Clear Cookies**
   - Clear browser cookies
   - Try incognito mode
   - Restart browser

3. **Check Session**
   ```javascript
   // In server.js
   app.use(session({
     secret: 'your-secret-key',
     resave: false,
     saveUninitialized: false,
     cookie: { maxAge: 24 * 60 * 60 * 1000 }
   }));
   ```

---

## 🔍 Debugging Tips

### Enable Verbose Logging

**ESP8266:**
```cpp
void setup() {
  Serial.begin(115200);
  Serial.setDebugOutput(true); // Enable debug
}
```

**Backend:**
```javascript
// Add debug logging
console.log('Debug:', variable);

// MQTT debug
mqttClient.on('message', (topic, message) => {
  console.log('MQTT:', topic, message.toString());
});
```

### Use Serial Monitor Effectively

```cpp
Serial.println("=== DEBUG START ===");
Serial.print("WiFi Status: ");
Serial.println(WiFi.status());
Serial.print("MQTT State: ");
Serial.println(mqttClient.state());
Serial.print("Free Heap: ");
Serial.println(ESP.getFreeHeap());
Serial.println("=== DEBUG END ===");
```

### Browser Console

Press F12 in browser:
- Check Console tab for errors
- Check Network tab for failed requests
- Check Application tab for cookies/storage

---

## 📞 Getting Help

If you're still stuck:

1. **Check Serial Monitor**
   - ESP8266 #1 and #2
   - Look for error messages
   - Note error codes

2. **Check Server Console**
   - Look for error messages
   - Check MQTT connection status

3. **Check Browser Console**
   - Press F12
   - Look for JavaScript errors
   - Check network requests

4. **Document Your Issue**
   - What were you trying to do?
   - What happened instead?
   - Error messages?
   - Steps to reproduce?

5. **Test Components Individually**
   - Test ESP8266 WiFi alone
   - Test MQTT connection alone
   - Test relay activation alone
   - Test dashboard alone

---

## ✅ Prevention Checklist

Before asking for help, verify:

- [ ] All wiring connections are correct
- [ ] WiFi credentials are correct
- [ ] MQTT broker is accessible
- [ ] All grounds are connected
- [ ] Power supply is adequate
- [ ] Code is uploaded successfully
- [ ] Serial Monitor shows no errors
- [ ] Server is running
- [ ] Browser is up to date
- [ ] Firewall allows connections

---

**Most issues are caused by:**
1. Incorrect wiring (40%)
2. Wrong WiFi credentials (25%)
3. Power supply problems (15%)
4. MQTT configuration (10%)
5. Other (10%)

**Always check the basics first!**
