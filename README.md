# Nintendo Switch Controller (Joy-Con) Shut Down

This program has just one function: To turn off Joy-Cons and other official Nintendo Switch controllers.

For some weird reason, Nintendo does not allow turning Switch controllers off. They just stay in
a sleep mode which drains the battery fully in about a week or two.

However, the controllers can be "turned off" (changed back to Shipping Low Power Mode)
with HID commands via Bluetooth. That should nearly completely stop the battery drain.

Usage:
1. Connect your controller with Bluetooth (USB does not work!)
2. Run the program, read the information carefully and click the correct buttons.

I have tested this with multiple Joy-Cons, Pro Controllers, NES Controllers, SNES controllers and N64 controllers 
but as this is not supported by Nintendo, you're on your own. Sega Genesis controllers are not supported for now as I don't have any of them.

USE WITH YOUR OWN RISK! I take no responsibility on whatever happens to your controller or computer or anything else.

## Thanks

I have no clue how to code C++ or how to reverse engineer the controllers, so this is based on the work of others.

Originally started as a fork of https://github.com/CTCaer/jc_toolkit, but after a couple of rewrites, does not really look like it anymore.

HID commands from: https://github.com/dekuNukem/Nintendo_Switch_Reverse_Engineering

Lots of help with HID code: https://github.com/shinyquagsire23/HID-Joy-Con-Whispering
