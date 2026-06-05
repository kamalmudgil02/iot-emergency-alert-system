# Hardware Wiring Guide
## Complete Connection Diagrams with Explanations

---

## 🔌 ESP8266 #1 - EMERGENCY TRIGGER UNIT

### Purpose
This unit acts as the emergency trigger. It publishes alert messages to the MQTT broker when an emergency is detected.

### Components Needed
- 1x ESP8266 NodeMCU
- 1x LED (optional, for status indication)
- 1x 220Ω Resistor (for LED)
- 1x 7.4V Li-ion Battery (optional, for portable operation)
- Breadboard and jumper wires

### Pin Configuration

```
ESP8266 NodeMCU Pin Layout:
┌─────────────────────────────┐
│         ESP8266             │
│                             │
│  3V3  ────────────── RST    │
│  GND  ────────────── D0     │ ← Status LED (GPIO16)
│  TX   ────────────── D1     │
│  RX   ────────────── D2     │
│  D8   ────────────── D3     │
│  D7   ────────────── D4     │
│  D6   ────────────── 3V3    │
│  D5   ────────────── GND    │
│  GND  ────────────── D5     │
│  3V3  ────────────── D6     │
│  D4   ────────────── D7     │
│  D3   ────────────── D8     │
│  D2   ────────────── RX     │
│  D1   ────────────── TX     │
│  D0   ────────────── GND    │
│  RST  ────────────── 3V3    │
│                             │
│  VIN  ← 7.4V Battery (+)    │
│  GND  ← Battery (-)         │
│  USB  ← Programming/Power   │
└─────────────────────────────┘
```

### Wiring Connections

#### Status LED Connection
```
D0 (GPIO16) ──────┬──── [220Ω Resistor] ──── LED (+) ──── LED (-) ──── GND
                  │
                  └──── (Built-in LED on some boards)
```

**Explanation:**
- D0 pin outputs HIGH (3.3V) or LOW (0V)
- 220Ω resistor limits current to ~15mA (safe for LED)
- LED lights up when D0 is HIGH
- Used to indicate WiFi connection and alert status

#### Power Options

**Option 1: USB Power (Development)**
```
Computer USB ──── USB Port on ESP8266
```
- Use during programming and testing
- Provides stable 5V power
- Easy to monitor via Serial

**Option 2: Battery Power (Portable)**
```
7.4V Li-ion Battery (+) ──── VIN pin
7.4V Li-ion Battery (-) ──── GND pin
```
- For portable/wireless operation
- ESP8266 has onboard voltage regulator (3.3V)
- Battery voltage range: 7-12V (VIN pin)
- Typical runtime: 4-6 hours with 1200mAh battery

### Breadboard Layout (Text Diagram)

```
Breadboard:
─────────────────────────────────────
  +  Rail (Red)    ← 3.3V (if needed)
─────────────────────────────────────
  -  Rail (Blue)   ← GND
─────────────────────────────────────

Row 1:  ESP8266 NodeMCU (straddling center)
Row 5:  D0 ──── 220Ω ──── LED+ ──── LED- ──── GND Rail
Row 10: (Empty - reserved for future sensors)
```

### No Physical Buttons?
**Solution**: We simulate button presses via Serial Monitor
- Open Arduino IDE Serial Monitor
- Type commands: `FIRE`, `MEDICAL`, `SECURITY`
- Press Enter to trigger alerts
- This is perfect for demonstration and testing

---

## 🔌 ESP8266 #2 - RECEIVER & RELAY CONTROLLER

### Purpose
This unit receives alert messages via MQTT and controls the relay module to activate sirens/alarms based on the alert type.

### Components Needed
- 1x ESP8266 NodeMCU
- 1x 4-Channel 5V Relay Module
- 1x 7.4V Li-ion Battery (for ESP8266)
- 1x 5V Power Supply (for Relay Module - can use USB adapter)
- Breadboard and jumper wires
- Optional: Buzzer/Siren connected to relay outputs

### Pin Configuration

```
ESP8266 NodeMCU:
┌─────────────────────────────┐
│         ESP8266             │
│                             │
│  D1 (GPIO5)  ──────────────>│ Relay IN1 (FIRE)
│  D2 (GPIO4)  ──────────────>│ Relay IN2 (MEDICAL)
│  D5 (GPIO14) ──────────────>│ Relay IN3 (SECURITY)
│  D6 (GPIO12) ──────────────>│ Relay IN4 (SPARE)
│  D0 (GPIO16) ──────────────>│ Status LED
│                             │
│  GND ───────────────────────│ Common Ground with Relay
│  VIN ───────────────────────│ 7.4V Battery (+)
└─────────────────────────────┘
```

### 4-Channel Relay Module Pinout

