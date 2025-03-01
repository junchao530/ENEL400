import serial
import time
from datetime import datetime
import streamlit as st

def usb_init():
    comPort = "/dev/rfcomm0"
    try:
        return serial.Serial(port=comPort, baudrate=9600, timeout=0, parity=serial.PARITY_EVEN, stopbits=1)
    except Exception:
        return -1

def read_from_usb(connection):
    if connection != -1:
        try:
            data = connection.readline(1024)
            return data.decode('utf-8', errors='ignore') if data else None
        except serial.SerialException:
            return None
    return None

def parse_data_packet(packet):
    parts = packet.split(';')
    date_time = datetime.strptime(parts[0].split(":")[1].strip(), "%Y-%m-%d %H-%M-%S")
    flow = float(parts[1].split(":")[1].strip())
    temperature = float(parts[2].split(":")[1].strip())
    turbidity = float(parts[3].split(":")[1].strip())
    return date_time, flow, temperature, turbidity
