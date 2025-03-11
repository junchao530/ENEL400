import serial
import time

# Function to initialize Bluetooth connection
def bluetooth_init():
    comPort = "COM10"  # Replace with the correct port for your system
    try:
        return serial.Serial(port=comPort, baudrate=9600, timeout=1)
    except Exception as e:
        print(f"Error initializing Bluetooth: {e}")
        return None

# Main execution
if __name__ == "__main__":
    # Initialize Bluetooth connection
    bluetooth_connection = bluetooth_init()
    
    if bluetooth_connection is not None:
        print("Bluetooth connected. Waiting for data...")
        
        while True:
            # Read data from the ESP32
            if bluetooth_connection.in_waiting > 0:
                data = bluetooth_connection.readline().decode('utf-8').strip()
                print(f"Received: {data}")
                
                # Send a response back to the ESP32
                response = "Hello from Python!"
                bluetooth_connection.write(response.encode('utf-8'))
                print(f"Sent: {response}")
            
            # Add a small delay to avoid excessive CPU usage
            time.sleep(1)
    else:
        print("Failed to initialize Bluetooth connection.")