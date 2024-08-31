# Nintendo Switch Controller (Joy-Con) Shut Down

This program has just one function: To turn off Joy-Cons and other official Nintendo Switch controllers.

For some weird reason, Nintendo does not allow turning Switch controllers off. They just stay in
a sleep mode which drains the battery fully in only about a week or two.

However, the controllers can be "turned off" (changed back to Shipping Low Power Mode)
with HID commands via Bluetooth. Doing this nearly completely stops any battery drain.

USE WITH YOUR OWN RISK! I take no responsibility on whatever happens to your controller or computer or anything else.
Even though this is using a HID command built-in to the controller firmware (subcommand 0x08), it is an undocumented feature and unsupported by Nintendo, so you're on your own. As far as I know, Nintendo officially only ever uses it to turn the low power mode off once when it's new.

## Supported official controllers
* Joy-Cons (L and R)
* Pro Controllers
* NES Controllers (L and R, detected as a Joy-Con, they share the same Device ID)
* SNES Controllers
* N64 Controllers

I have tested this multiple times with multiple units of all of the controllers in the above list.

Famicom controllers should work the same as NES controllers, but I have not tested them.  
Sega Genesis controllers are not supported for now as I don't have any of them to test with.  
Unofficial controllers are not supported.

## Usage
1. Connect your controller to your PC with Bluetooth (USB does not work!)
2. Run the program, read the information carefully and click the correct buttons.

## Thanks

I'm not very experienced in writing C/C++ or reverse engineering the controllers, so this is based on the work of others.

Originally started as a fork of https://github.com/CTCaer/jc_toolkit, but after a couple of rewrites, does not really look like it anymore.

HID commands from: https://github.com/dekuNukem/Nintendo_Switch_Reverse_Engineering

Lots of help with HID code: https://github.com/shinyquagsire23/HID-Joy-Con-Whispering

## Update 3 years in
Some of the controllers I originally turned off with a full/nearly full battery in 2021 are now starting to die out and are in a need of a charge. Therefore I'd estimate around 2-3 years of battery life when turned off. To keep the controllers turned off, charge them with a charging grip and/or a USB-C charger. Connecting them to a Switch immediately turns them on again.

For the best battery cell longevity I would recommend charging them to 50-60% once a year and maybe giving them a full charge/discharge cycle as well to calibrate the battery percentage: Use until the battery is flat -> charge fully -> use until battery level is around 60% and turn it off. Storing them in the fridge (2-8C) helps as well. Don't freeze.

For more info about lithium batteries, Battery University is an excellent source: https://batteryuniversity.com/article/bu-808-how-to-prolong-lithium-based-batteries
