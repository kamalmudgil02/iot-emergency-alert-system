/*
 * ============================================================================
 * ESP8266 #2 - ALERT RECEIVER & RELAY CONTROLLER
 * ============================================================================
 * 
 * Purpose: Receives emergency alerts and controls relay module
 * 
 * Features:
 * - WiFi connectivity
 * - MQTT subscribe (building/alert, building/reset topics)
 * - Heartbeat status messages every 10 seconds
 * - Relay control with different patterns for each alert type
 * - LED status indicator
 * 
 * Alert Patterns:
 * - FIRE: Continuous relay ON (siren)
 * - MEDICAL: Pulsing pattern (500ms ON, 500ms OFF)
 * - SECURITY: Fast flashing (200ms ON, 200ms OFF, alternating relays)
 * 
 * Hardware:
 * - ESP8266 NodeMCU
 * - 4-Channel Relay Module
 * - D1 (GPIO5) → Relay IN1 (FIRE)
 * - D2 (GPIO4) → Relay IN2 (MEDICAL)
 * - D5 (GPIO14) → Relay IN3 (SECURITY)
 * - D6 (GPIO12) → Relay IN4 (SPARE)
 * - LED on D0 (GPIO16) - optional
 * 
 * Author: IoT Emergency Alert System
 * Date: 2026
 * ============================================================================
 */

#include <ESP8266WiFi.h>
#include <PubSubClient.h>

// ============================================================================
// CONFIGURATION - CHANGE THESE VALUES
// ============================================================================

// WiFi Credentials
const char* ssid = "YOUR_WIFI_SSID";           // Replace with your WiFi name
const char* password = "YOUR_WIFI_PASSWORD";   // Replace with your WiFi password

// MQTT Broker Settings
const char* mqtt_server = "test.mosquitto.org"; // Public MQTT broker
const int mqtt_port = 1883;                     // Standard MQTT port
const char* mqtt_client_id = "ESP8266_Receiver_Unit"; // Unique client ID

// MQTT Topics
const char* topic_alert = "building/alert";     // Subscribe to alerts
const char* topic_reset = "building/reset";     // Subscribe to reset commands
const char* topic_status = "building/status";   // Publish heartbeat status

// Device Identification
const char* device_id = "ESP2";                 // This device's ID
const char* device_name = "Receiver Unit";      // Human-readable name

// Pin Definitions
const int RELAY_FIRE = D1;      // GPIO5 - Relay 1 for FIRE alerts
const int RELAY_MEDICAL = D2;   // GPIO4 - Relay 2 for MEDICAL alerts
const int RELAY_SECURITY = D5;  // GPIO14 - Relay 3 for SECURITY alerts
const int RELAY_SPARE = D6;     // GPIO12 - Relay 4 for future use
const int LED_PIN = D0;         // GPIO16 - Status LED

// Timing Constants
const unsigned long HEARTBEAT_INTERVAL = 10000; // Send status every 10 seconds
const unsigned long RECONNECT_INTERVAL = 5000;  // Try reconnect every 5 seconds

// Alert Pattern Timing
const unsigned long MEDICAL_PULSE_INTERVAL = 500;   // 500ms for medical pulse
const unsigned long SECURITY_FLASH_INTERVAL = 200;  // 200ms for security flash

// ============================================================================
// GLOBAL VARIABLES
// ============================================================================

WiFiClient espClient;                    // WiFi client object
PubSubClient mqttClient(espClient);      // MQTT client object

unsigned long lastHeartbeat = 0;         // Last heartbeat timestamp
unsigned long lastReconnectAttempt = 0;  // Last reconnection attempt
unsigned long lastPatternUpdate = 0;     // Last pattern update time

bool wifiConnected = false;              // WiFi connection status
bool mqttConnected = false;              // MQTT connection status

// Alert state variables
String currentAlert = "NONE";            // Current active alert type
bool alertActive = false;                // Is an alert currently active?
bool patternState = false;               // For toggling patterns (ON/OFF)

// ============================================================================
// SETUP FUNCTION - Runs once at startup
// ============================================================================