```
┌─────────────────────────────────────┐
│      4-Channel Relay Module         │
│                                     │
│  Control Side:                      │
│  ┌─────────────────────┐            │
│  │ VCC  ← 5V Power     │            │
│  │ GND  ← Ground       │            │
│  │ IN1  ← D1 (GPIO5)   │            │
│  │ IN2  ← D2 (GPIO4)   │            │
│  │ IN3  ← D5 (GPIO14)  │            │
│  │ IN4  ← D6 (GPIO12)  │            │
│  └─────────────────────┘            │
│                                     │
│  Relay Outputs (Each Channel):     │
│  ┌─────────────────────┐            │
│  │ COM  (Common)       │            │
│  │ NO   (Normally Open)│            │
│  │ NC   (Normally Closed)          │
│  └─────────────────────┘            │
└─────────────────────────────────────┘
```

### Complete Wiring Diagram

```
ESP8266 #2                    4-CH Relay Module              Load (Siren/Buzzer)
┌──────────┐                 ┌──────────────┐               ┌──────────┐
│          │                 │              │               │          │
│  D1 ─────┼────────────────>│ IN1          │               │          │
│  D2 ─────┼────────────────>│ IN2          │               │          │
│  D5 ─────┼────────────────>│ IN3          │               │          │
│  D6 ─────┼────────────────>│ IN4          │               │          │
│          │                 │              │               │          │
│  GND ────┼────┬───────────>│ GND          │               │          │
│          │    │            │              │               │          │
│  VIN ────┼─── │            │ VCC ←────────┼─── 5V Supply  │          │
│   ↑      │    │            │              │               │          │
│   │      │    │            │  Relay 1:    │               │          │
│ 7.4V     │    │            │  COM ────────┼──────────────>│ Siren +  │
│ Battery  │    │            │  NO          │               │          │
│          │    │            │  NC          │               │  Siren - │
└──────────┘    │            │              │               │    ↓     │
                │            └──────────────┘               │   GND    │
                │                                           └──────────┘
                └──────────────────────────────────────────────────────────> Common GND
```

### Detailed Connection Table

| ESP8266 Pin | Relay Module Pin | Wire Color (Suggested) | Function |
|-------------|------------------|------------------------|----------|
| D1 (GPIO5)  | IN1              | Yellow                 | FIRE Alert Control |
| D2 (GPIO4)  | IN2              | Green                  | MEDICAL Alert Control |
| D5 (GPIO14) | IN3              | Blue                   | SECURITY Alert Control |
| D6 (GPIO12) | IN4              | White                  | Spare/Future Use |
| GND         | GND              | Black                  | Common Ground |
| -           | VCC              | Red (from 5V supply)   | Relay Power |

### Power Supply Configuration

```
Power Setup:

1. ESP8266 Power:
   7.4V Battery (+) ──── VIN pin
   7.4V Battery (-) ──── GND pin

2. Relay Module Power:
   5V USB Adapter (+) ──── VCC pin on Relay
   5V USB Adapter (-) ──── GND pin on Relay

3. Common Ground:
   ESP8266 GND ──── Relay GND ──── Battery (-) ──── 5V Supply (-)
   
   ⚠️ IMPORTANT: All grounds must be connected together!
```

### Why Separate Power for Relay?

**Explanation:**
- Relays draw high current (70-80mA per channel when active)
- ESP8266 can only provide ~12mA per GPIO pin
- Relay module has its own power input (VCC) for coil activation
- ESP8266 GPIO pins only provide the control signal (LOW/HIGH)
- Separate 5V supply ensures stable relay operation

### How Relay Works (Simple Explanation)

```
Relay Internal Mechanism:

When GPIO is LOW (0V):
┌─────────────────┐
│   Coil: OFF     │
│                 │
│   COM ─┐        │
│        │  (gap) │
│   NO   │        │ ← Switch OPEN
│        X        │
│   NC ──┘        │ ← Switch CLOSED
└─────────────────┘

When GPIO is HIGH (3.3V):
┌─────────────────┐
│   Coil: ON      │ ← Electromagnet activated
│                 │
│   COM ──┐       │
│         │       │
│   NO ───┘       │ ← Switch CLOSED (connected)
│                 │
│   NC     (gap)  │ ← Switch OPEN
└─────────────────┘
```

**In Our Project:**
- We use NO (Normally Open) terminals
- When alert triggered: GPIO HIGH → Relay ON → Siren connected
- When reset: GPIO LOW → Relay OFF → Siren disconnected

### Connecting a Siren/Buzzer to Relay

```
Example: Connecting 12V Siren to Relay 1

12V Power Supply (+) ──── Siren (+)
                          Siren (-) ──── Relay1 COM
                                         Relay1 NO ──── 12V Power Supply (-)

When Relay1 is ON:
- COM connects to NO
- Circuit completes
- Siren sounds

When Relay1 is OFF:
- COM disconnects from NO
- Circuit breaks
- Siren silent
```

### Safety Considerations

⚠️ **IMPORTANT SAFETY NOTES:**

1. **Voltage Ratings:**
   - Relay module can switch up to 250V AC or 30V DC
   - For this project, use low voltage loads (5-12V)
   - Never exceed relay ratings

2. **Current Ratings:**
   - Each relay can handle up to 10A
   - Typical siren/buzzer: 100-500mA
   - Well within safe limits

3. **Isolation:**
   - Relay provides electrical isolation
   - Control circuit (ESP8266) is separate from load circuit
   - Protects ESP8266 from high voltage/current

