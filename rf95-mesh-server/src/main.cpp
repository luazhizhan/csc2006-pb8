/**
 * RH_TEST_NETWORK 1 is enabled in RHRouter.h for testing mesh network
 * Uncomment for production
 */
#include <SPI.h>
#include <RH_RF95.h>
#include <RHMesh.h>
#include <RHRouter.h>
#include <Wire.h>

#define RFM95_CS 10
#define RFM95_RST 9
#define RFM95_INT 2

// Change to 434.0 or other frequency, must match RX's freq!
#define RF95_FREQ 454.5

// Mesh network ID
// In this small artifical network of 3 nodes
#define CLIENT1_ADDRESS 1
#define CLIENT2_ADDRESS 2
#define SERVER_ADDRESS 3

// Define slave address
#define SLAVE_ADDRESS 0x3C

// Define size of return response from slave 
#define RESPONSE_SIZE 5

RH_RF95 rf95(RFM95_CS, RFM95_INT);
RHMesh manager(rf95, SERVER_ADDRESS);

// Define I2C receive and request event
void requestEvent();
void receiveEvent(int);


String smoke_detector = "Q23  ";

void setup()
{
  Serial.begin(115200);
  if (!manager.init())
  {
    Serial.println("Manager init failed");
    while (1)
      ;
  }
  Serial.println("LoRa radio init OK!");
  Serial.print("Set Freq to: ");
  Serial.println(RF95_FREQ);

  rf95.setFrequency(RF95_FREQ);
  rf95.setTxPower(13, false);

  Wire.begin(SLAVE_ADDRESS);
  Wire.onRequest(requestEvent);
  Wire.onReceive(receiveEvent);
}

void loop()
{
  if (manager.available())
  {
    uint8_t data[] = "ACK";
    uint8_t buf[RH_MESH_MAX_MESSAGE_LEN];
    uint8_t len = sizeof(buf);
    uint8_t from;
    if (manager.recvfromAck(buf, &len, &from))
    {
      Serial.print("got request from : 0x");
      Serial.print(from, HEX);
      Serial.print(": ");
      for (uint8_t i = 0; i < len; i++)
      {
        smoke_detector += (char)buf[i];
        Serial.print((char)buf[i]);
      }
      Serial.println();

      // Send a reply back to the originator client
      if (manager.sendtoWait(data, sizeof(data), from) != RH_ROUTER_ERROR_NONE)
      {
        Serial.println("sendtoWait failed");
      }
    }
    else
    {
      Serial.println("recvfromAck failed");
    }
  }
}

void requestEvent()
{
  Serial.println("Request Event");
  Serial.println(smoke_detector);
  Wire.write(smoke_detector.c_str());
  //smoke_detector = "";
}

void receiveEvent(int howMany)
{
  while (Wire.available())
  {
    char c = Wire.read();
    Serial.print(c);
  }
}