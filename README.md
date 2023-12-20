# Favero_Repeater

The Favero Full Arm scoring machines use serial data output that can be interpreted through an Arduino and then used to send data signals for ws2812b LED lights for strip repeater lights.

Full credit for the original concept and original code came from BenKohn2004 and their repository: https://github.com/BenKohn2004/Favero_Overlay

This uses one [Wemos D1 mini](https://www.amazon.com/Organizer-ESP8266-Internet-Development-Compatible/dp/B081PX9YFV/ref=sr_1_3?crid=3HISM104Q8NMO&keywords=wemos+d1+mini&qid=1636054524&qsid=133-6299387-8119633&sprefix=wemos%2Caps%2C400&sr=8-3&sres=B081PX9YFV%2CB08C7FYM5T%2CB08QZ2887K%2CB07W8ZQY62%2CB07RBNJLK4%2CB08FZ9YDGS%2CB07BK435ZW%2CB07V84VWSM%2CB07PF3NK12%2CB08H1YRN4M%2CB08FQYZX37%2CB073CQVFLK%2CB07VN328FS%2CB0899N647N%2CB07G9HZ5LM%2CB08MKLRSNH&srpt=SINGLE_BOARD_COMPUTER) per fencing strip, and one LED object for each fencer (left and right). The hardware is assembled as shown in the [basic diagram](Favero_repeater.png). This is a basic overview of how it works through a breadboard. I recommend using a protoboard/perfboard/pcb for real applications. [Some people swear online](https://youtu.be/H4pFvqIxkhQ?si=C3Unc96JtROeyP44) to put a resistor between the data pin on the LED strip and the microcontroller, but I found my setup worked fine without it. The [full setup](Favero_repeater_full_setup.png) includes this resistor, but it may not be required for you.


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
    - The GND pin on the Wemo will also need to be connected to your LED strips and their power source ground as well. A common ground between all the components is required.

<h3>Microcontroller to LEDs</h3>

- Connect the "left fencer" LED strip data to D2 (pin 4) (fencer on the left / FOTL) on the Wemo.
- Connect the "right fencer" LED strip data to D1 (pin 5) (fencer on the right / FOTR) on the Wemo.
- The GND pins will need to be connected on the same rail to your power source, LED strip, the scoring machine, and the Wemo's ground pin. A 100 ohm resistor is required from GND to the Favera FA scoring machine RJ-11 pin 3 (no other devices). A common ground between all the components is required.
- Connect your 5v/12v/24v or whatever power source you have to each LED strip for their power. If you need help figuring out how to power your LEDs, https://quinled.info/ is a fantastic resource. 


<h3>Edit  Wemos_Favero_Serial_Parser_Rev_1.ino</h3>

- Edit line 10 of [Wemos_Favero_Serial_Parser.ino](Wemos_Favero_Serial_Parser/Wemos_Favero_Serial_Parser.ino#L10) to the total number of LEDs (applies to both fencers).
- Edit line 44 of [Wemos_Favero_Serial_Parser.ino](Wemos_Favero_Serial_Parser/Wemos_Favero_Serial_Parser.ino#L46) to the power draw you want. This will have FastLED tell the LED strips to only draw a maximum power draw to prevent your LEDs from going out, and set the amps to an appropriate amount. 
    - `FastLED.setMaxPowerInVoltsAndMilliamps(5, 2000)` - Where `5` is 5 volts, and `2000` is 2000 milliamps (2 amps). This applies to the total output FastLED will allow to be output.

<h3>Troubleshooting</h3>

<h4>My LEDs are showing the wrong color (sometimes)!</h4>
- The code is set to only send signals for the appropriate color for left and right fencers and white for off-target. If the colors are mismatched such as green and red on the opposite sides, then move your led strip or swap the data pin connections. If you are getting some other color like a blue or a unexpected white, its most likely your LED configuration that is causing the issue. There is a lot of knowledge that you need to know that comes with LED strips (that I hadn't known before this project), such as the power injection, wire gauge etc. 

<h4>The serial says I am getting a lot of wrong checksums?</h4>
- This is caused by noise somewhere. My tests were usually the LED data pin causing the noise. I couldn't figure out how to stop it so I added the checksum verification to the program for the microcontroller. The microcontroller will only accept messages from the scoring machine if the checksum matches to prevent corrupted data.  

<h4>There is a small delay in the lights when two fencers don't make a touch at the same exact time, but both lights come on.</h4>
- Your guess is as good as mine as to why this occurs. Could be noise causing the serial message to be thrown out so often it causes a delay. You might be able to get around this by sending a 5v signal to the serial connection rather than a 3.3v from the Wemos. I haven't tried it though.
