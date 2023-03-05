/**
 * Uncomment RH_TEST_NETWORK 1 in RHRouter.h for testing mesh network
 * Comment for production
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

RH_RF95 rf95(RFM95_CS, RFM95_INT);
RHMesh manager(rf95, CLIENT2_ADDRESS);

void setup()
{
  Serial.begin(9600);

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
}

void loop()
{
  if (millis() % 5000 == 0)
  {
    uint8_t data[] = "CLIENT2!";
    Serial.println("Sending to gateway");

    // Send a message to a rf95-mesh-server
    // A route to the destination will be automatically discovered.
    if (manager.sendtoWait(data, sizeof(data), SERVER_ADDRESS) == RH_ROUTER_ERROR_NONE)
    {
      Serial.println("Sent to next hop");
    }
    else
    {
      Serial.println("sendtoWait failed");
    }
  }

  // Radio needs to stay always in receive mode ( to process/forward messages )
  if (manager.available())
  {
    uint8_t buf[RH_MESH_MAX_MESSAGE_LEN];
    uint8_t len = sizeof(buf);
    uint8_t from;
    if (manager.recvfromAck(buf, &len, &from))
    {
      Serial.print("got message from : 0x");
      Serial.print(from, HEX);
      Serial.print(": ");
      Serial.println((char *)buf);
    }
  }
}
