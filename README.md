# Favero_Repeater

The Favero Full Arm scoring machines use serial data output that can be interpreted through an Arduino and then used to send data signals for ws2812b LED lights for strip repeater lights.

Full credit for the original concept and code came from BenKohn2004 and their repository: https://github.com/BenKohn2004/Favero_Overlay

This uses one [Wemos D1 mini](https://www.amazon.com/Organizer-ESP8266-Internet-Development-Compatible/dp/B081PX9YFV/ref=sr_1_3?crid=3HISM104Q8NMO&keywords=wemos+d1+mini&qid=1636054524&qsid=133-6299387-8119633&sprefix=wemos%2Caps%2C400&sr=8-3&sres=B081PX9YFV%2CB08C7FYM5T%2CB08QZ2887K%2CB07W8ZQY62%2CB07RBNJLK4%2CB08FZ9YDGS%2CB07BK435ZW%2CB07V84VWSM%2CB07PF3NK12%2CB08H1YRN4M%2CB08FQYZX37%2CB073CQVFLK%2CB07VN328FS%2CB0899N647N%2CB07G9HZ5LM%2CB08MKLRSNH&srpt=SINGLE_BOARD_COMPUTER). The hardware is assembled as shown in the [diagram](Favero_repeater.png). This is a basic overview of how it works through a breadboard. I recommend using a protoboard/perfboard/pcb for real applications. 


<h2>Topology</h2>

<h3>Requirements</h3>

- Two ws2812 RGB LED strips and knowing how many total LEDs in each strip to change line 11 in [Wemos_Favero_Serial_Parser.ino](Wemos_Favero_Serial_Parser/Wemos_Favero_Serial_Parser.ino).  
- Power source for the RGB LED strips and microcontroller (not covered here)
- A 100 ohm resistor
- A favero full arm scoring machine with a rj-11 serial port on the back.
- A microcontroller: I am using a Wemo D1 mini in this example
- An RJ11 cable to connect from your Favero scoring machine to your microcontroller. An easy option is to use a RJ11 breakout such as [this](https://a.co/d/c3JmIds).

<h3>Microcontroller to Favero</h3>

The section in the [diagram](Favero_repeater.png) shows what is required from the microcontroller to the Favero scoring machine.

- Plug one end of an RJ-11 cable into the Favero FA scoring machine serial port on the back (keep powered off completely until ready to test).
- Wemo 3.3V pin to Favero RJ-11 pin 1.
- Wemo RX pin to Favero RJ-11 pin 2.
- Wemo GND pin to Favero RJ-11 pin 3.
    - There must be a 100 Ohm resistor in between the Favero scoring machine and GND (no other devices need the 100 ohm resistor).
    - The GND pin on the Wemo will also need to be connected to your LED strips and their power source ground as well. A common ground between the components is required.

<h3>Microcontroller to LEDs</h3>

- Connect your "left fencer" LED strip data to D2 (pin 4) (fencer on the left / FOTL) on the Wemo.
- Connect the "right fencer" LED strip data to D1 (pin 5) (fencer on the right / FOTR) on the Wemo.
- The GND pins will need to be connected on the same rail to your power source, LED strip, the scoring machine, and the Wemo's ground pin. A 100 ohm resistor is required from GND to the Favera FA scoring machine RJ-11 pin 3 (no other devices).
- Connect your 5v power to each LED strip. 


<h3>Edit  Wemos_Favero_Serial_Parser_Rev_1.ino</h3>

- Edit line 10 of [Wemos_Favero_Serial_Parser.ino](Wemos_Favero_Serial_Parser/Wemos_Favero_Serial_Parser.ino#L10) to the total number of LEDs (applies to both fencers).
- Edit line 44 of [Wemos_Favero_Serial_Parser_Rev_1.ino](Wemos_Favero_Serial_Parser/Wemos_Favero_Serial_Parser.ino#L44) to the power draw you want. This will cap FastLED to only output a maximum number of volts and milliamps to prevent your LEDs from going out. 
    - `FastLED.setMaxPowerInVoltsAndMilliamps(5, 2000)` - Where `5` is 5 volts, and `2000` is 2000 milliamps (2 amps). This applies to each LED matrix seperately.

<h3>Troubleshooting</h3>

I ran into an issue with my power supply for my LEDs causing lots of noise. I would see random serial output that didn't make sense because the scoring machine was having no changes, but I was receiving serial output for changes. Unplugging the LED power resolved the noise issue. I changed power supplies for my LEDs and it resolved my issue completely. I also got random junk data if my 3.3v wasn't stable, or I had a bad RJ-11 cable. 

Bad serial output (nothing changing on scoring machine):
```
11:30:44.289 -> The message in HEX is: FF,0,0,0,0,0,0,0,10,0,
11:30:45.554 -> The message in HEX is: FF,7F,0,0,0,0,0,0,0,0,
11:30:45.882 -> The message in HEX is: FF,FD,0,0,0,0,0,0,0,0,
```
Good serial output (nothing changing on scoring machine):
```
11:45:23.587 -> The message in HEX is: FF,0,0,0,0,0,0,0,30,0,
```
