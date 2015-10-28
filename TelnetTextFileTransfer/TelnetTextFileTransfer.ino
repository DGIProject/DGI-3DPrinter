#include <UIPEthernet.h>

byte mac[] = { 0xDE, 0xAD, 0xBE, 0xEF, 0xFE, 0xED };

const int timeout = 15000;

EthernetServer server(80);

unsigned long cTime = 0, sTime = 0;
enum Modes{ BASE, FTP };
enum States { NOTHING, FTP_WRITE };
Modes currentMode = BASE;
States currentState = NOTHING;

void setup() { 
  
  Serial.begin(9600);

  // start the Ethernet connection and the server:
  // DHCP get config  and print it
  Ethernet.begin(mac, 53);

  Serial.print("localIP: ");
  Serial.println(Ethernet.localIP());
  Serial.print("subnetMask: ");
  Serial.println(Ethernet.subnetMask());
  Serial.print("gatewayIP: ");
  Serial.println(Ethernet.gatewayIP());
  Serial.print("dnsServerIP: ");
  Serial.println(Ethernet.dnsServerIP());

  server.begin();
  Serial.println("Server started");
}


void loop() {
  // listen for incoming clients
  EthernetClient client = server.available();
  if (client) {
    Serial.println("new client");
    sTime = millis();

    // a request ends with a blank line
    boolean currentLineIsBlank = true;
    String cmd = "";
    while (client.connected()) {
      cTime = millis();

      if (client.available()) {

        sTime = millis();
        cTime = millis() + 1;

        char c = client.read();
        if (c != '\n' && c != '\r') {
          cmd += c;
        }
      
        
        //Serial.write(c);
        client.print("");
        // if you've gotten to the end of the line (received a newline
        // character) and the line is blank, the http request has ended,
        // so you can send a reply
        if (c == '\n' && currentLineIsBlank) {
          break;
        }
        if (c == '\n') {
          //starting a new line
          Serial.println(cmd);
          parseCmd(cmd, client);
          cmd = "";

          currentLineIsBlank = true;
        }
        else if (c != '\r') {
          // you've gotten a character on the current line
          currentLineIsBlank = false;
        }

      }


      //Serial.print(cTime); Serial.print(" ") ;Serial.print(sTime);  Serial.print(" ") ;Serial.println(cTime-sTime);

      if (cTime - sTime >= timeout)
      {
        client.println("no message since 5s: closing connection");
        break;
      }
    }

    // close the connection:
    client.println("Closing connection");
    client.stop();
    Serial.println("client disconnected");
  }


}

void parseCmd(String cmd, EthernetClient client)
{
  switch (currentMode)
  {
    case FTP :
      parseFTPCmd(cmd, client);
      break;
    default :
      if (cmd == "ftp" )
      {
        currentMode = FTP;
        bc("------------------------", client);
        bc("         FTP MODE       ", client);
        bc("------------------------", client);
      }
  }


}

void parseFTPCmd(String cmd, EthernetClient client)
{
  switch (currentState)
  {
    case FTP_WRITE :
      break;
    default :
      if (cmd == "write")
      {
        currentState = FTP_WRITE;
        bc("New File, with name 'tttt'", client);
      }
  }
}

void bc(String mes, EthernetClient client )
{
  Serial.println(mes);
  client.println(mes);
}

