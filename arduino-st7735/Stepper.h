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

class Stepper
{
    int _enable;
    int _dir;
    int _step;

public:
    Stepper(int pin_enable, int pin_dir, int pin_step) : _enable(pin_enable), _dir(pin_dir), _step(pin_step) {}

    void begin()
    {
        enable(false);
    }

    void enable(bool state = true)
    {
        digitalWrite(_step, false);
        digitalWrite(_enable, !state);
    }

    void dir(bool is_reverse)
    {
		digitalWrite(_dir, is_reverse);
	}

	void step(bool phase)
	{
		digitalWrite(_step, phase);
    }
};

class StepAction
{
	int _steps;
	unsigned _mid_step_delay;
	unsigned _step_delay;
	bool _dir;
	bool _phase;
	bool _enable_set;
	Stepper *_stepper;
public:
	StepAction() {}

	StepAction(Stepper *stepper, int steps, unsigned step_delay) : _stepper(stepper)
	{
		_step_delay = step_delay;

		if (steps < 0)
		{
			_dir = true;
			_steps = -steps;
		}
		else
		{
			_steps = steps;
			_dir = false;
		}

		_enable_set = false;
	}

	// Executes in ISR context
	// Return true if active, false if completed
	bool update_isr()
	{
		if (_mid_step_delay > 0)
		{
			_mid_step_delay--;
			return true;
		}

		if (!_enable_set)
		{
			_stepper->dir(_dir);
			_stepper->enable(true);
			_enable_set = true;
		}

		if (_steps == 0)
		{
			_stepper->enable(false);
			return false;
		}

		_stepper->step(_phase);
		_phase = !_phase;

		if (_phase == false)
		{
			_steps--;
			_mid_step_delay = _step_delay;
		}

		return true;
	}
};

template <const unsigned _size>
class StepQueueClass
{
	StepAction _queue[_size];
	unsigned _head;
	unsigned _count;

	Stepper *_stepper;
	unsigned _step_delay;
public:
	StepQueueClass(Stepper *stepper, unsigned step_delay) : _stepper(stepper), _step_delay(step_delay) {}

	bool append(int steps)
	{
		bool rc;

		noInterrupts();

		if (_count >= _size)
		{
			rc = false;
		}
		else
		{
			_queue[(_head + _count) % _size] = StepAction(_stepper, steps, _step_delay);
			_count++;

			rc = true;
		}
		interrupts();

		return rc;
	}

	const unsigned size(void)
	{
		return _size;
	}

	// Executes in ISR context
	bool update_isr(void)
	{
		if (!_count)
		{
			return false;
		}

		if (!_queue[_head].update_isr())
		{
			_head = (_head + 1) % _size;
			_count--;
		}

		return true;
	}
};

typedef StepQueueClass<8> StepQueue;

/* vim: set shiftwidth=4 expandtab:  */
