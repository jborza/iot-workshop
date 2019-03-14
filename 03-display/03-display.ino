//Wiring for ESP32 - 
//SDA - 21
//SCL - 22

#include <Arduino.h>
#include <U8x8lib.h>

#ifdef U8X8_HAVE_HW_SPI
#include <SPI.h>
#endif

// Please UNCOMMENT one of the contructor lines below
// The complete list is available here: https://github.com/olikraus/u8g2/wiki/u8x8setupcpp
// Please update the pin numbers according to your setup. Use U8X8_PIN_NONE if the reset pin is not connected

U8X8_SSD1306_128X64_NONAME_HW_I2C u8x8(/* reset=*/ U8X8_PIN_NONE);

void setup(void)
{
  u8x8.begin();
  u8x8.setPowerSave(0);
}

void loop(void)
{
  u8x8.setFont(u8x8_font_chroma48medium8_r);
  u8x8.drawString(0, 0, "Hello World!");
  u8x8.drawString(0, 1, "0123456789012345");
  u8x8.drawString(0, 2, "QWERTYUIOPASDFGH");
  u8x8.drawString(0, 3, "!@#$%^&*()_+[]\\");
  u8x8.drawString(0, 4, ":-) :-( :-| :-D");
  u8x8.drawString(0, 5, "abcdefghijklmnop");
  u8x8.drawString(0, 6, "Hello World 6!");
//  u8x8.refreshDisplay();		// only required for SSD1606/7
  delay(2000);
}
