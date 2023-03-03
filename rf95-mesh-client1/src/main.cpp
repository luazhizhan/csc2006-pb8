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

// MQ2 Gas/Smoke Sensor
#define MQ2pin 8

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
RHMesh manager(rf95, CLIENT1_ADDRESS);

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

  Serial.println("MQ2 warming up!");
  delay(5000); // allow the MQ2 to warm up
}

void loop()
{
  // 0 - smoke detected
  // 1 - no smoke detected
  int sensorValue = digitalRead(MQ2pin); // read digital output pin

  // Send a message to a rf95-mesh-server
  char data[11];
  sprintf(data, "client1 - %d", sensorValue);
  uint8_t dataBytes[11];
  memcpy(dataBytes, data, sizeof(data));
  Serial.println("Sending to gateway");

  // Send a message to a rf95-mesh-server
  // A route to the destination will be automatically discovered.
  if (manager.sendtoWait(dataBytes, sizeof(dataBytes), SERVER_ADDRESS) == RH_ROUTER_ERROR_NONE)
  {
    Serial.println("Sent to next hop");
  }
  else
  {
    Serial.println("sendtoWait failed");
  }

  // Radio needs to stay always in receive mode ( to process/forward messages )
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

  delay(5000);
}
