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

#include <EEPROM.h>

#include "Stepper.h"

#define FEEDER_MAGIC 0xfeedf00d

typedef struct
{
    unsigned long magic;
    unsigned long interval;
    unsigned count;
} feeder_eeprom_t;

class Feeder
{
	// Step queue
	StepQueue *_queue;

	// Dispensing cycle
	int _steps_reverse;
	int _steps_forward;

	// Interval to next feed in ms
	unsigned long _interval_ms;
	// Number of dispenses at the feed
	unsigned _dispense_count;

	// Time of last dispense
	unsigned long _dispense_time_ms;
private:
    void eeprom_get_(void *buff, size_t size)
    {
        uint8_t *cp = static_cast<uint8_t *>(buff);

        for (size_t i = 0; i < size; i++, cp++)
        {
            *cp = EEPROM.read(i);
        }
    }

    void eeprom_put_(const void *buff, size_t size)
    {
        const uint8_t *cp = static_cast<const uint8_t *>(buff);

        for (size_t i = 0; i < size; i++, cp++)
        {
            uint8_t tmp = EEPROM.read(i);
            if (tmp != *cp)
            {
                EEPROM.write(i, *cp);
            }
        }
    }

public:
	Feeder(StepQueue *queue, int reverse, int forward) : _queue(queue), _steps_reverse(reverse), _steps_forward(forward)
	{
	}

	void begin(void)
    {
	    feeder_eeprom_t eeprom;

        eeprom_get_(&eeprom, sizeof(eeprom));

        if (eeprom.magic == FEEDER_MAGIC)
        {
            interval(eeprom.interval);
            count(eeprom.count);

            // Dispense at 1/4 the interval, so that we don't starve hungry pets after a power loss
            _dispense_time_ms = (unsigned long)((long)millis() - (long)interval()/4);
        }
    }

    void save(void)
    {
        feeder_eeprom_t eeprom;

        eeprom.magic = FEEDER_MAGIC;
        eeprom.interval = interval();
        eeprom.count = count();

        eeprom_put_(&eeprom, sizeof(eeprom));
    }

	unsigned long interval(void)
	{
		return _interval_ms;
	}

    // Limited to 1 day
	void interval(unsigned long quant)
	{
	    if (quant <= 24UL * 60 * 60000)
        {
		    _interval_ms = quant;
        }
	}

	unsigned count(void)
	{
		return _dispense_count;
	}

    // Limited to queue size (nominally 20)
	void count(unsigned quant)
	{
		if (quant <= _queue->size()/2)
		{
			_dispense_count = quant;
		}
	}

    // Time elapsed since last feed in ms
	unsigned long elapsed(void)
    {
        unsigned long now = millis();
        unsigned long time_ms = (unsigned long)((long)now - (long)_dispense_time_ms);

        return time_ms;
    }

    void dispense(void)
    {
        _dispense_time_ms = millis();

        _queue->append(_steps_reverse);
        for (unsigned i = 0; i < _dispense_count; i++)
        {
            _queue->append(_steps_forward);
        }
    }

	void update(void)
	{
		if (elapsed() > _interval_ms)
		{
            dispense();
		}
	}
};

/* vim: set shiftwidth=4 expandtab:  */
