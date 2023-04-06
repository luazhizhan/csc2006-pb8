# Trasnfer File From Pi to Pc
# scp pi@[IP]:~/IOT/master.py [File source dir]
# Trasnfer File From Pc to Pi
# scp [File source dir] pi@[IP]:~/IOT/master.py

# import library
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
  try:
    BUS.write_i2c_block_data(ADDRESS, 0x00, data_bytes)
  except OSError as e:
        print("Remote I/O error occurred. Restarting app...")
        restart_program()

# I2C receive data from slave
def receive_data(length):
  print("Receive data")
  try:
    data = BUS.read_i2c_block_data(ADDRESS, 0x00, length)
    data_str = ''.join([chr(byte) for byte in data])
    cleaned_str = ""
    for char in data_str:
      if char != 'ÿ':
          cleaned_str += char
    return cleaned_str[1:]
  except OSError as e:
    print("I2C communication error occurred")
    restart_program()
  
# Connect to ThingsBoard using default MQTT port and 60 seconds keepalive interval 
def tb_configuration():
  print("Connecting to ThingsBoard")
  client = mqtt.Client()
  client.username_pw_set(ACCESS_TOKEN)
  client.connect(THINGSBOARD_HOST, 1883, 60)

  return client

# Subscribe to RPC topic
def connect_to_tb():
  client = tb_configuration()

  # Subscribe to RPC topic
  result, _ = client.subscribe('v1/devices/me/rpc/request/+')
  if result != mqtt.MQTT_ERR_SUCCESS:
      print(f"Failed to subscribe to RPC topic: {result}")
      return

  client.subscribe('v1/devices/me/rpc/request/+')

  # Set the callback function for RPC requests
  client.message_callback_add('v1/devices/me/rpc/request/+', on_rpc_request)

  # Start the loop for message reception
  client.loop_start()
  
  print("Connected to ThingsBoard and subscribed to RPC topic")

# Send telemetry data
def send_telemetry_data(data):
   time.sleep(2)
   print("Sending telemetry to ThingsBoard")
   client = tb_configuration()
   client.publish(TELEMETRY_TOPIC, json.dumps(data), 1)
   client.loop()
   client.disconnect()

# Define callback function for RPC requests
def on_rpc_request(client, userdata, message):
    print('onRPC')
    payload = json.loads(message.payload.decode())
    print('Received setLed RPC with state:', payload)
    if 'method' in payload and payload['method'] == 'setLed':
        led_state = payload['params']['ledState']
        print('Received setLed RPC with state:', led_state)

# Define a function to send attributes to ThingsBoard
def send_attributes(data):
    time.sleep(2)
    print("Sending attributes to ThingsBoard")
    client = tb_configuration()
    client.publish(ATTRIBUTES_TOPIC, json.dumps(data), 1, True)
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

# Start the thingsboard thread
def connect_thread():
    t = threading.Thread(target=connect_to_tb)
    t.daemon = True
    t.start()

def main():
    #Start the watchdog thread
    t = threading.Thread(target=watchdog_thread)
    t.start()

    # Connect to ThingsBoard in a separate thread
    connect_thread()
    
    # Initialize variables
    smoke = 'None'
    smokeLED = False
    motion = 'None'
    motionLED = False


    while 1: 
      # Send a message to get somke data and wait for response from the slave
      send_data('Smoke')
      received_data = receive_data(32)
      print('Received: ', received_data)
      if smoke != received_data:
        smoke = received_data
        # Send telemetry data to ThingsBoard
        data = {'Smoke': smoke}
        send_telemetry_data(data)
      time.sleep(2)

      # Send a message to get somke LED and wait for response from the slave
      send_data('Smoke LED')
      received_data = receive_data(32)
      print('Received: ', received_data)
      if smokeLED != received_data:
        smokeLED = received_data
        # Send attributes data to ThingsBoard
        data = {'Smoke LED': smokeLED}
        send_attributes(data)
      time.sleep(2)

      # Send a message to get motion data and wait for response from the slave
      send_data('Motion')
      received_data = receive_data(32)
      print('Received: ', received_data)
      if motion != received_data:
        motion = received_data
        # Send telemetry data to ThingsBoard
        data = {'Motion': motion}
        send_telemetry_data(data)
      time.sleep(2)

      # Send a message to get motion LED and wait for response from the slave
      send_data('Motion LED')
      received_data = receive_data(32)
      print('Received: ', received_data)
      if motionLED != received_data:
        motionLED = received_data
        # Send attributes data to ThingsBoard
        data = {'Motion LED': motionLED}
        send_attributes(data)
      time.sleep(2)

if __name__ == '__main__':
    main()
