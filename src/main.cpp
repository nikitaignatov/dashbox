#include <Homie.h>

#include <MD_Parola.h>
#include <MD_MAX72xx.h>
#include <SPI.h>

#define HARDWARE_TYPE MD_MAX72XX::FC16_HW
#define MAX_DEVICES 4
#define CLK_PIN D5
#define DATA_PIN D7
#define CS_PIN D4

uint8_t scrollSpeed = 25;
textEffect_t scrollEffect = PA_SCROLL_LEFT;
textPosition_t scrollAlign = PA_LEFT;
uint16_t scrollPause = 2 * 1000;

#define BUF_SIZE 75
char curMessage[BUF_SIZE] = {"Dash Box"};
char newMessage[BUF_SIZE] = {"Connecting..."};
String Q = "Dash Box~Connecting...";
char separator = '~';
bool newMessageAvailable = true;

MD_Parola P = MD_Parola(HARDWARE_TYPE, DATA_PIN, CLK_PIN, CS_PIN, MAX_DEVICES);
HomieNode screenNode("screen", "display");

bool resetHandler(const HomieRange &range, const String &value)
{
  Homie.getLogger() << "Message received: " << value << endl;
  Homie.reset();
}

bool messageHandler(const HomieRange &range, const String &value)
{
  Homie.getLogger() << "Message received: " << value << endl;

  screenNode.setProperty("message").send(value);
  if (Q.length() > 0)
  {
    Q.concat('~');
  }
  Q.concat(value);
  newMessageAvailable = true;

  Homie.getLogger() << "Current Q: " << Q << endl;
  return true;
}

void setup()
{
  Homie_setBrand("dashbox");
  Homie.disableLedFeedback();

  Serial.begin(115200);
  Serial << endl
         << endl;

  P.begin();
  P.displayText(curMessage, scrollAlign, scrollSpeed, scrollPause, scrollEffect, scrollEffect);

  Homie_setFirmware("dashbox-message", "2018.12.14");

  screenNode.advertise("message").settable(messageHandler);
  screenNode.advertise("reset").settable(resetHandler);

  Homie.setup();
}

void showStatus(String message)
{
    message.toCharArray(curMessage, message.length() + 1);
    // strcpy(curMessage, newMessage);
    P.displayReset();
}

void loop()
{
  Homie.loop();
  if (P.displayAnimate())
  { 
    int l = max(0, Q.indexOf(separator));
    int index = l == 0 ? Q.length() : l + 1;
    String message = Q.substring(0, index);
    message.replace("~","");
    Q = Q.substring(index);
    showStatus(message);
  }
}