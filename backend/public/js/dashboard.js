/*
 * ============================================================================
 * Dashboard JavaScript - Real-Time Updates
 * ============================================================================
 */

// WebSocket connection
const socket = io();

// Current alert state
let currentAlertId = null;
let currentAlertActive = false;

// Audio notification (optional)
const alertSound = new Audio('data:audio/wav;base64,UklGRnoGAABXQVZFZm10IBAAAAABAAEAQB8AAEAfAAABAAgAZGF0YQoGAACBhYqFbF1fdJivrJBhNjVgodDbq2EcBj+a2/LDciUFLIHO8tiJNwgZaLvt559NEAxQp+PwtmMcBjiR1/LMeSwFJHfH8N2QQAoUXrTp66hVFApGn+DyvmwhBSuBzvLZiTYIGGS57OihUBELTKXh8bllHAU2jdXvzn0vBSh+zPDajzsKElyx6OyrWBUIQ5zd8sFuJAUuhM/z24k2Bhxqvu7mnE0QDFCn4/C2YxwGOJHX8sx5LAUkd8fw3ZBACg==');

// Initialize dashboard
document.addEventListener('DOMContentLoaded', () => {
    console.log('Dashboard initialized');
    checkAuth();
    loadAlertHistory();
    updateLastUpdateTime();
    setInterval(updateLastUpdateTime, 1000);
});

// Check authentication
async function checkAuth() {
    try {
        const response = await fetch('/api/auth/check');
        const data = await response.json();
        
        if (!data.authenticated) {
            window.location.href = '/login';
        } else {
            document.getElementById('username').textContent = data.username;
        }
    } catch (error) {
        console.error('Auth check error:', error);
    }
}

// Logout function
async function logout() {
    try {
        await fetch('/api/logout', { method: 'POST' });
        window.location.href = '/login';
    } catch (error) {
        console.error('Logout error:', error);
    }
}

// Load alert history
async function loadAlertHistory() {
    try {
        const response = await fetch('/api/alerts/history');
        const alerts = await response.json();
        displayAlertHistory(alerts);
    } catch (error) {
        console.error('Error loading alert history:', error);
    }
}

// Display alert history in table
function displayAlertHistory(alerts) {
    const tbody = document.getElementById('alertHistoryBody');
    
    if (alerts.length === 0) {
        tbody.innerHTML = '<tr><td colspan="4" class="text-center">No alerts yet</td></tr>';
        return;
    }
    
    tbody.innerHTML = alerts.slice(0, 20).map(alert => {
        const time = new Date(alert.timestamp).toLocaleString();
        const statusBadge = alert.acknowledged 
            ? '<span class="badge bg-success">Acknowledged</span>'
            : '<span class="badge bg-warning">Pending</span>';
        
        const typeBadge = getAlertTypeBadge(alert.type);
        
        return `
            <tr>
                <td>${time}</td>
                <td>${typeBadge}</td>
                <td>${alert.device}</td>
                <td>${statusBadge}</td>
            </tr>
        `;
    }).join('');
}

// Get alert type badge
function getAlertTypeBadge(type) {
    const badges = {
        'FIRE': '<span class="badge bg-danger">🔥 FIRE</span>',
        'MEDICAL': '<span class="badge bg-info">🏥 MEDICAL</span>',
        'SECURITY': '<span class="badge bg-warning">🔒 SECURITY</span>'
    };
    return badges[type] || `<span class="badge bg-secondary">${type}</span>`;
}

// WebSocket event handlers

// New alert received
socket.on('newAlert', (data) => {
    console.log('New alert received:', data);
    
    currentAlertId = data.id;
    currentAlertActive = true;
    
    // Update alert display
    displayCurrentAlert(data);
    
    // Play sound notification
    try {
        alertSound.play().catch(e => console.log('Audio play failed:', e));
    } catch (e) {
        console.log('Audio not supported');
    }
    
    // Show browser notification
    showNotification(data);
    
    // Reload alert history
    loadAlertHistory();
    
    // Enable control buttons
    document.getElementById('ackBtn').disabled = false;
    document.getElementById('resetBtn').disabled = false;
});

// Device status update
socket.on('deviceStatus', (devices) => {
    console.log('Device status update:', devices);
    updateDeviceStatus(devices);
});

// Alert acknowledged
socket.on('alertAcknowledged', (data) => {
    console.log('Alert acknowledged:', data);
    loadAlertHistory();
});

// Alert reset
socket.on('alertReset', (data) => {
    console.log('Alert reset:', data);
    clearCurrentAlert();
    loadAlertHistory();
});

// Current alert (on connect)
socket.on('currentAlert', (alert) => {
    if (alert.active) {
        currentAlertId = alert.id;
        currentAlertActive = true;
        displayCurrentAlert(alert);
        document.getElementById('ackBtn').disabled = false;
        document.getElementById('resetBtn').disabled = false;
    }
});

