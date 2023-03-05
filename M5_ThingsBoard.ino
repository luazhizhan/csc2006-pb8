/*
*******************************************************************************
* Copyright (c) 2021 by M5Stack
*                  Equipped with M5StickC-Plus sample source code
*                             M5StickC-Plus
* Visit for more information: https://docs.m5stack.com/en/core/m5stickc_plus
*
* Describe: MQTT
* Date: 2021/11/5
*******************************************************************************
*/
#include "M5StickCPlus.h"
#include <WiFi.h>
#include <ThingsBoard.h>

// ThingsBoard access token
#define TOKEN "0mzmxUlNY67ACEKU1jtG"

// Configure the name and password of the connected wifi
#define SSID "Galaxy S22 Ultra70B5"
#define PASSWORD "qryv5364"

// Define SLAVE_ADDR
#define SLAVE_ADDR 60

// Define size of return response
#define RESPONSE_SIZE 5

// ThingsBoard server instance.
char thingsboardServer[] = "demo.thingsboard.io";

WiFiClient espClient;
ThingsBoard tb(espClient);

// the Wifi radio's status
int status = WL_IDLE_STATUS;

unsigned long lastMsg = 0;
#define MSG_BUFFER_SIZE (50)
char msg[MSG_BUFFER_SIZE];
int value = 0;


void setupWifi();
void reConnect();

void setup() {
  // Setup serial monitor
  Serial.begin(115200);
  M5.begin();
  Wire.begin(0, 26);
  M5.Lcd.setRotation(3);
  setupWifi();
  Serial.println("Setup completed!");
}

void loop() {

  // Write data to Slave
  // Serial.println("Writing to slave");
  // Wire.beginTransmission(SLAVE_ADDR);
  // Wire.write("Test");
  // Wire.endTransmission();

  // Read data from Slave
  Wire.requestFrom(SLAVE_ADDR, RESPONSE_SIZE);

  byte response[RESPONSE_SIZE];
  int i = 0;
  while (Wire.available() && i < RESPONSE_SIZE) {
    response[i] = Wire.read();
    Serial.print(response[i], HEX);
    Serial.print(" ");
    i++;
  }

  Serial.println();
  Serial.println("Bytes received: " + String(i));
  if (i == RESPONSE_SIZE) {
    String message = "";
    for (int j = 0; j < RESPONSE_SIZE; j++) {
      message += (char)response[j];
    }
    Serial.println("Response: " + message);
  } else {
    Serial.println("Response size error.");
  }

  // String response = "";
  // while (Wire.available()) {
  //   char r = (char)Wire.read();
  //   response += r;
  //   Serial.print("Received byte: ");
  //   Serial.println(r);
  //   Serial.print("Current response string: ");
  //   Serial.println(response);
  // }
  // Serial.println("Response: " + response);
  // Send to the thingsboard
  if (response.length() > 0) {
    if (!tb.connected()) {
      reConnect();
    }

    unsigned long now = millis();  // Obtain the host startup duration.
    if (now - lastMsg > 2000) {
      lastMsg = now;
      tb.sendTelemetryFloat("Temp", 1.2);
      ++value;
    }
    tb.loop();
  }
  delay(5000);
}

void setupWifi() {
  delay(10);
  M5.Lcd.setCursor(0, 0);
  M5.Lcd.printf("Connecting to %s", SSID);
  WiFi.mode(WIFI_STA);         // Set the mode to WiFi station mode.
  WiFi.begin(SSID, PASSWORD);  // Start Wifi connection.

  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    M5.Lcd.print(".");
  }
  M5.Lcd.printf("\nSuccess\n");
}


void reConnect() {
  while (!tb.connected()) {
    M5.Lcd.print("Attempting connection to ThingsBoard...");

    if (tb.connect(thingsboardServer, TOKEN)) {
      M5.Lcd.setCursor(0, 0);
      M5.Lcd.print("ThingsBoard connected");
    } else {
      M5.Lcd.setCursor(0, 0);
      M5.Lcd.print("Fail: try again in 5 seconds");
      delay(5000);
    }
  }
}