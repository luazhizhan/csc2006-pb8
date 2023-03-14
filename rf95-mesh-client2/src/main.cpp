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

// IR sensor
const int IN_D0 = 8; // digital input

void setup()
{
  Serial.begin(9600);

  if (!manager.init())
  {
    Serial.println("Manager init failed");
    while (1)
      ;
  }
  Serial.println("LoRa client2 init OK!");

  Serial.print("Set Freq to: ");
  Serial.println(RF95_FREQ);

  rf95.setFrequency(RF95_FREQ);
  rf95.setTxPower(20, false);

  // long-range transmission, relatively low data rate, low power consumption
  // 125 kHz bw, 2048 chips/symbol, CR 4/5
  rf95.setModemConfig(RH_RF95::Bw125Cr45Sf2048);

  // increase timeout as data rate is low
  manager.setTimeout(5000);

  pinMode(IN_D0, INPUT);
}

void loop()
{
  if (millis() % 5000 == 0)
  {
    // Reads the digital input from the IR distance sensor
    // 0 is close, 1 is far
    bool IR_D0 = digitalRead(IN_D0);

    char data[11];
    sprintf(data, "client2 - %d", IR_D0);
    uint8_t dataBytes[11];
    memcpy(dataBytes, data, sizeof(data));
    // serial print data 
    Serial.println(data);
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
