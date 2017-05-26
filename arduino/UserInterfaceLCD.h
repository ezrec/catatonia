/*
 * Copyright (C) 2017, Jason S. McMullan <jason.mcmullan@gmail.com>
 * All rights reserved.
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version 2
 * of the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 */

#pragma once

#include <LiquidCrystal_I2C.h>

#include "Feeder.h"
#include "Button.h"

#define LCD_I2C     0x20
#define LCD_COLS    20
#define LCD_ROWS    4

#define BUTTON_TIME 1000

typedef enum {
    UI_MENU_DARK,
    UI_MENU_STATUS,
    UI_MENU_SETTINGS,
    UI_MENU_SETTINGS_INTERVAL,
    UI_MENU_SETTINGS_COUNT,
} ui_menu_t;

class UserInterfaceLCD
{
    Feeder *_feeder;
    Button *_button;
    LiquidCrystal_I2C _lcd;

    unsigned long _menu_timeout_ms;
    ui_menu_t _menu;
    bool _menu_first;

    unsigned long _last_elapsed;
    bool _button_active;
    int _button_pick;

    unsigned _count_update;
    unsigned long _interval_update;

private:
    ui_menu_t menu_dark_(bool first = false)
    {
        if (first)
        {
            _lcd.clear();
            _lcd.home();
            _lcd.noBacklight();
            _button_active = false;
        }

        // Wait for a button press
        if (_button->state())
        {
            _button_active = true;
        }
        else if (_button_active)
        {
            _button_active = false;
            return UI_MENU_STATUS;
        }

        return UI_MENU_DARK;
    }

    void print_time_(unsigned long ms)
    {
        if (ms < 60000)
        {
            _lcd.print(ms / 1000);
            _lcd.print("s");
        }
        else if (ms < 60UL * 60000)
        {
            _lcd.print(ms / 60000);
            _lcd.print("m ");
            _lcd.print((ms / 1000) % 60);
            _lcd.print("s ");
        }
        else if (ms < 24UL * 60UL * 60000)
        {
            _lcd.print(ms / (60UL * 60000));
            _lcd.print("h ");
            _lcd.print((ms / 60000) % 60UL);
            _lcd.print("m");
        }
        else
        {
            _lcd.print(ms / (24UL * 60UL * 60000));
            _lcd.print("d ");
            _lcd.print((ms / (60UL * 60000)) % 24);
            _lcd.print("h");
        }
    }

    ui_menu_t menu_status_(bool first = false)
    {
        if (first)
        {
            _lcd.clear();
            _lcd.home();
            _lcd.setCursor(0, 0);
            _lcd.print("Catatonia");
            _lcd.setCursor(0, 1);
            _lcd.print("Last feed:");
            _lcd.setCursor(0, 2);
            _lcd.print("Next feed:");
            _lcd.setCursor(0, 3);
            _lcd.print(":Feed");
            _lcd.setCursor(10, 3);
            _lcd.print(":Settings");
            _lcd.backlight();

            _last_elapsed = _feeder->elapsed();
        }

        unsigned long elapsed = _feeder->elapsed();
        unsigned long interval = _feeder->interval();

        unsigned long since_last_update = (unsigned long)((long)elapsed - (long)_last_elapsed);

        if (since_last_update > 1000)
        {
            // Need to update
            _lcd.noCursor();
            _lcd.noBlink();

            // Show last feed time
            _lcd.setCursor(10, 1);
            _lcd.print("        ");
            _lcd.setCursor(10, 1);
            if (elapsed < 5000)
            {
                _lcd.print("NOW");
            }
            else
            {
                print_time_(elapsed);
            }

            // Show time till next feed
            _lcd.setCursor(10, 2);
            _lcd.print("        ");
            _lcd.setCursor(10, 2);
            print_time_(interval - elapsed);

            if (since_last_update > 5000)
            {
                _lcd.setCursor(1, 3);
                _lcd.print("Feed");
                _last_elapsed = elapsed;
            }

            // Show portions
            _lcd.setCursor(6, 3);
            _lcd.print(_feeder->count());
        }

        if (_button->state())
        {
            if (!_button_active)
            {
                _lcd.blink();
                _lcd.cursor();
                _button_active = true;
            }
            _button_pick = (_button->elapsed() / BUTTON_TIME) % 2;

            if (_button_pick == 0)
            {
                _lcd.setCursor(0, 3);
            }
            else
            {
                _lcd.setCursor(10, 3);
            }

            _lcd.cursor();
            _lcd.blink();
        }
        else if (_button_active)
        {
            _button_active = false;

            if (_button_pick == 0)
            {
                _feeder->dispense();
                _lcd.setCursor(1, 3);
                _lcd.print("FEED");
            }
            else
            {
                return UI_MENU_SETTINGS;
            }
        }

        return UI_MENU_STATUS;
    }