// Alert history (on connect)
socket.on('alertHistory', (alerts) => {
    displayAlertHistory(alerts);
});

// Display current alert
function displayCurrentAlert(alert) {
    const card = document.getElementById('currentAlertCard');
    const statusDiv = document.getElementById('alertStatus');
    
    // Update card style
    card.className = 'card mb-4 alert-active';
    card.querySelector('.card-header').className = 'card-header bg-danger text-white';
    card.querySelector('.card-header h5').textContent = '🚨 ACTIVE ALERT';
    
    // Update content
    const alertClass = `alert-${alert.type.toLowerCase()}`;
    const icons = {
        'FIRE': '🔥',
        'MEDICAL': '🏥',
        'SECURITY': '🔒'
    };
    
    statusDiv.innerHTML = `
        <h2 class="${alertClass}">${icons[alert.type]} ${alert.type} ALERT</h2>
        <p class="text-muted">Triggered at: ${new Date(alert.timestamp).toLocaleString()}</p>
        <p class="text-muted">Device: ${alert.device}</p>
        <div class="alert alert-danger mt-3">
            <strong>Action Required:</strong> Respond to emergency and reset system when resolved.
        </div>
    `;
}

// Clear current alert display
function clearCurrentAlert() {
    const card = document.getElementById('currentAlertCard');
    const statusDiv = document.getElementById('alertStatus');
    
    currentAlertActive = false;
    currentAlertId = null;
    
    // Reset card style
    card.className = 'card mb-4';
    card.querySelector('.card-header').className = 'card-header bg-success text-white';
    card.querySelector('.card-header h5').textContent = 'Current Alert Status';
    
    // Reset content
    statusDiv.innerHTML = `
        <h3 class="text-success">✓ No Active Alerts</h3>
        <p class="text-muted">System is operating normally</p>
    `;
    
    // Disable control buttons
    document.getElementById('ackBtn').disabled = true;
    document.getElementById('resetBtn').disabled = true;
}

// Update device status
function updateDeviceStatus(devices) {
    for (const deviceId in devices) {
        const device = devices[deviceId];
        const deviceDiv = document.getElementById(`device-${deviceId}`);
        
        if (deviceDiv) {
            const statusBadge = device.status === 'online'
                ? '<span class="badge bg-success">Online</span>'
                : '<span class="badge bg-secondary">Offline</span>';
            
            const lastSeen = device.lastSeen 
                ? new Date(device.lastSeen).toLocaleTimeString()
                : 'Never';
            
            deviceDiv.innerHTML = `
                <div class="d-flex justify-content-between align-items-center">
                    <div>
                        <strong>${deviceId}</strong><br>
                        <small>${device.name}</small><br>
                        <small class="text-muted">Last seen: ${lastSeen}</small>
                    </div>
                    ${statusBadge}
                </div>
            `;
        }
    }
}

// Acknowledge alert
async function acknowledgeAlert() {
    if (!currentAlertId) {
        alert('No active alert to acknowledge');
        return;
    }
    
    try {
        const response = await fetch('/api/alerts/acknowledge', {
            method: 'POST',
            headers: { 'Content-Type': 'application/json' },
            body: JSON.stringify({ alertId: currentAlertId })
        });
        
        const data = await response.json();
        
        if (data.success) {
            alert('Alert acknowledged successfully');
            loadAlertHistory();
        } else {
            alert('Failed to acknowledge alert');
        }
    } catch (error) {
        console.error('Error acknowledging alert:', error);
        alert('Error acknowledging alert');
    }
}

// Reset alert
async function resetAlert() {
    if (!confirm('Are you sure you want to reset the system? This will turn off all relays.')) {
        return;
    }
    
    try {
        const response = await fetch('/api/alerts/reset', {
            method: 'POST',
            headers: { 'Content-Type': 'application/json' }
        });
        
        const data = await response.json();
        
        if (data.success) {
            alert('System reset successfully');
        } else {
            alert('Failed to reset system');
        }
    } catch (error) {
        console.error('Error resetting system:', error);
        alert('Error resetting system');
    }
}

// Show browser notification
function showNotification(alert) {
    if ('Notification' in window && Notification.permission === 'granted') {
        new Notification('Emergency Alert!', {
            body: `${alert.type} alert triggered at ${new Date(alert.timestamp).toLocaleTimeString()}`,
            icon: '/favicon.ico',
            tag: 'emergency-alert'
        });
    } else if ('Notification' in window && Notification.permission !== 'denied') {
        Notification.requestPermission().then(permission => {
            if (permission === 'granted') {
                showNotification(alert);
            }
        });
    }
}

// Update last update time
function updateLastUpdateTime() {
    document.getElementById('lastUpdate').textContent = new Date().toLocaleTimeString();
}

// Connection status
socket.on('connect', () => {
    console.log('Connected to server');
});

socket.on('disconnect', () => {
    console.log('Disconnected from server');
});
