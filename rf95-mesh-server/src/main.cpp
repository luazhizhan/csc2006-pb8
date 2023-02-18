/**
 * RH_TEST_NETWORK 1 is enabled in RHRouter.h for testing mesh network
 * Uncomment for production
 */
#include <SPI.h>
#include <RH_RF95.h>
#include <RHMesh.h>
#include <RHRouter.h>

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

// Singleton instance of the radio driver
RH_RF95 rf95(RFM95_CS, RFM95_INT);

// RadioHead Mesh Manager
RHMesh manager(rf95, SERVER_ADDRESS);

void setup()
{
  Serial.begin(9600);

  if (!manager.init())
  {
    Serial.println("init failed");
  }
  else
  {
    Serial.println("done");
  }
  Serial.println("LoRa radio init OK!");

  Serial.print("Set Freq to: ");
  Serial.println(RF95_FREQ);

  rf95.setFrequency(RF95_FREQ);
  rf95.setTxPower(13, false);
  rf95.setCADTimeout(500);
}

void loop()
{
  uint8_t data[] = "And hello back to you from server";
  uint8_t buf[RH_MESH_MAX_MESSAGE_LEN];

  uint8_t len = sizeof(buf);
  uint8_t from;
  if (manager.recvfromAck(buf, &len, &from))
  {
    Serial.print("got request from : 0x");
    Serial.print(from, HEX);
    Serial.print(": ");
    Serial.println((char *)buf);

    // Send a reply back to the originator client
    if (manager.sendtoWait(data, sizeof(data), from) != RH_ROUTER_ERROR_NONE)
    {
      Serial.println("sendtoWait failed");
    }
  }
}
