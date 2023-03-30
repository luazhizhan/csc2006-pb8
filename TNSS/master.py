# Trasnfer File From Pi to Pc
# scp pi@192.168.159.127:~/IOT/master.py C:\Users\Kenny\Documents\SIT-UOG\Year_2\Tri_2\CSC2006-Internet_of_Things_Protocols_and_Networks\Projects
# Trasnfer File From Pc to Pi
# scp C:\Users\Kenny\Documents\SIT-UOG\Year_2\Tri_2\CSC2006-Internet_of_Things_Protocols_and_Networks\Projects\master.py pi@192.168.159.127:~/IOT/master.py

import smbus
import time
import json
import paho.mqtt.client as mqtt
import threading
import sys
import os
from watchdog.observers import Observer
from watchdog.events import FileSystemEventHandler

# Define ThingsBoard credentials and topic
THINGSBOARD_HOST = 'demo.thingsboard.io'
ACCESS_TOKEN = '2k8OqE9ToNbTNkmeHvVK'
TELEMETRY_TOPIC = 'v1/devices/me/telemetry'
ATTRIBUTES_TOPIC = 'v1/devices/me/attributes'

# Define I2C slave address and bus number
ADDRESS = 0x3A
BUS = smbus.SMBus(1)

# I2C send data to slave
def send_data(data):
  print("Sending data")
  data_bytes = [ord(c) for c in data]
  # print("data size: ", len(data))
  # print("data_bytes: " ,data_bytes)
  try:
    BUS.write_i2c_block_data(ADDRESS, 0x00, data_bytes)
  except OSError as e:
        print("Remote I/O error occurred. Restarting app...")
        restart_program()

# I2C receive data from slave
def receive_data(length):
  print("Receive data")
  data = BUS.read_i2c_block_data(ADDRESS, 0x00, length)
  data_str = ''.join([chr(byte) for byte in data])
  cleaned_str = ""
  for char in data_str:
      if char != 'ÿ':
          cleaned_str += char
  return cleaned_str[1:]

# Connect to ThingsBoard using default MQTT port and 60 seconds keepalive interval
def connect_to_tb():
  print("Connecting to ThingsBoard")
  client = mqtt.Client()
  client.username_pw_set(ACCESS_TOKEN)
  client.connect(THINGSBOARD_HOST, 1883, 60)

  # # Subscribe to attributes topic
  # client.subscribe(ATTRIBUTES_TOPIC)

  # # Start the MQTT client loop in a separate thread
  # client.loop_start()

  return client

# Send telemetry data
def send_telemetry_data(data):
   print("Sending to ThingsBoard")
   client = connect_to_tb()
   client.publish(TELEMETRY_TOPIC, json.dumps(data), 1)
   client.loop()
   client.disconnect()

# Define event handler for watchdog
class MyHandler(FileSystemEventHandler):
    def __init__(self, restart_function):
        self.restart_function = restart_function

    def on_modified(self, event):
        print("File system modified")
        self.restart_function()

# Create a separate thread for watchdog
def watchdog_thread():
    event_handler = MyHandler(restart_program)
    observer = Observer()
    observer.schedule(event_handler, ".", recursive=True)
    observer.start()
    observer.join()

# Restart the program
def restart_program():
    print("Restarting program")
    python = sys.executable
    os.execl(python, python, *sys.argv)


def main():
    #Start the watchdog thread
    t = threading.Thread(target=watchdog_thread)
    t.start()

    # Initialize variables
    smoke = 'None'
    motion = 'None'

    while 1: 
      # Send a message to turn on the LED and wait for response from the slave
      send_data('Smoke')
      received_data = receive_data(32)
      print('Received: ', received_data)
      if smoke != received_data:
        smoke = received_data
        # Send telemetry data to ThingsBoard
        data = {'Smoke': smoke}
        send_telemetry_data(data)
      time.sleep(3)

      send_data('Motion')
      received_data = receive_data(32)
      print('Received: ', received_data)
      if motion != received_data:
        motion = received_data
        # Send telemetry data to ThingsBoard
        data = {'Motion': motion}
        send_telemetry_data(data)

      time.sleep(3)

if __name__ == '__main__':
    main()
