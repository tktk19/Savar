#include <PololuLedStrip.h>

// Create an ledStrip object and specify the pin it will use.
PololuLedStrip<13> ledStrip;

// Create a buffer for holding the colors (3 bytes per color).
#define LED_COUNT 1
rgb_color colors[LED_COUNT];

// Color to display
uint32_t color;
uint32_t v = 0;
uint32_t h;

// recieve by serial
char serial_recv;

// mode
bool isRoot = false;

// AlertCount
unsigned long cntTimer =    0;
unsigned long tmpTime  =    0;
int blinkTime          =  500; // msec
#define COUNT_INFORM      500; // count
#define COUNT_ALERT      1000; // count

void setup()
{
  Serial.begin(9600);
}

// Converts a color from HSV to RGB.
// h is hue, as a number between 0 and 360.
// s is the saturation, as a number between 0 and 255.
// v is the value, as a number between 0 and 255.
rgb_color hsvToRgb(uint16_t h, uint8_t s, uint8_t v)
{
    uint8_t f = (h % 60) * 255 / 60;
    uint8_t p = (255 - s) * (uint16_t)v / 255;
    uint8_t q = (255 - f * (uint16_t)s / 255) * (uint16_t)v / 255;
    uint8_t t = (255 - (255 - f) * (uint16_t)s / 255) * (uint16_t)v / 255;
    uint8_t r = 0, g = 0, b = 0;
    switch((h / 60) % 6){
        case 0: r = v; g = t; b = p; break;
        case 1: r = q; g = v; b = p; break;
        case 2: r = p; g = v; b = t; break;
        case 3: r = p; g = q; b = v; break;
        case 4: r = t; g = p; b = v; break;
        case 5: r = v; g = p; b = q; break;
    }
    return (rgb_color){r, g, b};
}

void loop()
{
  while(Serial.available() > 0) {
    serial_recv = Serial.read(); // シリアル通信を受信
    switch(serial_recv){
    case 'r':
      // mode RED
      isRoot = true;
      v = 0;
      break;
    case 'g':
      // mode GREEN
      isRoot = false;
      v = 0;
      break;
    case 'a':
      // mode ALERT
      cntTimer = COUNT_ALERT;
      color = 0; // RED
      break;
    case 'i':
      // mode INFORMATION
      cntTimer = COUNT_INFORM;
      color = 39; // YELLOW
      break;
    }
    Serial.println(serial_recv);
  }
  
  // Update the colors.  
  uint16_t time = millis() >> 2;
  byte x = (time >> 2) - (0 << 3);
  
  if (cntTimer > 0) {
    // Alert or Information受信時
    cntTimer--;
    if (millis() - tmpTime > blinkTime) {
      tmpTime = millis();
      if (v == 0) {
        v = 255;
      } else {
        v = 0;
      }
    }
    colors[0] = hsvToRgb(color, 255, v);
  } else {
    v = (uint32_t)x * 200 / 256; // 明るさ 100が最大
    h = (isRoot ? 0 : 126); // 色彩 赤:0 黄色:126
    if (v > 100) {
      colors[0] = hsvToRgb(h, 255, 200 - v);
    } else {
      colors[0] = hsvToRgb(h, 255, v);
    }
  }
  
  // Write the colors to the LED strip.
  ledStrip.write(colors, LED_COUNT);

  delay(10);
}
