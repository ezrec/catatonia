//
// jason.mcmullan@gmail.com, 2017
//

#pragma once

#include "WindowGFX.h"
#include <Adafruit_ST7735.h>

#define TFT_CS  10
#define TFT_RST 9
#define TFT_DC  8

#define GPIO_PIN_BACKLIGHT 6

class LiquidCrystal_I2C : public Print
{
private:
    WindowGFX gfx_;
    uint16_t background_;
    uint16_t text_;
    bool cursor_visible_;
    bool cursor_blink_;

public:
    LiquidCrystal_I2C(uint8_t i2c_bus, int cols, int rows);

    // LiquidCrystal_I2C emulation
    void init();
    void clear() { gfx_.fillScreen(background_); }
    void home() { setCursor(0,0); gfx_.setTextColor(text_, background_); }
    void backlight() { if (1) analogWrite(GPIO_PIN_BACKLIGHT, 255); }
    void noBacklight() { if (1) analogWrite(GPIO_PIN_BACKLIGHT, 0); }
    void setCursor(int col, int row) { gfx_.setTextCursor(col, row*2); }
    void noCursor() { cursor_visible_ = false; }
    void cursor() { cursor_visible_ = true; }
    void noBlink() { cursor_blink_ = false; }
    void blink() { cursor_blink_ = true; }


    // Print functions
    virtual size_t write(uint8_t c) { return gfx_.write(c); }
};