void setup() {
  // Initialize Serial communication
  Serial.begin(115200);
  delay(100);
  
  Serial.println("\n\n");
  Serial.println("============================================");
  Serial.println("  ESP8266 RECEIVER & RELAY CONTROLLER");
  Serial.println("============================================");
  Serial.println("Device ID: " + String(device_id));
  Serial.println("Device Name: " + String(device_name));
  Serial.println("============================================\n");
  
  // Initialize relay pins (OUTPUT mode)
  pinMode(RELAY_FIRE, OUTPUT);
  pinMode(RELAY_MEDICAL, OUTPUT);
  pinMode(RELAY_SECURITY, OUTPUT);
  pinMode(RELAY_SPARE, OUTPUT);
  pinMode(LED_PIN, OUTPUT);
  
  // Turn off all relays initially (LOW = OFF for active-low relays)
  digitalWrite(RELAY_FIRE, LOW);
  digitalWrite(RELAY_MEDICAL, LOW);
  digitalWrite(RELAY_SECURITY, LOW);
  digitalWrite(RELAY_SPARE, LOW);
  digitalWrite(LED_PIN, LOW);
  
  Serial.println("✓ Relay pins initialized (all OFF)");
  
  // Connect to WiFi
  setupWiFi();
  
  // Configure MQTT client
  mqttClient.setServer(mqtt_server, mqtt_port);
  mqttClient.setCallback(mqttCallback);
  
  // Connect to MQTT broker
  connectMQTT();
  
  Serial.println("\n--- READY TO RECEIVE ALERTS ---\n");
}

// ============================================================================
// MAIN LOOP - Runs continuously
// ============================================================================

void loop() {
  // Maintain WiFi connection
  if (WiFi.status() != WL_CONNECTED) {
    wifiConnected = false;
    digitalWrite(LED_PIN, LOW);
    Serial.println("WiFi disconnected! Reconnecting...");
    setupWiFi();
  } else {
    wifiConnected = true;
  }
  
  // Maintain MQTT connection
  if (!mqttClient.connected()) {
    mqttConnected = false;
    digitalWrite(LED_PIN, LOW);
    
    unsigned long now = millis();
    if (now - lastReconnectAttempt > RECONNECT_INTERVAL) {
      lastReconnectAttempt = now;
      Serial.println("MQTT disconnected! Reconnecting...");
      connectMQTT();
    }
  } else {
    mqttConnected = true;
    digitalWrite(LED_PIN, HIGH); // LED on when connected
    mqttClient.loop(); // Process MQTT messages
  }
  
  // Send heartbeat status message
  sendHeartbeat();
  
  // Update relay patterns based on current alert
  updateRelayPatterns();
  
  // Small delay to prevent watchdog timer issues
  delay(10);
}

// ============================================================================
// WiFi SETUP FUNCTION
// ============================================================================

void setupWiFi() {
  Serial.println("\n--- Connecting to WiFi ---");
  Serial.print("SSID: ");
  Serial.println(ssid);
  
  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, password);
  
  int attempts = 0;
  while (WiFi.status() != WL_CONNECTED && attempts < 30) {
    delay(500);
    Serial.print(".");
    attempts++;
  }
  
  if (WiFi.status() == WL_CONNECTED) {
    Serial.println("\n✓ WiFi Connected!");
    Serial.print("IP Address: ");
    Serial.println(WiFi.localIP());
    Serial.print("Signal Strength: ");
    Serial.print(WiFi.RSSI());
    Serial.println(" dBm");
    wifiConnected = true;
  } else {
    Serial.println("\n✗ WiFi Connection Failed!");
    wifiConnected = false;
  }
}

// ============================================================================
// MQTT CONNECTION FUNCTION
// ============================================================================

void connectMQTT() {
  if (!wifiConnected) {
    Serial.println("Cannot connect to MQTT: WiFi not connected");
    return;
  }
  
  Serial.println("\n--- Connecting to MQTT Broker ---");
  Serial.print("Broker: ");
  Serial.print(mqtt_server);
  Serial.print(":");
  Serial.println(mqtt_port);
  
  if (mqttClient.connect(mqtt_client_id)) {
    Serial.println("✓ MQTT Connected!");
    mqttConnected = true;
    
    // Subscribe to topics
    mqttClient.subscribe(topic_alert);
    Serial.println("✓ Subscribed to: " + String(topic_alert));
    
    mqttClient.subscribe(topic_reset);
    Serial.println("✓ Subscribed to: " + String(topic_reset));
    
    // Publish online status
    String statusMsg = String(device_id) + ":ONLINE";
    mqttClient.publish(topic_status, statusMsg.c_str(), true);
    Serial.println("✓ Published: " + statusMsg);
    
  } else {
    Serial.print("✗ MQTT Connection Failed! Error code: ");
    Serial.println(mqttClient.state());
    mqttConnected = false;
  }
}

// ============================================================================
// MQTT CALLBACK - Handles incoming messages
// ============================================================================

void mqttCallback(char* topic, byte* payload, unsigned int length) {
  // Convert payload to string
  String message = "";
  for (unsigned int i = 0; i < length; i++) {
    message += (char)payload[i];
  }
  
  Serial.println("\n╔════════════════════════════════╗");
  Serial.println("║   📨 MESSAGE RECEIVED          ║");
  Serial.println("╚════════════════════════════════╝");
  Serial.println("Topic: " + String(topic));
  Serial.println("Payload: " + message);
  Serial.println("Length: " + String(length) + " bytes");
  
  // Handle alert messages
  if (String(topic) == topic_alert) {
    handleAlert(message);
  }
  // Handle reset messages
  else if (String(topic) == topic_reset) {
    handleReset(message);
  }
  
  Serial.println("════════════════════════════════\n");
}

