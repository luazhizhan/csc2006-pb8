# csc2006-pb8

CSC2006 PB8 Building Management IoT project. We implemented LoRa with mesh topologies. 

We have two mesh algorithms:

1. RadioHead Mesh (RHMesh)
2. Time-Slicing, Nodes Sequencing and Synchronization (TNSS)

## System Architecture Diagram

![image](https://user-images.githubusercontent.com/16435270/230287296-d016f2e0-0f83-4846-af0b-697dfc9d37bc.png)

## Get Started (RHMesh)

### Installation

1. [PlatformIO with VSCode](https://platformio.org/install/ide?install=vscode)

### Usage

1. `client1` - Smoke sensor with buzzer and led
2. `client2` - IR sensor with buzzer and led
3. `client3` - LED
4. `server` - Gateway

Open `RHMesh/client1`, `RHMesh/client2`, `RHMesh/client3`, `RHMesh/server` folder on VSCode to build and install on Maker UNO.  

Connect cable from Maker UNO to your workstation. Ensure the correct project and COM port is selected. Click on top right `tick icon` to build and upload or bottom left to `right arrow icon` to build and upload.

## Get Started (TNSS)

There are 3 source code files under TNSS. 

1. `node.ino` - Node with LoRa-shield and their respective sensors. To run on different LoRa-shield, change the `myNode` to any value from 1-4. 
2. `gateway.ino` Gateway with LoRa-shield 
3. `master.py` - Pi 4B (MQTT Client).

### Installation

1. [Thingsboard](https://demo.thingsboard.io/)
2. [Arduino](https://www.arduino.cc/en/software)
3. [Pi 4B](https://www.raspberrypi.com/software/)

## Dashboard

Create a new devices to connect Pi 4B to the thingsboard. Click on the create device to get the `access token`. This `access token` is use to establish a connection to thingsboard for sending and receiving of data from Pi 4B. 

Create a new dashboard and add the telemtery and attribute data to the dashboard.

Issue: Pi 4B was uable to receive any downlink data from thingsboard after using mutlithread to create the connection and subscribing to the RPC topic.

### Upload to Maker UNO

Ensure you have installed Arduino IDE.
Connect cable from Maker UNO to your workstation. Ensure the correct file and COM port is selected. Click on top left `arrow button` to build and upload.

### Upload to Pi 4B

Connect Pi 4B using putty to your workstation. Ensure the correct file and path. Send the file from your local folder by opening cmd and using scp command to send to Pi 4B. Run the file by 'pyhton3 master.py'

## References

### RadioHead

1. https://edgecollective.io/notes/simplemesh/
2. https://www.hackster.io/davidefa/esp32-lora-mesh-1-the-basics-3a0920#toc-mesh-testing-2
3. https://github.com/royyandzakiy/LoRa-RHMesh
4. https://github.com/hallard/RadioHead
5. https://www.airspayce.com/mikem/arduino/RadioHead/classRHMesh.html

### TNSS

1. https://www.instructables.com/Simple-Arduino-Wireless-Mesh/
