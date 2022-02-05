#include <TheThingsNetwork.h>
#include <SoftwareSerial.h>

// Set your AppEUI and AppKey
const char *appEui = "70B3D57ED0036E7D";
const char *appKey = "213C0F71E3E08007BB829D3C3A94615C";

#define debugSerial Serial

// Replace REPLACE_ME with TTN_FP_EU868 or TTN_FP_US915
#define freqPlan TTN_FP_EU868

SoftwareSerial loraSerial(10, 11); // RX, TX
TheThingsNetwork ttn(loraSerial, debugSerial, freqPlan);

void setup()
{
  loraSerial.begin(9600);
  debugSerial.begin(57600);

  // Wait a maximum of 10s for Serial Monitor
  while (!debugSerial && millis() < 10000)
    ;

  // Set callback for incoming messages
  ttn.onMessage(message);

  debugSerial.println("-- STATUS");
  ttn.showStatus();

  debugSerial.println("-- JOIN");
  ttn.join(appEui, appKey);
}

void loop()
{
  debugSerial.println("-- LOOP");

  // Send single byte to poll for incoming messages
  ttn.poll();

  delay(10000);
}

void message(const uint8_t *payload, size_t size, port_t port)
{
  debugSerial.println("-- MESSAGE");
  debugSerial.print("Received " + String(size) + " bytes on port " + String(port) + ": ");
  String msg = "";
  for (int i = 0; i < size; i++)
  {
    char c = payload[i];
    msg += c;
  }
  debugSerial.println(msg);
}
