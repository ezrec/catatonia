a=catatonia.Stepper()
s=catatonia.Schedule()
j=catatonia.ScheduledAction(action=a.rotate, steps=100)
k=catatonia.ScheduledAction(action=a.rotate, steps=-10)
s.add(4, [j, k])
s.start()