# Quick Start Guide
## Get Your System Running in 30 Minutes

---

## ⚡ Prerequisites

- 2x ESP8266 NodeMCU
- 1x 4-Channel Relay Module
- Arduino IDE installed
- Node.js installed
- WiFi network (2.4GHz)

---

## 🚀 5-Step Setup

### Step 1: Install Arduino Libraries (5 min)

1. Open Arduino IDE
2. Go to: **Tools → Board → Boards Manager**
3. Search and install: **ESP8266**
4. Go to: **Sketch → Include Library → Manage Libraries**
5. Search and install: **PubSubClient**

### Step 2: Upload ESP8266 Firmware (10 min)

**ESP8266 #1 (Trigger):**
1. Open: `esp8266_trigger/esp8266_trigger.ino`
2. Edit lines 35-36:
   ```cpp
   const char* ssid = "YOUR_WIFI_NAME";
   const char* password = "YOUR_WIFI_PASSWORD";
   ```
3. Select: **Tools → Board → NodeMCU 1.0**
4. Select: **Tools → Port → [Your COM Port]**
5. Click: **Upload** (→)
6. Wait for "Done uploading"

**ESP8266 #2 (Receiver):**
1. Disconnect ESP8266 #1, connect ESP8266 #2
2. Open: `esp8266_receiver/esp8266_receiver.ino`
3. Edit WiFi credentials (same as above)
4. Click: **Upload** (→)
5. Wait for "Done uploading"

### Step 3: Wire Hardware (5 min)

**Connect ESP8266 #2 to Relay:**
```
ESP8266 D1 → Relay IN1
ESP8266 D2 → Relay IN2
ESP8266 D5 → Relay IN3
ESP8266 D6 → Relay IN4
ESP8266 GND → Relay GND
5V Supply → Relay VCC
```

**Important:** Connect all grounds together!

### Step 4: Start Backend Server (5 min)

```bash
cd backend
npm install
npm start
```

Wait for:
```
✓ Server running on http://localhost:3000
✓ MQTT Connected!
```

### Step 5: Test System (5 min)

1. Open browser: http://localhost:3000
2. Login:
   - Username: `admin`
   - Password: `admin123`
3. Open Serial Monitor for ESP8266 #1 (115200 baud)
4. Type: `FIRE` and press Enter
5. Check:
   - ✅ Relay clicks (ESP8266 #2)
   - ✅ Dashboard shows alert
   - ✅ Alert appears in history

---

## 🎯 Quick Test Commands

In ESP8266 #1 Serial Monitor:
- `FIRE` - Fire alert (continuous relay)
- `MEDICAL` - Medical alert (pulsing relay)
- `SECURITY` - Security alert (flashing relay)
- `STATUS` - Show device status

---

## 🔧 Quick Troubleshooting

**WiFi won't connect?**
- Check SSID and password spelling
- Ensure 2.4GHz network (not 5GHz)
- Move closer to router

**MQTT fails?**
- Check internet connection
- Try: `ping test.mosquitto.org`
- Restart ESP8266

**Relay won't activate?**
- Check wiring connections
- Verify 5V power to relay
- Check common ground

**Dashboard not loading?**
- Verify server is running
- Check: http://localhost:3000
- Clear browser cache

---

## 📱 Dashboard Features

- **Current Alert**: Shows active emergency
- **Device Status**: ESP1 and ESP2 online/offline
- **Alert History**: Past 20 alerts
- **Acknowledge**: Mark alert as seen
- **Reset**: Turn off all relays

---

## 🎓 Next Steps

1. Read `README.md` for full documentation
2. Check `docs/ARCHITECTURE.md` for system design
3. Review `docs/WIRING_GUIDE.md` for detailed wiring
4. Study `docs/MQTT_EXPLAINED.md` to understand protocol
5. Keep `docs/TROUBLESHOOTING.md` handy

---

## 💡 Tips

- Keep Serial Monitor open for debugging
- Use quality USB cables
- Ensure stable power supply
- Test one component at a time
- Check all connections twice

---

## 🆘 Need Help?

1. Check Serial Monitor for errors
2. Verify all wiring connections
3. Restart ESP8266 devices
4. Restart backend server
5. See `docs/TROUBLESHOOTING.md`

---

**You're ready to go! 🚀**

Your IoT Emergency Alert System should now be fully operational. Trigger alerts from ESP8266 #1 and watch them appear in real-time on your dashboard!
