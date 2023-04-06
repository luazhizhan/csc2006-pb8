#include <SPI.h>
#include <RH_RF95.h>
#include <RHMesh.h>
#include <AESLib.h>

uint8_t key[] = {0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15};

#define RFM95_CS 10
#define RFM95_RST 9
#define RFM95_INT 2

// Change to 434.0 or other frequency, must match RX's freq!
#define RF95_FREQ 454.5

// Mesh network ID
#define CLIENT1_ADDRESS 1
#define CLIENT2_ADDRESS 2
#define CLIENT3_ADDRESS 3
#define SERVER_ADDRESS 4

RH_RF95 rf95(RFM95_CS, RFM95_INT);
RHMesh manager(rf95, CLIENT3_ADDRESS);

void setup()
{
  Serial.begin(9600);
  if (!manager.init())
  {
    Serial.println("Manager init failed");
    while (1)
      ;
  }

  Serial.println("LoRa client3 init OK!");

  Serial.print("Set Freq to: ");
  Serial.println(RF95_FREQ);

  rf95.setFrequency(RF95_FREQ);
  rf95.setTxPower(23, false);

  // long-range transmission, relatively low data rate, low power consumption
  // 125 kHz bw, 2048 chips/symbol, CR 4/5
  rf95.setModemConfig(RH_RF95::Bw125Cr45Sf2048);

  // increase timeout as data rate is low
  manager.setTimeout(5000);
}

void loop()
{

  // Radio needs to stay always in receive mode ( to process/forward messages )
  if (manager.available())
  {
    uint8_t buf[RH_MESH_MAX_MESSAGE_LEN];
    uint8_t len = sizeof(buf);
    uint8_t from;
    if (manager.recvfromAck(buf, &len, &from))
    {
      Serial.print(F("got message from : client"));
      Serial.print(from, HEX);
      Serial.print(F(": "));
      aes128_dec_single(key, buf);
      len = sizeof(buf);
      Serial.println((char *)buf);
    }
    else
    {
      Serial.println("recvfromAck failed");
    }
  }
}
