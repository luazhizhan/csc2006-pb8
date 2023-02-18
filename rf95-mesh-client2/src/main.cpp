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

#define TXINTERVAL 1000 // delay between successive transmissions
unsigned long nextTxTime;

// Singleton instance of the radio driver
RH_RF95 rf95(RFM95_CS, RFM95_INT);

// RadioHead Mesh Manager
RHMesh manager(rf95, CLIENT2_ADDRESS);

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
  nextTxTime = millis();
}

void loop()
{

  uint8_t data[] = "CLIENT2!";
  uint8_t buf[RH_MESH_MAX_MESSAGE_LEN];

  if (millis() > nextTxTime)
  {
    nextTxTime += TXINTERVAL;
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