    ui_menu_t menu_settings_(bool first = false)
    {
        if (first)
        {
            _lcd.clear();
            _lcd.noBlink();
            _lcd.noCursor();

            _interval_update = _feeder->interval();
            _count_update = _feeder->count();

            _lcd.setCursor(0, 0);
            _lcd.print("Feeding Settings");

            _lcd.setCursor(0, 1);
            _lcd.print("Interval: ");
            print_time_(_interval_update);

            _lcd.setCursor(0, 2);
            _lcd.print("Portions: ");
            _lcd.print(_count_update);

            _lcd.setCursor(0, 3);
            _lcd.print(":Cancel   :Save");
            _lcd.backlight();
        }

        if (_button->state())
        {
            if (!_button_active)
            {
                _lcd.blink();
                _lcd.cursor();
                _button_active = true;
            }

            _button_pick = (_button->elapsed() / BUTTON_TIME) % 4;

            if (_button_pick == 2)
            {
                _lcd.setCursor(10, 1);
            }
            else if (_button_pick == 3)
            {
                _lcd.setCursor(10, 2);
            }
            else if (_button_pick == 0)
            {
                _lcd.setCursor(0, 3);
            }
            else if (_button_pick == 1)
            {
                _lcd.setCursor(10, 3);
            }
        }
        else if (_button_active)
        {
            _button_active = false;

            if (_button_pick == 0)
            {
                // Cancel
                return UI_MENU_STATUS;
            }
            else if (_button_pick == 1)
            {
                // Save
                _feeder->count(_count_update);
                _feeder->interval(_interval_update);
                _feeder->save();
                return UI_MENU_STATUS;
            }
            else if (_button_pick == 2)
            {
                // Update interval
                return UI_MENU_SETTINGS_INTERVAL;
            }
            else if (_button_pick == 3)
            {
                // Update count
                return UI_MENU_SETTINGS_COUNT;
            }
        }

        return UI_MENU_SETTINGS;
    }

    ui_menu_t menu_settings_interval_(bool first)
    {
        const unsigned long button_map[] = {
            15UL * 60000,       // 15 minutes
            30UL * 60000,       // 30 minutes
            1UL * 60 * 60000,   // 1 hour
            2UL * 60 * 60000,   // 2 hours
            3UL * 60 * 60000,   // 3 hours
            4UL * 60 * 60000,   // 4 hours
            6UL * 60 * 60000,   // 6 hours
            8UL * 60 * 60000,   // 8 hours
            12UL * 60 * 60000,  // 12 hours
            18UL * 60 * 60000,  // 18 hours
            24UL * 60 * 60000,  // 24 hours
        };

        if (_button->state())
        {
            if (!_button_active)
            {
                _button_active = true;
            }

            _button_pick = (_button->elapsed() / BUTTON_TIME) % (sizeof(button_map)/sizeof(button_map[0]));

            _lcd.setCursor(10, 1);
            _lcd.print("          ");
            _lcd.setCursor(10, 1);
            print_time_(button_map[_button_pick]);
        }
        else if (_button_active)
        {
            _button_active = false;
            _interval_update =  button_map[_button_pick];

            _lcd.setCursor(0, 1);
            _lcd.print("Interval: ");
            return UI_MENU_SETTINGS;
        }
        else
        {
            _lcd.setCursor(0, 1);
            _lcd.print("INTERVAL:");
        }

        return UI_MENU_SETTINGS_INTERVAL;
    }

    ui_menu_t menu_settings_count_(bool first)
    {
        if (_button->state())
        {
            if (!_button_active)
            {
                _button_active = true;
            }

            _button_pick = (_button->elapsed() / BUTTON_TIME) % 10;

            _lcd.setCursor(10, 2);
            _lcd.print(_button_pick + 1);
            _lcd.print(" ");
        }
        else if (_button_active)
        {
            _button_active = false;
            _count_update = _button_pick + 1;

            _lcd.setCursor(0, 2);
            _lcd.print("Portions: ");
            return UI_MENU_SETTINGS;
        }
        else
        {
            _lcd.setCursor(0, 2);
            _lcd.print("PORTIONS:");
        }

        return UI_MENU_SETTINGS_COUNT;
    }

public:
    UserInterfaceLCD(Feeder *feeder, Button *button, unsigned long menu_timeout_ms = 10000)
        : _feeder(feeder), _button(button),
         _lcd(LCD_I2C, LCD_COLS, LCD_ROWS),
        _menu_timeout_ms(menu_timeout_ms),
         _menu(UI_MENU_DARK),
        _menu_first(true)
    {}

    void begin(void) {
        _lcd.init();
    }

    void update(void)
    {
        ui_menu_t menu;
        switch (_menu)
        {
        case UI_MENU_DARK:
            menu = menu_dark_(_menu_first);
            break;
        case UI_MENU_STATUS:
            menu = menu_status_(_menu_first);
            break;
        case UI_MENU_SETTINGS:
            menu = menu_settings_(_menu_first);
            break;
        case UI_MENU_SETTINGS_INTERVAL:
            menu = menu_settings_interval_(_menu_first);
            break;
        case UI_MENU_SETTINGS_COUNT:
            menu = menu_settings_count_(_menu_first);
            break;
        }

        _menu_first = false;

        if (!_button->state() && _button->elapsed() > _menu_timeout_ms)
        {
            menu = UI_MENU_DARK;
        }

        if (menu != _menu)
        {
            if (_menu <= UI_MENU_SETTINGS)
            {
                _menu_first = true;
            }
            _menu = menu;
        }
    }
};

/* vim: set shiftwidth=4 expandtab:  */
