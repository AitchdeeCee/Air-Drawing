
#include <Elegoo_GFX.h>
#include <Elegoo_TFTLCD.h>
#include <TouchScreen.h>
#include <EEPROM.h>  


#define YP A2
#define XM A3 
#define YM 8  
#define XP 9 


#define TS_MINX 150
#define TS_MINY 120
#define TS_MAXX 920
#define TS_MAXY 940
#define LCD_RESET A4 

TouchScreen ts = TouchScreen(XP, YP, XM, YM, 300);

#define LCD_CS A3
#define LCD_CD A2
#define LCD_WR A1
#define LCD_RD A0

#define	BLACK   0x0000
#define	BLUE    0x001F
#define	RED     0xF800
#define	GREEN   0x07E0
#define CYAN    0x07FF
#define MAGENTA 0xF81F
#define YELLOW  0xFFE0
#define WHITE   0xFFFF


Elegoo_TFTLCD tft(LCD_CS, LCD_CD, LCD_WR, LCD_RD, LCD_RESET);

void setup(void) {
  tft.reset();
  tft.begin(0x8357);
  tft.setRotation(2); 
  tft.fillScreen(BLACK);
  pinMode(13, OUTPUT);
}

#define MINPRESSURE 10
#define MAXPRESSURE 1000

void loop()
{
  digitalWrite(13, HIGH);
  //get data
  TSPoint p = ts.getPoint();
  
  digitalWrite(13, LOW);
  
  pinMode(XM, OUTPUT);
  pinMode(YP, OUTPUT);
  if (p.z > MINPRESSURE && p.z < MAXPRESSURE) {
    p.y = map(p.y, TS_MINX, TS_MAXX, tft.width(), 0);
    p.x = map(p.x, TS_MINY, TS_MAXY, tft.height(), 0);
    tft.fillCircle(p.y, p.x, 4, WHITE);
  }
}