4. **Common Ground:**
   - Always connect all grounds together
   - Prevents floating voltages
   - Ensures proper signal reference

5. **Power Supply:**
   - Use regulated power supplies
   - Check polarity before connecting
   - Avoid reverse polarity (can damage components)

### Breadboard Layout (Text Diagram)

```
Large Breadboard Layout:

─────────────────────────────────────────────────────────
  +5V Rail (Red)    ← 5V Supply for Relay
─────────────────────────────────────────────────────────
  GND Rail (Blue)   ← Common Ground
─────────────────────────────────────────────────────────

Left Side:
Row 1-10:  ESP8266 NodeMCU (straddling center)

Right Side:
Row 15-20: Relay Module (if breadboard-compatible)
           OR place relay module separately and use jumper wires

Connections:
- D1 to Relay IN1 (Yellow wire)
- D2 to Relay IN2 (Green wire)
- D5 to Relay IN3 (Blue wire)
- D6 to Relay IN4 (White wire)
- ESP GND to GND Rail (Black wire)
- Relay GND to GND Rail (Black wire)
- Relay VCC to +5V Rail (Red wire)
```

---

## 🔋 BATTERY CONFIGURATION

### 7.4V Li-ion Battery Specifications
- **Voltage**: 7.4V (2S configuration - two 3.7V cells in series)
- **Capacity**: 1200mAh
- **Chemistry**: Lithium-ion
- **Connector**: JST or similar

### Battery Connection

```
Battery Pack:
┌─────────────────┐
│   7.4V 1200mAh  │
│   Li-ion Pack   │
│                 │
│   Red (+) ──────┼──── VIN pin on ESP8266
│   Black (-) ────┼──── GND pin on ESP8266
└─────────────────┘

⚠️ Polarity Check:
- Red wire = Positive (+)
- Black wire = Negative (-)
- Double-check before connecting!
```

### Battery Life Estimation

**ESP8266 Power Consumption:**
- Active (WiFi on): ~80mA
- Transmitting: ~170mA (peak)
- Average: ~100mA

**Calculation:**
- Battery: 1200mAh
- Average current: 100mA
- Runtime: 1200mAh / 100mA = 12 hours (theoretical)
- Practical runtime: 8-10 hours (accounting for inefficiencies)

### Charging
- Use appropriate Li-ion charger (7.4V/2S)
- Never charge unattended
- Disconnect from ESP8266 while charging
- Check for balanced charging (both cells equal voltage)

---

## 🧪 TESTING CHECKLIST

### Before Powering On:

- [ ] Double-check all connections against diagrams
- [ ] Verify polarity of power supplies
- [ ] Ensure common ground connection
- [ ] Check for short circuits (use multimeter)
- [ ] Verify relay module has separate 5V power
- [ ] Confirm GPIO pins match code configuration

### After Powering On:

- [ ] ESP8266 LED should blink (booting)
- [ ] Check Serial Monitor for WiFi connection
- [ ] Verify MQTT connection messages
- [ ] Test relay activation (should hear click)
- [ ] Measure GPIO voltage: HIGH = 3.3V, LOW = 0V
- [ ] Check relay LED indicators (should light when active)

### Troubleshooting:

**ESP8266 won't boot:**
- Check power supply voltage (should be 7-12V on VIN or 3.3V on 3V3)
- Try USB power first
- Check for loose connections

**Relay won't activate:**
- Verify 5V power to relay module
- Check GPIO pin connections
- Test GPIO with LED first
- Ensure common ground connection

**Intermittent operation:**
- Check battery voltage (should be >7V)
- Tighten all connections
- Check for loose jumper wires
- Verify breadboard contact quality

---

## 📸 Visual Reference (Text-Based)

### Complete System Layout

```
Workbench Layout:

┌─────────────────────────────────────────────────────────────┐
│                                                             │
│  [ESP8266 #1]          [ESP8266 #2]      [Relay Module]    │
│   Trigger Unit          Receiver          4-Channel         │
│       │                     │                  │            │
│       │                     │                  │            │
│   [Battery]             [Battery]         [5V Supply]       │
│    7.4V                  7.4V                               │
│                                                             │
│                     [Breadboard]                            │
│                                                             │
│                  [Computer with Arduino IDE]                │
│                                                             │
└─────────────────────────────────────────────────────────────┘

WiFi Router (for network connectivity)
```

---

## ✅ Final Wiring Verification

Before uploading code, verify:

1. **ESP8266 #1:**
   - Power connected (USB or battery)
   - Status LED on D0 (optional)
   - Can access Serial Monitor

2. **ESP8266 #2:**
   - Power connected to VIN/GND
   - D1, D2, D5, D6 connected to relay IN1-IN4
   - Relay module has separate 5V power
   - Common ground established
   - Status LED on D0 (optional)

3. **Relay Module:**
   - VCC connected to 5V supply
   - GND connected to common ground
   - IN1-IN4 connected to ESP8266 GPIOs
   - Relay outputs ready for load connection

---

**You're now ready to upload the firmware! 🚀**

Next step: Configure WiFi credentials and MQTT broker settings in the code.
