**Device Driver for a 16x2 LCD DISPLAY in 4 bit mode.**

**Description:**

This is a kernel level Linux Device driver to control a 16x2 character LCD (with HD44780 LCD controller) with 4 bit mode. The LCD is interfaced with raspberry pi using GPIO pins.


**Components Required**
* A Raspberry Pi
* 16 x 2 LCD Module
* 10 KΩ Potentiometer
* Breadboard
* Jumper wires
* 5V – 2A Power Supply
* Laptop, Ethernet Cable, etc.

The pin description of the 16×2 LCD Display Module is shown in the following table:
![WhatsApp Image 2021-12-09 at 13 05 18](https://user-images.githubusercontent.com/91193582/145353592-964aa964-5452-46bf-9cd4-81c7c0ea5f0e.jpeg)


**HARDWARE DESIGN:**
![WhatsApp Image 2021-12-09 at 12 42 03](https://user-images.githubusercontent.com/91193582/145350653-c5740613-2254-4f52-b469-4019572dd807.jpeg)

![WhatsApp Image 2021-12-08 at 22 31 43](https://user-images.githubusercontent.com/91193582/145343593-26f51f67-99d8-431e-a07a-a775c554002e.jpeg)

**OUTPUT:**
![WhatsApp Image 2021-12-08 at 22 35 21](https://user-images.githubusercontent.com/91193582/145343517-e06864af-27f4-4ecf-b341-6b63e015ed07.jpeg)

**How to operate the Project?**
1) Kernel level program:
* Download the project folder in which the kernel level codes (klcd.c, klcd.h) and Makefile are included.
* Change the directory to the downloaded folder(lcd_4bit) and run "**sudo make all**" command to build the program.
* Then insert the kernel moodule klcd.ko using "**sudo insmod klcd.ko**".
* Change the permissions of the file using "**sudo chmod 666 /dev/klcd**"
* Then type whatever message you want to display on your lcd using "**echo "YOUR MESSAGE" >/dev/klcd**"

2)User level program Usage:
* 
