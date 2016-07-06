#define ETHERNET_PORT 3012

#ifdef NETWORK_ENABLED

#include <Ethernet.h>

// Enter a MAC address for your controller below.
// Newer Ethernet shields have a MAC address printed on a sticker on the shield
byte mac[] = {
  0xDE, 0xAD, 0xBE, 0xEF, 0xFE, 0xED
};

extern Stream& HomeClient;

EthernetServer server(ETHERNET_PORT);

void printIPAddress() {
  Serial.print(F("IP address: "));
  Ethernet.localIP().printTo(Serial);
  Serial.println();

  Serial.print(F("Subnet: "));
  Ethernet.subnetMask().printTo(Serial);
  Serial.println();

  Serial.print(F("Gateway: "));
  Ethernet.gatewayIP().printTo(Serial);
  Serial.println();

  Serial.print(F("DNS: "));
  Ethernet.dnsServerIP().printTo(Serial);
  Serial.println();
}

void updateDhcp() {
  switch (Ethernet.maintain()) {
    case 1:
      //renew failed
      Serial.println(F("Ethernet error: renew failed"));
      break;

    case 2:
      //renew success
      Serial.println(F("Ethernet renewed"));
      printIPAddress();
      break;

    case 3:
      //rebind fail
      Serial.println(F("Ethernet error: rebind failed"));

    case 4:
      //rebind success
      Serial.println(F("Ethernet rebind success"));
      printIPAddress();
      break;

    default:
      //nothing happend
      break;
  }
}

void handleClients() {
  EthernetClient client = server.available();

  if(client) {
    Serial.println(F("New ethernet client"));
    client.print(F("ready\r\n"));

    HomeClient = client;
  }
}

void network_setup() {
  if (Ethernet.begin(mac) == 0) {
    Serial.println(F("Ethernet error: failed to configure Ethernet using DHCP"));
  }
  printIPAddress();

  server.begin();
}

void network_loop() {
  updateDhcp();

  handleClients();
}

#endif
