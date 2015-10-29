#include <UIPEthernet.h>

byte mac[] = { 0xDE, 0xAD, 0xBE, 0xEF, 0xFE, 0xED };

const int timeout = 15000;
const char IFS = ' ';

EthernetServer server(80);

unsigned long cTime = 0, sTime = 0;
enum Modes { BASE, FTP };
enum States { NOTHING, FTP_WRITE };
enum Errors { OK, CMD_NFOUND, SYNTAX_ER };
long int fileSize = 0, receivedSize = 0;

Modes currentMode = BASE;
States currentState = NOTHING;
Errors currentEr = OK;

String getArg(String cmd, byte pos = 0)
{
  byte i = 0, curPos = 0;
  String arg = "";
  bool first = false, second = false;

  if (pos == 0)
    return cmd.substring(0, cmd.indexOf(IFS));
  //Serial.print("continue ");

  for (i = 0; i < cmd.length(); i++)
  {
    if (cmd.charAt(i) == IFS)
    {
      //Serial.print(" IFS at " + String(i) + " ");
      second = (first) ? true : false;
      //Serial.print(" Sec  " + String(((first) ? "true" : "false")) + " ");
      first = true;
      if (first && second)
      {
        curPos++;
        //Serial.print(" arg "+String(curPos)+" ");
        first = true;
        second = false;
        if (curPos == pos)
          return arg;
        arg = "";
      }

    }
    else
    {
      if (first)
        arg += cmd.charAt(i);
    }

  }
  return arg;

}

void setup() {

  Serial.begin(115200);

  Serial.println("DHCP config");
  // start the Ethernet connection and the server:
  // DHCP get config  and print it

  Ethernet.begin(mac, 48);

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
          errorDisplay(client);
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
        bc("no message since " + String(timeout / 1000) + "s: closing connection", client);
        break;
      }
    }

    // close the connection:
    bc("Closing connection", client);
    client.stop();
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
      if (getArg(cmd) == "ftp" )
      {
        currentMode = FTP;
        bc("------------------------", client);
        bc("         FTP MODE       ", client);
        bc("------------------------", client);
        bc("FTP > ", client);
      }
      else
      {
        currentEr = CMD_NFOUND;
      }
  }
}



byte getArgNumber(String cmd)
{
  byte i = 0;
  byte argNumber = 0;
  for (i = 0; i < cmd.length(); i++)
  {
    if (cmd.charAt(i) == IFS)
      argNumber++;
  }
  return argNumber;
}

void parseFTPCmd(String cmd, EthernetClient client)
{

  switch (currentState)
  {
    case FTP_WRITE :
      if (quitCmd(cmd))
      {
        currentState = NOTHING;
        fileSize = 0; receivedSize = 0;
        bc("Back to FTP prompt", client);
        break;
      }

      receivedSize++;

      client.print(((double)receivedSize / fileSize) * 100);
      client.println("%");
      break;
    default :
      bc("FTP > ", client);
      if (quitCmd(getArg(cmd)))
      {
        currentMode = BASE;
        bc("Back to prompt", client);

      }
      else if ( getArg(cmd) == "write")
      {
        if (getArg(cmd, 1) != "")
        {
          currentState = FTP_WRITE;
          fileSize = getArg(cmd, 2).toInt();
          bc("New File, with name " + String(getArg(cmd, 1)), client);
          bc("Waiting " + String(getArg(cmd, 2)) + " For Data ...", client);
        }
        else
        {
          currentEr = SYNTAX_ER;
        }
      }
      else
      {
        currentEr = CMD_NFOUND;
      }

  }
}

void bc(String mes, EthernetClient client )
{
  Serial.println(mes);
  client.println(mes);
}

bool quitCmd(String cmd)
{
  return (cmd == "Quit");
}

void errorDisplay(EthernetClient client )
{
  switch (currentEr)
  {
    case CMD_NFOUND:
      bc("Command not found", client);
      break;
    case SYNTAX_ER:
      bc("Syntax error", client);
      break;
    case OK :
      break;
    default :
      bc("Unexpected Error", client);
  }
  currentEr = OK;
}

