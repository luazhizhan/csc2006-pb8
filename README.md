# csc2006-pb8-radiohead-lora-mesh

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
