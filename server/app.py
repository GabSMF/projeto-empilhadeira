import serial
import time
from flask import Flask, render_template, request, jsonify

app = Flask(__name__)

# --- CONFIGURATION ---
# Replace with your actual serial port (e.g., '/dev/ttyUSB0' for Linux/Mac, 'COM3' for Windows)
# You can find this in the Arduino IDE under Tools > Port
SERIAL_PORT = '/dev/tty.usbmodem14101' # Example default, user needs to change
BAUD_RATE = 115200

# Initialize Serial Connection
try:
    ser = serial.Serial(SERIAL_PORT, BAUD_RATE, timeout=1)
    time.sleep(2) # Wait for connection to stabilize
    print(f"Connected to {SERIAL_PORT} at {BAUD_RATE} baud.")
except serial.SerialException as e:
    ser = None
    print(f"Error connecting to serial port: {e}")
    print("Make sure the Arduino is connected and the correct port is specified in app.py")

def send_command(command):
    if ser and ser.is_open:
        try:
            ser.write(command.encode('utf-8'))
            return True
        except Exception as e:
            print(f"Error sending command: {e}")
            return False
    else:
        print(f"Serial not connected. Command '{command}' not sent.")
        return False

@app.route('/')
def index():
    return render_template('index.html')

@app.route('/move/<direction>', methods=['POST'])
def move(direction):
    cmd_map = {
        'forward': 'F',
        'backward': 'B',
        'left': 'L',
        'right': 'R',
        'stop': 'S'
    }
    
    if direction in cmd_map:
        cmd = cmd_map[direction]
        success = send_command(cmd)
        if success:
            return jsonify({'status': 'success', 'command': direction, 'code': cmd})
        else:
            return jsonify({'status': 'error', 'message': 'Serial connection failed'}), 500
    else:
        return jsonify({'status': 'error', 'message': 'Invalid direction'}), 400

if __name__ == '__main__':
    app.run(debug=True, host='0.0.0.0', port=5000)
