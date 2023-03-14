/**
 * Uncomment RH_TEST_NETWORK 1 in RHRouter.h for testing mesh network
 * Comment for production
 */
#include <SPI.h>
#include <RH_RF95.h>
#include <RHMesh.h>
#include <RHRouter.h>
#include <AESLib.h>

uint8_t key[] = {0,1,2,3,4,5,6,7,8,9,10,11,12,13,14,15};

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

uint32_t msgCount = 0;
void setup()
{
  Serial.begin(9600);

  if (!manager.init())
  {
    Serial.println(F("Manager init failed"));
    while (1)
      ;
  }
  Serial.println(F("LoRa client2 init OK!"));

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
  if (millis() % 5000 == 0)
  {
    uint8_t data[32] = "C2!";
    Serial.println(F("Sending to gateway"));
    msgCount++;
    // // Get current Unix time
    // unixTime = time(NULL);
    // // Create a buffer to store the formatted time string

    // // Format the Unix time as a string
    // sprintf(timeStr, "t:%lu", unixTime);
    // strftime(timeStr, sizeof(timeStr), "%c", gmtime(&unixTime));
    // Serial.print("Unix time: ");
    // Serial.println(timeStr);
    // // Append the time string to the data array
    //strcat((char*)data, msgCount);

    
    //sprintf((char*)data, "%d", msgCount);

    char msgCountStr[11];  // buffer to store the formatted value of msgCount
    itoa(msgCount, msgCountStr, 10);  // convert msgCount to a string with base 10
    strcat((char*)data, msgCountStr);

    int padding_bytes = 16 - (strlen((char*)data));
    Serial.print("padding bytes: ");
    Serial.println(padding_bytes);
    // Append the padding bytes
    for (int i = 0; i < padding_bytes; i++) {
        data[strlen((char*)data) + i] = '\0';
    }
    Serial.print("data: ");
    Serial.println((char*)data);
    aes128_enc_single(key, data);
    Serial.println((char *)data);
    if (manager.sendtoWait(data, sizeof(data), SERVER_ADDRESS) == RH_ROUTER_ERROR_NONE)
    {
      Serial.println(F("Sent to next hop"));
    }
    else
    {
      Serial.println(F("sendtoWait failed"));
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
      Serial.print(F("got message from : client"));
      Serial.print(from, HEX);
      Serial.print(F(": "));
      aes128_dec_single(key, buf);
      len = sizeof(buf);
      Serial.println((char *)buf);
    }
  }
}
