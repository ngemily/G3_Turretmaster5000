See main.c for the main test code.
LowLevelTest(): Low level test of SD functionality, including init and sector reads.
HighLevelTest(): High level test of SD functionality, with FILE level reads and writes.
	- NOTE: must have a file foo.txt before beginning the test.
	
LaserTest(): Flashes the laser on and off.
MotorPatternTest(): rotates the turret in a fixed pattern.

GUIDE TO FILES:
main.c: main test code.
diskio.[ch]: Low level functions for accessing the SD card. The SPI interface code goes here.
ff.[ch] and ffconf.h: This implements the filesystem layer. Taken straight from the FATFS project.
motorcontrol.[ch]: Code for controlling the laser and the motors.


Resources:
FATFS: http://elm-chan.org/fsw/ff/00index_e.html
SD SPI interface: http://elm-chan.org/docs/mmc/mmc_e.html
More SD SPI stuff: http://www.chlazza.net/sdcardinfo.html