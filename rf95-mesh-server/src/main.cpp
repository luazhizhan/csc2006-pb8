/**
 * RH_TEST_NETWORK 1 is enabled in RHRouter.h for testing mesh network
 * Uncomment for production
 */
#include <SPI.h>
#include <RH_RF95.h>
#include <RHMesh.h>
#include <RHRouter.h>
#include <AESLib.h>

uint8_t key[] = {0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15};

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
RHMesh manager(rf95, SERVER_ADDRESS);
uint8_t buf[RH_MESH_MAX_MESSAGE_LEN];

void setup()
{
  Serial.begin(9600);
  if (!manager.init())
  {
    Serial.println(F("Manager init failed"));
    while (1)
      ;
  }
  Serial.println(F("LoRa server init OK!"));
  Serial.print(F("Set Freq to: "));
  Serial.println(RF95_FREQ);

  rf95.setFrequency(RF95_FREQ);
  rf95.setTxPower(20, false);

  // long-range transmission, relatively low data rate, low power consumption
  // 125 kHz bw, 2048 chips/symbol, CR 4/5
  rf95.setModemConfig(RH_RF95::Bw125Cr45Sf2048);

  // increase timeout as data rate is low
  manager.setTimeout(5000);
}

void loop()
{
  if (manager.available())
  {
    uint8_t data[] = "ACK3456789012345";

    uint8_t len = sizeof(buf);
    uint8_t from;
    if (manager.recvfromAck(buf, &len, &from))
    {
      Serial.print(F("got request from : client"));
      Serial.print(from, HEX);
      Serial.print(F(": "));
      aes128_dec_single(key, buf);
      Serial.print(F("Decrypted: "));
      Serial.print((char *)buf);
      Serial.println();
      aes128_enc_single(key, data);
      // Send a reply back to the originator client
      if (manager.sendtoWait(data, sizeof(data), from) != RH_ROUTER_ERROR_NONE)
      {
        Serial.println(F("sendtoWait failed"));
      }
    }
    else
    {
      Serial.println(F("recvfromAck failed"));
    }
  }
}
