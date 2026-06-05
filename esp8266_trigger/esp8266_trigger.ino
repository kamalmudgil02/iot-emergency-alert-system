/*
 * ============================================================================
 * ESP8266 #1 - EMERGENCY TRIGGER UNIT
 * ============================================================================
 * 
 * Purpose: Publishes emergency alerts to MQTT broker
 * 
 * Features:
 * - WiFi connectivity
 * - MQTT publish (building/alert topic)
 * - Heartbeat status messages every 10 seconds
 * - Serial command interface for triggering alerts
 * - LED status indicator
 * 
 * Commands via Serial Monitor:
 * - Type "FIRE" and press Enter → Triggers fire alert
 * - Type "MEDICAL" and press Enter → Triggers medical alert
 * - Type "SECURITY" and press Enter → Triggers security alert
 * - Type "STATUS" and press Enter → Shows device status
 * 
 * Hardware:
 * - ESP8266 NodeMCU
 * - LED on D0 (GPIO16) - optional
 * - Power via USB or 7.4V battery on VIN
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
const char* ssid = "Kamal_WiFi";           // Replace with your WiFi name
const char* password = "12345678";   // Replace with your WiFi password

// MQTT Broker Settings
const char* mqtt_server = "test.mosquitto.org"; // Public MQTT broker
const int mqtt_port = 1883;                     // Standard MQTT port
const char* mqtt_client_id = "ESP8266_Trigger_Unit"; // Unique client ID

// MQTT Topics
const char* topic_alert = "building/alert";     // Topic for emergency alerts
const char* topic_status = "building/status";   // Topic for heartbeat status

// Device Identification
const char* device_id = "ESP1";                 // This device's ID
const char* device_name = "Trigger Unit";       // Human-readable name

// Pin Definitions
const int LED_PIN = D0;  // GPIO16 - Status LED (built-in on some boards)

// Timing Constants
const unsigned long HEARTBEAT_INTERVAL = 10000; // Send status every 10 seconds
const unsigned long RECONNECT_INTERVAL = 5000;  // Try reconnect every 5 seconds

// ============================================================================
// GLOBAL VARIABLES
// ============================================================================

WiFiClient espClient;                    // WiFi client object
PubSubClient mqttClient(espClient);      // MQTT client object

unsigned long lastHeartbeat = 0;         // Last heartbeat timestamp
unsigned long lastReconnectAttempt = 0;  // Last reconnection attempt
bool wifiConnected = false;              // WiFi connection status
bool mqttConnected = false;              // MQTT connection status

String serialInput = "";                 // Buffer for serial input
bool serialComplete = false;             // Flag for complete serial line

// ============================================================================
// SETUP FUNCTION - Runs once at startup
// ============================================================================

void setup() {
  // Initialize Serial communication for debugging and commands
  Serial.begin(115200);
  delay(100);
  
  Serial.println("\n\n");
  Serial.println("============================================");
  Serial.println("  ESP8266 EMERGENCY TRIGGER UNIT");
  Serial.println("============================================");
  Serial.println("Device ID: " + String(device_id));
  Serial.println("Device Name: " + String(device_name));
  Serial.println("============================================\n");
  
  // Initialize LED pin
  pinMode(LED_PIN, OUTPUT);
  digitalWrite(LED_PIN, LOW); // LED off initially
  
  // Connect to WiFi
  setupWiFi();
  
  // Configure MQTT client
  mqttClient.setServer(mqtt_server, mqtt_port);
  mqttClient.setCallback(mqttCallback); // Set callback for incoming messages
  
  // Connect to MQTT broker
  connectMQTT();
  
  Serial.println("\n--- READY TO SEND ALERTS ---");
  Serial.println("Commands:");
  Serial.println("  FIRE     - Trigger fire alert");
  Serial.println("  MEDICAL  - Trigger medical alert");
  Serial.println("  SECURITY - Trigger security alert");
  Serial.println("  STATUS   - Show device status");
  Serial.println("================================\n");
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
  
  // Process serial commands
  processSerialCommands();
  
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
  
  // Start WiFi connection
  WiFi.mode(WIFI_STA); // Station mode (client)
  WiFi.begin(ssid, password);
  
  // Wait for connection with timeout
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
    Serial.println("Please check your credentials and try again.");
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
  
  // Attempt to connect
  if (mqttClient.connect(mqtt_client_id)) {
    Serial.println("✓ MQTT Connected!");
    mqttConnected = true;
    
    // Publish online status
    String statusMsg = String(device_id) + ":ONLINE";
    mqttClient.publish(topic_status, statusMsg.c_str(), true);
    
    Serial.println("Published: " + statusMsg + " to " + String(topic_status));
    
  } else {
    Serial.print("✗ MQTT Connection Failed! Error code: ");
    Serial.println(mqttClient.state());
    Serial.println("Error codes:");
    Serial.println("  -4 : Connection timeout");
    Serial.println("  -3 : Connection lost");
    Serial.println("  -2 : Connect failed");
    Serial.println("  -1 : Disconnected");
    Serial.println("   0 : Connected");
    mqttConnected = false;
  }
}

// ============================================================================
// MQTT CALLBACK - Handles incoming messages (not used in trigger unit)
// ============================================================================

void mqttCallback(char* topic, byte* payload, unsigned int length) {
  // This trigger unit doesn't subscribe to topics, but callback is required
  Serial.print("Message received on topic: ");
  Serial.println(topic);
}

// ============================================================================
// SEND HEARTBEAT STATUS
// ============================================================================

void sendHeartbeat() {
  unsigned long now = millis();
  
  // Send heartbeat every HEARTBEAT_INTERVAL milliseconds
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
// PROCESS SERIAL COMMANDS
// ============================================================================

void processSerialCommands() {
  // Read serial input
  while (Serial.available()) {
    char inChar = (char)Serial.read();
    
    if (inChar == '\n' || inChar == '\r') {
      serialComplete = true;
    } else {
      serialInput += inChar;
    }
  }
  
  // Process complete command
  if (serialComplete) {
    serialInput.trim(); // Remove whitespace
    serialInput.toUpperCase(); // Convert to uppercase
    
    if (serialInput.length() > 0) {
      handleCommand(serialInput);
    }
    
    // Reset for next command
    serialInput = "";
    serialComplete = false;
  }
}

// ============================================================================
// HANDLE COMMAND
// ============================================================================

void handleCommand(String command) {
  Serial.println("\n>>> Command received: " + command);
  
  // Check connection status
  if (!mqttConnected) {
    Serial.println("✗ Cannot send alert: MQTT not connected");
    return;
  }
  
  // Process commands
  if (command == "FIRE") {
    publishAlert("FIRE");
  } 
  else if (command == "MEDICAL") {
    publishAlert("MEDICAL");
  } 
  else if (command == "SECURITY") {
    publishAlert("SECURITY");
  } 
  else if (command == "STATUS") {
    showStatus();
  } 
  else {
    Serial.println("✗ Unknown command: " + command);
    Serial.println("Valid commands: FIRE, MEDICAL, SECURITY, STATUS");
  }
}

// ============================================================================
// PUBLISH ALERT
// ============================================================================

void publishAlert(String alertType) {
  Serial.println("\n╔════════════════════════════════╗");
  Serial.println("║   🚨 EMERGENCY ALERT 🚨       ║");
  Serial.println("╚════════════════════════════════╝");
  Serial.println("Type: " + alertType);
  Serial.println("Device: " + String(device_id));
  Serial.println("Time: " + String(millis() / 1000) + "s");
  
  // Publish to MQTT
  if (mqttClient.publish(topic_alert, alertType.c_str())) {
    Serial.println("✓ Alert published successfully!");
    Serial.println("Topic: " + String(topic_alert));
    Serial.println("Payload: " + alertType);
    
    // Blink LED to indicate alert sent
    for (int i = 0; i < 3; i++) {
      digitalWrite(LED_PIN, LOW);
      delay(100);
      digitalWrite(LED_PIN, HIGH);
      delay(100);
    }
  } else {
    Serial.println("✗ Failed to publish alert!");
  }
  
  Serial.println("════════════════════════════════\n");
}

// ============================================================================
// SHOW STATUS
// ============================================================================

void showStatus() {
  Serial.println("\n╔════════════════════════════════╗");
  Serial.println("║      DEVICE STATUS             ║");
  Serial.println("╚════════════════════════════════╝");
  Serial.println("Device ID: " + String(device_id));
  Serial.println("Device Name: " + String(device_name));
  Serial.println("─────────────────────────────────");
  Serial.println("WiFi Status: " + String(wifiConnected ? "✓ Connected" : "✗ Disconnected"));
  if (wifiConnected) {
    Serial.println("  SSID: " + String(ssid));
    Serial.println("  IP: " + WiFi.localIP().toString());
    Serial.println("  Signal: " + String(WiFi.RSSI()) + " dBm");
  }
  Serial.println("─────────────────────────────────");
  Serial.println("MQTT Status: " + String(mqttConnected ? "✓ Connected" : "✗ Disconnected"));
  if (mqttConnected) {
    Serial.println("  Broker: " + String(mqtt_server));
    Serial.println("  Port: " + String(mqtt_port));
    Serial.println("  Client ID: " + String(mqtt_client_id));
  }
  Serial.println("─────────────────────────────────");
  Serial.println("Uptime: " + String(millis() / 1000) + " seconds");
  Serial.println("Free Heap: " + String(ESP.getFreeHeap()) + " bytes");
  Serial.println("════════════════════════════════\n");
}

// ============================================================================
// END OF CODE
// ============================================================================
