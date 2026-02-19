Hi!
So, here I'd like to share my architecture, it already has UI/UX design, so operating it shouldn't be hard, right?
No, I'm not joking, it has already human-designed UI, but it runs on your CPU, I've planned to implement it on the circuit's processor, but for now, let it use your CPU.
I'm writting this so my thesis examiner wouldn't be a total jack-ass.

HOW IT WORKS:
So, by using pyserial module, the compiler would capture the ML data on real-time. The Pyserial terminal, which is could be read by CPP program, would be taken as data-value as the servo-motors' motion would be moving on.
It has two compiler running on the background:
1. Index.py
Module thus I've used:
A. OpenCV
As the main-component on the ML side, it'll be improved on the bias-level, so the motion would be more smooth.
B. Pyserial
As the environment where both Python and CPP could share same memory, the terminal would captures data values from OpenCV module.

2. Main.CPP (Platform.io)
Module thus I've used:
A. Arduino
Old-school circuits where we could write prompt for the hardware run
B. Servo
One of the hardware, works as piston to move the skeletons.

HOW TO RUN:
Run main.exe on the command prompt, there'll be option on the control panels;
A. Connect Device (Unfinished)
B. Start Hand Tracker (You only can run it if the circuit's already connected)
C. Select Tracer (Unfinished)

Select the second option.
A camera windows will be openned.
Full documentation would be made on my Youtube channel.
