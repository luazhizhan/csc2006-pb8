# csc2006-pb8

CSC2006 PB8 Building Management IoT project. Uses LoRa with mesh topology. Powered by [RadioHead Packet Radio library](https://github.com/hallard/RadioHead)

There are 3 folders. `rf95-mesh-client1` and `rf95-mesh-client2` are just nodes with LoRa-shield and their respective sensors. `rf95-mesh-server` is the Gateway with LoRa-shield, sensor, and M5StickC-Plus (MQTT Client).

## Get Started

### Installation

1. [PlatformIO with VSCode](https://platformio.org/install/ide?install=vscode)

### Upload to Maker UNO

Ensure you have installed PlatformIO with VSCode and enabled the extensions before proceeding on.

Open `csc2006-pb8-radiohead-lora-mesh.code-workspace` file with VSCode as **Workspace**.

Connect cable from Maker UNO to your workstation. Ensure the correct project and COM port is selected. Click on top right `tick icon` to build and upload or bottom left to `right arrow icon` to build and upload.

## References

### RadioHead

1. https://edgecollective.io/notes/simplemesh/
2. https://www.hackster.io/davidefa/esp32-lora-mesh-1-the-basics-3a0920#toc-mesh-testing-2
3. https://github.com/royyandzakiy/LoRa-RHMesh
4. https://github.com/hallard/RadioHead
5. https://www.airspayce.com/mikem/arduino/RadioHead/classRHMesh.html

### Maker UNO

1. https://cdn.cytron.io/makeruno/Arduino_Learning_Guide_for_Beginner_Using_Maker_UNO.pdf


# TSNS

There are 3 folders under TNSS. `Nodes1` is just nodes with LoRa-shield and their respective sensors. To run on different LoRa-shield, change the `myNode` to any value from 1-4. `Gateway` is the Gateway with LoRa-shield and `master` is for Pi 4B (MQTT Client).

### Installation

1. [Thingsboard](https://demo.thingsboard.io/)
2. [Arduino](https://www.arduino.cc/en/software)
3. [Pi 4B](https://www.raspberrypi.com/software/)

## Dash board

Create a new devices to connect Pi 4B to the thingsboard. Click on the create device to get the `access token`. This `access token` is use to establish a connection to thingsboard for sending and receiving of data from Pi 4B. 

Create a new dashboard and add the telemtery and attribute data to the dashboard.

Issue: Pi 4B was uable to receive any downlink data from thingsboard after using mutlithread to create the connection and subscribing to the RPC topic.

### Upload to Maker UNO

Ensure you have installed Arduino IDE.
Connect cable from Maker UNO to your workstation. Ensure the correct file and COM port is selected. Click on top left `arrow button` to build and upload.

### Upload to Pi 4B

Connect Pi 4B using putty to your workstation. Ensure the correct file and path. Send the file from your local folder by opening cmd and using scp command to send to Pi 4B. Run the file by 'pyhton3 master.py'

