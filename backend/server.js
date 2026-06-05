/*
 * ============================================================================
 * BACKEND SERVER - IoT Emergency Alert System
 * ============================================================================
 * 
 * Purpose: Central server for managing alerts, device monitoring, and dashboard
 * 
 * Features:
 * - Express.js web server
 * - MQTT client (subscribe to alerts and status)
 * - WebSocket (Socket.io) for real-time dashboard updates
 * - Session-based authentication
 * - Alert logging (JSON file storage)
 * - Device health monitoring (heartbeat tracking)
 * - REST API endpoints
 * 
 * Author: IoT Emergency Alert System
 * Date: 2026
 * ============================================================================
 */

const express = require('express');
const http = require('http');
const socketIO = require('socket.io');
const mqtt = require('mqtt');
const session = require('express-session');
const bodyParser = require('body-parser');
const fs = require('fs');
const path = require('path');

// ============================================================================
// CONFIGURATION
// ============================================================================

const PORT = 3000;                              // Web server port
const MQTT_BROKER = 'mqtt://test.mosquitto.org'; // MQTT broker URL
const MQTT_TOPIC_ALERT = 'building/alert';      // Alert topic
const MQTT_TOPIC_STATUS = 'building/status';    // Status topic
const MQTT_TOPIC_RESET = 'building/reset';      // Reset topic

const DEVICE_OFFLINE_TIMEOUT = 30000;           // 30 seconds
const ALERTS_FILE = path.join(__dirname, 'data', 'alerts.json');

// Simple user credentials (in production, use database with hashed passwords)
const USERS = {
  admin: 'admin123',  // username: password
  user: 'user123'
};

// ============================================================================
// INITIALIZE EXPRESS APP
// ============================================================================

const app = express();
const server = http.createServer(app);
const io = socketIO(server);

// Middleware
app.use(bodyParser.json());
app.use(bodyParser.urlencoded({ extended: true }));

// Session configuration
app.use(session({
  secret: 'iot-emergency-alert-secret-key-2026',
  resave: false,
  saveUninitialized: false,
  cookie: { 
    maxAge: 24 * 60 * 60 * 1000, // 24 hours
    httpOnly: true
  }
}));

// Serve static files from public directory
app.use(express.static(path.join(__dirname, 'public')));

// ============================================================================
// DATA STORAGE
// ============================================================================

// In-memory device status
let deviceStatus = {
  ESP1: {
    id: 'ESP1',
    name: 'Trigger Unit',
    status: 'offline',
    lastSeen: null,
    ip: null
  },
  ESP2: {
    id: 'ESP2',
    name: 'Receiver Unit',
    status: 'offline',
    lastSeen: null,
    ip: null
  }
};

// Current alert state
let currentAlert = {
  active: false,
  type: null,
  timestamp: null,
  device: null
};

// Initialize alerts file if it doesn't exist
function initializeAlertsFile() {
  const dataDir = path.join(__dirname, 'data');
  
  if (!fs.existsSync(dataDir)) {
    fs.mkdirSync(dataDir, { recursive: true });
    console.log('✓ Created data directory');
  }
  
  if (!fs.existsSync(ALERTS_FILE)) {
    const initialData = { alerts: [] };
    fs.writeFileSync(ALERTS_FILE, JSON.stringify(initialData, null, 2));
    console.log('✓ Created alerts.json file');
  }
}

// Load alerts from file
function loadAlerts() {
  try {
    const data = fs.readFileSync(ALERTS_FILE, 'utf8');
    return JSON.parse(data);
  } catch (error) {
    console.error('Error loading alerts:', error);
    return { alerts: [] };
  }
}

// Save alerts to file
function saveAlerts(alertsData) {
  try {
    fs.writeFileSync(ALERTS_FILE, JSON.stringify(alertsData, null, 2));
    return true;
  } catch (error) {
    console.error('Error saving alerts:', error);
    return false;
  }
}

// Add new alert
function addAlert(type, device) {
  const alertsData = loadAlerts();
  const newAlert = {
    id: `alert_${Date.now()}`,
    type: type,
    timestamp: new Date().toISOString(),
    acknowledged: false,
    acknowledgedAt: null,
    device: device
  };
  
  alertsData.alerts.unshift(newAlert); // Add to beginning
  
  // Keep only last 100 alerts
  if (alertsData.alerts.length > 100) {
    alertsData.alerts = alertsData.alerts.slice(0, 100);
  }
  
  saveAlerts(alertsData);
  return newAlert;
}

// ============================================================================
// MQTT CLIENT SETUP
// ============================================================================

console.log('\n--- Connecting to MQTT Broker ---');
console.log('Broker:', MQTT_BROKER);

