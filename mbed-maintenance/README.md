Mbed Maintenance
----------------

Is there an issue with the mbed on the robot? Then this is the place for you.
This folder contains useful files for debugging the mbed. This README will 
document things we discover about the mbed to help you debug things later.

Mbed
====

IGVC uses the LPC1768 mbed from NXP. The latest information about it can be found at
the [mbed website for it](https://os.mbed.com/platforms/mbed-LPC1768/). This
website has links to the latest firmware, the datasheet, the user manual, and
more. For the status of the mbeds that we own, check the spreadsheet in the IGVC
drive. Be sure to update that spreadsheet if anything happens to the mbeds.

Firmware
========

All the mbeds used by IGVC are up-to-date with the latest firmware. You should
not have to reflash the firmware, unless for some reason you downgraded it.
The latest firmware is stored here as `mbedmicrontroller_141212.if`.
To flash the firmware, all you need to do is copy the file to the mbed device
and power cycle (turn off and on again) the mbed. It should take a little to
reconnect, but when it reappears, the .if file should be gone.

File Management
===============

The mbed has a file system that you can interface with over USB. This system
lets you easily upload code to the mbed to execute. However, it can sometimes
cause problems when re-uploading the same code or deleting files. If you are
having trouble getting the mbed to run your code, it may be due to this. To fix
this:

1) Delete all `.bin` files from the mbed
2) Delete the trash directory (this may appear as .trash-1000, .Trashes, etc.)
3) Power cycle the mbed and ensure that no files appear when it is plugged in
4) Upload your program to the mbed again

The exact cause of this problem is unknown but it seems to resolve most issues
when getting a program to run. In particular, this seems to occur when replacing
the file (instead of uploading a separate file) or deleting the currently
running program.

Blinky
======

Often when debugging the IGVC firmware, it can be useful to see if the mbed is
running code properly. For convenience, this directory contains two test
programs that you can use for seeing that the mbed is indeed working and
running. The first is called `blinky-mbed` and the second is called
`blinky-mbed-os-5`. They both run a simple blinky program, except the first uses
the mbed 2 library and the second uses the mbed-os 5 library. If neither work,
it is likely that the mbed is broken. If only the first works, then there may be
an issue with the firmware. This would be a good time to reflash the mbed.

FAQ
===

Q: Is it ok if the mbed is plugged into the logic board and computer at the same
time?

  A: Yes it is. The mbed has components (diodes) that allow both power sources to
be active at the same time.



Q: I keep resetting the mbed but nothing is changing. Is the mbed broken?

  A: Likely not. Make sure that when you hit the reset button that the status
light turns off and back on. It is possible that when pressing the button that
you don't activate the switch and are just pressing the plastic down.

---

_Update this file with problems you encounter and how to solve them when you find
them._

_Updated 2/17/2020 by Dallas D. (@dallas-d)_
