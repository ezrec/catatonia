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

#include "Adafruit_Joystick.h"

class Button
{
    bool _state;
    unsigned long _state_ms;
    Adafruit_Joystick _joy;

public:
    Button(int pin) : _joy(pin) {}

    void begin(void)
    { }

    void update(void)
    {
       bool now = _joy.read() != AFJOYSTICK_NONE;

        if (now != _state)
        {
            _state_ms = millis();
            _state = now;
        }
    }

    bool state(void)
    {
        return _state;
    }

    unsigned long elapsed(void)
    {
        return (unsigned long)((long)millis() - (long)_state_ms);
    }
};

/* vim: set shiftwidth=4 expandtab:  */
