#include <WiFi101.h>
#include <ArduinoECCX08.h>
#include "Adafruit_MCP9808.h"

WiFiSSLClient client;
Adafruit_MCP9808 tempsensor = Adafruit_MCP9808();

void setup() {
  // Buad Rate of BPM
  Serial1.begin(38400);
  // To get Serial Number from the chip
  ECCX08.begin();
  // To get ambient temperature
  tempsensor.begin(0x18);
  // PIN_PD7 controls power to BPM
  pinMode(19, OUTPUT);
  digitalWrite(19, 1);
  // chipSelect, irq, reset, enable = PIN_PA7,PIN_PF2,PIN_PA1,PIN_PF3
  WiFi.setPins(7, 22, 1, 23);
  // Connect to WiFi
  while (WiFi.begin("", "") != WL_CONNECTED) yield();
  // AVR-IoT WG is too fast for BPM
  delay(4000);
  // Turn BPM On
  for (int i = 0; i < 4; i++) {
    digitalWrite(19, !digitalRead(19));
    delay(100);
  }
  int TempNum = 0, SYS = 0, DIA = 0, PPM = 0;
  char data[20] = "";
  while (TempNum != 16) {
    while (Serial1.available()) {
      char c = Serial1.read();
      if ((c >= 'a' && c <= 'z')) sprintf(data, "%s%c", data, c);
      else if (c == '\n') {
        if (strstr(data, "endtest") != NULL) {
          while (TempNum != 16) {
            if (Serial1.available()) {
              c = Serial1.read();
              switch (c) {
                case '0' ... '9':
                  TempNum += (c - 48) * (TempNum > 15 ? 1 : 16);
                  break;
                case 'A' ... 'F':
                  TempNum += (c - 55) * (TempNum > 15 ? 1 : 16);
                  break;
                case '\r':
                  SYS == 0 ? SYS = TempNum : DIA == 0 ? (DIA = TempNum) : (PPM = TempNum);
                  TempNum = 0;
                  break;
                default:
                  break;
              }
            }
          }
        } else sprintf(data, "%s", "");
      }
    }
  }
  // JSON to be sent
  String Values = "{\"Device\":\"" + ECCX08.serialNumber() + "\",\"SYS\":" + SYS + ",\"DIA\":" + DIA + ",\"PPM\":" + PPM + ",\"Temp\":" + tempsensor.readTempF() + "}";
  const char* host = "script.google.com";
  client.connect(host, 443);
  // POST to Web App
  client.println("POST /macros/s/<>/exec HTTP/1.1");
  client.println("Host: " + (String)host + "\r\nContent-Type: application/json\r\nContent-Length: " + (String)Values.length() + "\r\n\r\n" + Values);
}

void loop() {}