const mqttClient = mqtt.connect(MQTT_BROKER, {
  clientId: 'NodeJS_Backend_Server_' + Math.random().toString(16).substr(2, 8),
  clean: true,
  reconnectPeriod: 5000
});

// MQTT connection event
mqttClient.on('connect', () => {
  console.log('✓ MQTT Connected!');
  
  // Subscribe to topics
  mqttClient.subscribe(MQTT_TOPIC_ALERT, (err) => {
    if (!err) {
      console.log('✓ Subscribed to:', MQTT_TOPIC_ALERT);
    } else {
      console.error('✗ Failed to subscribe to alerts:', err);
    }
  });
  
  mqttClient.subscribe(MQTT_TOPIC_STATUS, (err) => {
    if (!err) {
      console.log('✓ Subscribed to:', MQTT_TOPIC_STATUS);
    } else {
      console.error('✗ Failed to subscribe to status:', err);
    }
  });
});

// MQTT message event
mqttClient.on('message', (topic, message) => {
  const payload = message.toString();
  console.log(`\n📨 MQTT Message: ${topic} → ${payload}`);
  
  if (topic === MQTT_TOPIC_ALERT) {
    handleAlertMessage(payload);
  } else if (topic === MQTT_TOPIC_STATUS) {
    handleStatusMessage(payload);
  }
});

// MQTT error event
mqttClient.on('error', (error) => {
  console.error('✗ MQTT Error:', error);
});

// Handle alert messages
function handleAlertMessage(alertType) {
  console.log('🚨 ALERT RECEIVED:', alertType);
  
  // Update current alert state
  currentAlert = {
    active: true,
    type: alertType,
    timestamp: new Date().toISOString(),
    device: 'ESP1' // Alerts come from trigger unit
  };
  
  // Log alert to file
  const savedAlert = addAlert(alertType, 'ESP1');
  console.log('✓ Alert logged:', savedAlert.id);
  
  // Broadcast to all connected dashboard clients
  io.emit('newAlert', {
    type: alertType,
    timestamp: currentAlert.timestamp,
    device: 'ESP1',
    id: savedAlert.id
  });
  
  console.log('✓ Alert broadcasted to dashboard clients');
}

// Handle status messages (heartbeat)
function handleStatusMessage(statusMsg) {
  // Format: "ESP1:ALIVE" or "ESP2:ALIVE"
  const parts = statusMsg.split(':');
  if (parts.length !== 2) return;
  
  const deviceId = parts[0];
  const status = parts[1];
  
  if (deviceStatus[deviceId]) {
    const wasOffline = deviceStatus[deviceId].status === 'offline';
    
    deviceStatus[deviceId].status = 'online';
    deviceStatus[deviceId].lastSeen = Date.now();
    
    if (wasOffline) {
      console.log(`✓ ${deviceId} is now ONLINE`);
      broadcastDeviceStatus();
    }
  }
}

// ============================================================================
// DEVICE HEALTH MONITORING
// ============================================================================

// Check device health every 5 seconds
setInterval(() => {
  const now = Date.now();
  let statusChanged = false;
  
  for (const deviceId in deviceStatus) {
    const device = deviceStatus[deviceId];
    
    if (device.lastSeen && (now - device.lastSeen > DEVICE_OFFLINE_TIMEOUT)) {
      if (device.status === 'online') {
        device.status = 'offline';
        console.log(`⚠ ${deviceId} is now OFFLINE (no heartbeat for 30s)`);
        statusChanged = true;
      }
    }
  }
  
  if (statusChanged) {
    broadcastDeviceStatus();
  }
}, 5000);

// Broadcast device status to all clients
function broadcastDeviceStatus() {
  io.emit('deviceStatus', deviceStatus);
}

// ============================================================================
// AUTHENTICATION MIDDLEWARE
// ============================================================================

function requireAuth(req, res, next) {
  if (req.session && req.session.authenticated) {
    next();
  } else {
    res.status(401).json({ error: 'Unauthorized. Please login.' });
  }
}

// ============================================================================
// REST API ENDPOINTS
// ============================================================================

// Login endpoint
app.post('/api/login', (req, res) => {
  const { username, password } = req.body;
  
  console.log(`\n🔐 Login attempt: ${username}`);
  
  if (USERS[username] && USERS[username] === password) {
    req.session.authenticated = true;
    req.session.username = username;
    console.log(`✓ Login successful: ${username}`);
    res.json({ success: true, username: username });
  } else {
    console.log(`✗ Login failed: ${username}`);
    res.status(401).json({ success: false, error: 'Invalid credentials' });
  }
});

