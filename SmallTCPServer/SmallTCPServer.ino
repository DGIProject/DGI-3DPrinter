/*
 * UIPEthernet EchoServer example.
 *
 * UIPEthernet is a TCP/IP stack that can be used with a enc28j60 based
 * Ethernet-shield.
 *
 * UIPEthernet uses the fine uIP stack by Adam Dunkels <adam@sics.se>
 *
 *      -----------------
 *
 * This Hello World example sets up a server at 192.168.1.6 on port 1000.
 * Telnet here to access the service.  The uIP stack will also respond to
 * pings to test if you have successfully established a TCP connection to
 * the Arduino.
 *
 * This example was based upon uIP hello-world by Adam Dunkels <adam@sics.se>
 * Ported to the Arduino IDE by Adam Nielsen <malvineous@shikadi.net>
 * Adaption to Enc28J60 by Norbert Truchsess <norbert.truchsess@t-online.de>
 */

#include <UIPEthernet.h>

EthernetServer server = EthernetServer(1000);
unsigned long sendDate=0, currentDate=0;
bool clientConnected=false;

void setup()
{
  Serial.begin(9600);

  uint8_t mac[6] = {0x00, 0x01, 0x02, 0x03, 0x04, 0x05};
  Ethernet.begin(mac);

  Serial.print("localIP: ");
  Serial.println(Ethernet.localIP());
  Serial.print("subnetMask: ");
  Serial.println(Ethernet.subnetMask());
  Serial.print("gatewayIP: ");
  Serial.println(Ethernet.gatewayIP());
  Serial.print("dnsServerIP: ");
  Serial.println(Ethernet.dnsServerIP());


  server.begin();
}

void loop()
{
  size_t size;
  
  EthernetClient client;
  if (client = server.available())
  {
    while ((size = client.available()) > 0)
    {
      sendDate = millis();
      clientConnected=true;
      uint8_t* msg = (uint8_t*)malloc(size);
      size = client.read(msg, size);
      Serial.write(msg, size);
      free(msg);
    }
    client.println("DATA from Server!");
    client.stop();
  }

 /* currentDate = millis();
  Serial.print(currentDate);
  Serial.print(" ");
  Serial.print(sendDate);
  Serial.print(" diff ");
  Serial.println(currentDate-sendDate);
  
  if (currentDate-sendDate>=10000 && clientConnected)
  {
    Serial.println("\n\n\n stoping Transfer no client data since 10s \n\n\n");
    client.stop();
    clientConnected=false;
  }*/
}
