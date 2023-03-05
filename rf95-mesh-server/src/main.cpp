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

// Define Slave Address
#define SLAVE_ADDR 60

// Define size of return response
#define RESPONSE_SIZE 5

// Singleton instance of the radio driver
RH_RF95 rf95(RFM95_CS, RFM95_INT);

// RadioHead Mesh Manager
RHMesh manager(rf95, SERVER_ADDRESS);

// Define I2C functions
void requestEvent();
void receiveEvent();

String answer = "ABCDE";

void setup()
{
  Serial.begin(115200);

  Wire.begin(SLAVE_ADDR);
  Wire.onRequest(requestEvent);
  Wire.onReceive(receiveEvent);

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
    for (uint8_t i = 0; i < len; i++)
    {
      Serial.print((char)buf[i]);
    }
    Serial.println();

    // Send a reply back to the originator client
    if (manager.sendtoWait(data, sizeof(data), from) != RH_ROUTER_ERROR_NONE)
    {
      Serial.println("sendtoWait failed");
    }
  }
}

void requestEvent()
{
  byte response[RESPONSE_SIZE];

  // for(byte i=0; i<RESPONSE_SIZE; i++){
  //   response[i] = (byte)answer.charAt(i);
  //   Serial.println(response[i]);
  // }

  memset(response, 0, RESPONSE_SIZE); // clear the response buffer
  for (byte i = 0; i < RESPONSE_SIZE; i++)
  {
    response[i] = (byte)answer.charAt(i);
    Serial.println(response[i]);
  }

  Wire.write(response, sizeof(response));
  Serial.println("I2C request event");
}

void receiveEvent()
{
  String receiveInput = "";
  while (0 < Wire.available())
  {
    char c = Wire.read();
    receiveInput += c;
  }
  Serial.println(receiveInput);
}