// Logout endpoint
app.post('/api/logout', (req, res) => {
  const username = req.session.username;
  req.session.destroy();
  console.log(`✓ Logout: ${username}`);
  res.json({ success: true });
});

// Check authentication status
app.get('/api/auth/check', (req, res) => {
  if (req.session && req.session.authenticated) {
    res.json({ authenticated: true, username: req.session.username });
  } else {
    res.json({ authenticated: false });
  }
});

// Get alert history
app.get('/api/alerts/history', requireAuth, (req, res) => {
  const alertsData = loadAlerts();
  res.json(alertsData.alerts);
});

// Get current alert
app.get('/api/alerts/current', requireAuth, (req, res) => {
  res.json(currentAlert);
});

// Acknowledge alert
app.post('/api/alerts/acknowledge', requireAuth, (req, res) => {
  const { alertId } = req.body;
  
  console.log(`\n✓ Acknowledging alert: ${alertId}`);
  
  const alertsData = loadAlerts();
  const alert = alertsData.alerts.find(a => a.id === alertId);
  
  if (alert) {
    alert.acknowledged = true;
    alert.acknowledgedAt = new Date().toISOString();
    saveAlerts(alertsData);
    
    // Broadcast to all clients
    io.emit('alertAcknowledged', {
      alertId: alertId,
      acknowledgedAt: alert.acknowledgedAt
    });
    
    res.json({ success: true, alert: alert });
  } else {
    res.status(404).json({ success: false, error: 'Alert not found' });
  }
});

// Reset alert (turn off relays)
app.post('/api/alerts/reset', requireAuth, (req, res) => {
  console.log('\n🔄 RESET command received from dashboard');
  
  // Publish reset command to MQTT
  mqttClient.publish(MQTT_TOPIC_RESET, 'RESET', (err) => {
    if (err) {
      console.error('✗ Failed to publish reset command:', err);
      res.status(500).json({ success: false, error: 'Failed to send reset command' });
    } else {
      console.log('✓ Reset command published to MQTT');
      
      // Clear current alert state
      currentAlert = {
        active: false,
        type: null,
        timestamp: null,
        device: null
      };
      
      // Broadcast to all clients
      io.emit('alertReset', {
        message: 'All alerts reset',
        timestamp: new Date().toISOString()
      });
      
      res.json({ success: true, message: 'Reset command sent' });
    }
  });
});

// Get device status
app.get('/api/devices/status', requireAuth, (req, res) => {
  res.json(deviceStatus);
});

// ============================================================================
// WEBSOCKET CONNECTION HANDLING
// ============================================================================

io.on('connection', (socket) => {
  console.log('\n✓ Dashboard client connected:', socket.id);
  
  // Send current state to newly connected client
  socket.emit('currentAlert', currentAlert);
  socket.emit('deviceStatus', deviceStatus);
  
  // Send recent alerts
  const alertsData = loadAlerts();
  socket.emit('alertHistory', alertsData.alerts.slice(0, 10)); // Last 10 alerts
  
  socket.on('disconnect', () => {
    console.log('✗ Dashboard client disconnected:', socket.id);
  });
});

// ============================================================================
// SERVE HTML PAGES
// ============================================================================

// Login page (public)
app.get('/login', (req, res) => {
  res.sendFile(path.join(__dirname, 'public', 'login.html'));
});

// Dashboard page (requires authentication)
app.get('/dashboard', requireAuth, (req, res) => {
  res.sendFile(path.join(__dirname, 'public', 'dashboard.html'));
});

// Root redirect
app.get('/', (req, res) => {
  if (req.session && req.session.authenticated) {
    res.redirect('/dashboard');
  } else {
    res.redirect('/login');
  }
});

// ============================================================================
// START SERVER
// ============================================================================

initializeAlertsFile();

server.listen(PORT, () => {
  console.log('\n╔════════════════════════════════════════════╗');
  console.log('║  IoT Emergency Alert System - Backend     ║');
  console.log('╚════════════════════════════════════════════╝');
  console.log(`✓ Server running on http://localhost:${PORT}`);
  console.log(`✓ Login page: http://localhost:${PORT}/login`);
  console.log(`✓ Dashboard: http://localhost:${PORT}/dashboard`);
  console.log('\nDefault credentials:');
  console.log('  Username: admin');
  console.log('  Password: admin123');
  console.log('\n════════════════════════════════════════════\n');
});

// ============================================================================
// GRACEFUL SHUTDOWN
// ============================================================================

process.on('SIGINT', () => {
  console.log('\n\n--- Shutting down gracefully ---');
  mqttClient.end();
  server.close(() => {
    console.log('✓ Server closed');
    process.exit(0);
  });
});

// ============================================================================
// END OF CODE
// ============================================================================
