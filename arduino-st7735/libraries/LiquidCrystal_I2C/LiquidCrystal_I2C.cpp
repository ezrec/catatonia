#include <LiquidCrystal_I2C.h>

static Adafruit_ST7735 tft_ = Adafruit_ST7735(TFT_CS, TFT_DC, TFT_RST);

LiquidCrystal_I2C::LiquidCrystal_I2C(uint8_t i2c_bus, int cols, int rows) :
    gfx_(&tft_, cols*6, rows*2*8, 20, 40)
{
    background_ = 0xffff;
    text_ = 0;
}

void LiquidCrystal_I2C::init()
{
    tft_.initR(INITR_BLACKTAB);   // initialize a ST7735S chip, black tab
    tft_.setRotation(3);
    tft_.fillScreen(0xffff);
}