// ============================================================================
// HANDLE ALERT
// ============================================================================

void handleAlert(String alertType) {
  Serial.println("\n🚨 ALERT TRIGGERED: " + alertType);
  
  currentAlert = alertType;
  alertActive = true;
  patternState = false;
  lastPatternUpdate = millis();
  
  // Turn off all relays first
  allRelaysOff();
  
  // Activate appropriate relay pattern
  if (alertType == "FIRE") {
    Serial.println("→ Activating FIRE pattern (Continuous siren)");
    digitalWrite(RELAY_FIRE, HIGH); // Continuous ON
  } 
  else if (alertType == "MEDICAL") {
    Serial.println("→ Activating MEDICAL pattern (Pulsing)");
    // Pattern will be handled in updateRelayPatterns()
  } 
  else if (alertType == "SECURITY") {
    Serial.println("→ Activating SECURITY pattern (Fast flashing)");
    // Pattern will be handled in updateRelayPatterns()
  }
  else {
    Serial.println("✗ Unknown alert type: " + alertType);
    alertActive = false;
  }
}

// ============================================================================
// HANDLE RESET
// ============================================================================

void handleReset(String message) {
  Serial.println("\n🔄 RESET COMMAND RECEIVED");
  
  if (message == "RESET") {
    Serial.println("→ Turning off all relays");
    Serial.println("→ Clearing alert state");
    
    // Turn off all relays
    allRelaysOff();
    
    // Clear alert state
    currentAlert = "NONE";
    alertActive = false;
    patternState = false;
    
    Serial.println("✓ System reset complete");
    
    // Send confirmation
    String statusMsg = String(device_id) + ":RESET_CONFIRMED";
    mqttClient.publish(topic_status, statusMsg.c_str());
  }
}

// ============================================================================
// UPDATE RELAY PATTERNS
// ============================================================================

void updateRelayPatterns() {
  if (!alertActive) {
    return; // No active alert, nothing to update
  }
  
  unsigned long now = millis();
  
  // FIRE pattern: Continuous (already set in handleAlert, no updates needed)
  if (currentAlert == "FIRE") {
    // Relay stays ON continuously
    // No pattern updates needed
  }
  
  // MEDICAL pattern: Pulsing (500ms ON, 500ms OFF)
  else if (currentAlert == "MEDICAL") {
    if (now - lastPatternUpdate >= MEDICAL_PULSE_INTERVAL) {
      lastPatternUpdate = now;
      patternState = !patternState; // Toggle state
      
      digitalWrite(RELAY_MEDICAL, patternState ? HIGH : LOW);
      
      // Debug output (comment out for production)
      // Serial.println("Medical pulse: " + String(patternState ? "ON" : "OFF"));
    }
  }
  
  // SECURITY pattern: Fast flashing alternating relays (200ms)
  else if (currentAlert == "SECURITY") {
    if (now - lastPatternUpdate >= SECURITY_FLASH_INTERVAL) {
      lastPatternUpdate = now;
      patternState = !patternState; // Toggle state
      
      if (patternState) {
        digitalWrite(RELAY_SECURITY, HIGH);
        digitalWrite(RELAY_SPARE, LOW);
      } else {
        digitalWrite(RELAY_SECURITY, LOW);
        digitalWrite(RELAY_SPARE, HIGH);
      }
      
      // Debug output (comment out for production)
      // Serial.println("Security flash: " + String(patternState ? "R3" : "R4"));
    }
  }
}

// ============================================================================
// TURN OFF ALL RELAYS
// ============================================================================

void allRelaysOff() {
  digitalWrite(RELAY_FIRE, LOW);
  digitalWrite(RELAY_MEDICAL, LOW);
  digitalWrite(RELAY_SECURITY, LOW);
  digitalWrite(RELAY_SPARE, LOW);
  Serial.println("✓ All relays OFF");
}

// ============================================================================
// SEND HEARTBEAT STATUS
// ============================================================================

void sendHeartbeat() {
  unsigned long now = millis();
  
  if (now - lastHeartbeat > HEARTBEAT_INTERVAL) {
    lastHeartbeat = now;
    
    if (mqttConnected) {
      String statusMsg = String(device_id) + ":ALIVE";
      
      if (mqttClient.publish(topic_status, statusMsg.c_str())) {
        Serial.println("❤ Heartbeat sent: " + statusMsg);
      } else {
        Serial.println("✗ Failed to send heartbeat");
      }
    }
  }
}

// ============================================================================
// END OF CODE
// ============================================================================
