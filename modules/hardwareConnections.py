import serial
import time
from datetime import datetime
import streamlit as st
import pandas as pd
from modules.postgresDataLoader import insert_data

def usb_init():
    comPort = "/dev/rfcomm0"
    try:
        return serial.Serial(port=comPort, baudrate=9600, timeout=0, parity=serial.PARITY_EVEN, stopbits=1)
    except Exception:
        return -1

def read_from_bluetooth(connection):
    if connection != -1:
        try:
            data = connection.readline(1024)
            return data.decode('utf-8', errors='ignore') if data else None
        except serial.SerialException:
            return None
    return None

def parse_data_packet(packet):
    try:
        print(packet)
        parts = packet.split(';')
        
        # Ensure packet has enough parts
        # if len(parts) < 3:
        #     raise ValueError("Incomplete packet")

        # Extract each value based on the structure of the string
        date = parts[0].split(":")[1].strip()
        flow = float(parts[1].split(":")[1].strip())
        turbidity = float(parts[2].split(":")[1].strip())
        temperature = 0

        date_time = datetime.strptime(date, "%Y-%m-%d %H-%M-%S")
        return date_time, flow, temperature, turbidity

    except (IndexError, ValueError, AttributeError) as e:
        print(f"Error parsing packet: {e}")
        return None, None, None, None


def Real_Time():
    #st.title("Real-Time Data Visualization")
    columns = ['timestamp', 'flow_rate', 'temperature', 'turbidity']
    data = pd.DataFrame(columns=columns)
    chart_placeholder = st.empty()

    connection = usb_init()

    while True:
        raw_data = read_from_bluetooth(connection)
        if raw_data is None:  # Check if raw_data is None
            continue  # Skip the rest of the loop if no data is available
        
        date_time, flow_rate, temperature, turbidity = parse_data_packet(raw_data)

        new_row = {
            'timestamp': date_time,
            'flow_rate': flow_rate,
            'temperature': temperature,
            'turbidity': turbidity
        }
        data = pd.concat([data, pd.DataFrame([new_row])], ignore_index=True)
        if len(data) > 50:
            #insert_data(data[0]['timestamp'], data[0]['flow_rate'], data[0]['temperature'], data[0]['turbidity']) // works in theory!
            
            data = data.drop(data.index[0])


        # Convert timestamp column to index for plotting
        data['timestamp'] = pd.to_datetime(data['timestamp'])
        data.set_index('timestamp')
        with chart_placeholder:
            st.line_chart(data[["flow_rate", "temperature", "turbidity"]])
        
        #time.sleep(1)


