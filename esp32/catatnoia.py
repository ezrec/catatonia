#
# Catonia
#

import time
import _thread

from machine import Pin

class Stepper(object):
    def __init__(self):
        self.enable = Pin(5, Pin.OUT)
        self.step = Pin(16, Pin.OUT)
        self.dir = Pin(17, Pin.OUT)

    def rotate(self, steps=10):
        if steps < 0:
            self.dir.value(0)
            steps = -steps
        else:
            self.dir.value(1)

        self.enable.value(0)
        for i in range(steps):
            self.step.value(1)
            time.sleep_ms(1)
            self.step.value(0)
            time.sleep_ms(5)
        self.enable.value(1)

class ScheduledAction(object):
    def __init__(self, action = None, **kwargs):
        self.action = action
        self.kwargs = kwargs

    def execute(self):
        self.action(**self.kwargs)

class Schedule(object):
    def __init__(self):
        self.todo = []
        self.dead = True

    def add(self, interval=1, action_list = []):
        self.todo += [( time.time() + interval, interval, action_list)]
        self.todo.sort()

    def next(self):
        now = time.time()
        while len(self.todo) > 0 and self.todo[0][0] <= now:
            was, interval, action_list = self.todo.pop(0)
            for x in action_list:
                x.execute()
            self.add(interval, action_list)

    def thread_(self):
        while not self.dead:
            time.sleep(1)
            self.next()

    def start(self):
        self.dead = False
        _thread.start_new_thread(self.thread_, [])
