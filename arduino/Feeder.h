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

#include "Stepper.h"

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
	unsigned _dispense_time_ms;

public:
	Feeder(StepQueue *queue, int reverse, int forward) : _queue(queue), _steps_reverse(reverse), _steps_forward(forward)
	{
	}

	unsigned long interval(void)
	{
		return _interval_ms;
	}

	void interval(unsigned long quant)
	{
		_interval_ms = quant;
	}

	unsigned count(void)
	{
		return _dispense_count;
	}

	void count(unsigned quant)
	{
		if (quant <= _queue->size()/2)
		{
			_dispense_count = quant;
		}
	}

	void update(void)
	{
		unsigned long now = millis();
		unsigned long time_ms = (unsigned long)((long)now - (long)_dispense_time_ms);

		Serial.print(_interval_ms);Serial.print("-");
		Serial.println(time_ms);

		if (time_ms > _interval_ms)
		{
			_dispense_time_ms = now;
			for (unsigned i = 0; i < _dispense_count; i++)
			{
				_queue->append(_steps_reverse);
				_queue->append(_steps_forward);
			}
		}
	}
};

/* vim: set shiftwidth=8 noexpandtab:  */
