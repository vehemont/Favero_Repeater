#include <FastLED.h>
#define MY_NAME "REPEATER"

const unsigned int MAX_MESSAGE_LENGTH = 10;
const unsigned int MAX_SERIAL_BUFFER_BYTES = 512;
const char STARTING_BYTE = 255;

const int FOTL_DATA = 4; // D2
const int FOTR_DATA = 5; // D1
const int NUM_LEDS = 300; // Number of LEDs in your LED strip/matrix
unsigned int COUNTER = 0;
bool MACHINE_OFF = true;

CRGB left[NUM_LEDS]; // FOTL LED array
CRGB right[NUM_LEDS]; // FOTR LED array

struct __attribute__((packed)) dataPacket {
  int unsigned Right_Score;
  int unsigned Left_Score;
  int unsigned Seconds_Remaining;
  int unsigned Minutes_Remaining;
  bool Green_Light;
  bool Red_Light;
  bool White_Green_Light;
  bool White_Red_Light;
  bool Yellow_Green_Light;
  bool Yellow_Red_Light;
  bool Yellow_Card_Green;
  bool Yellow_Card_Red;
  bool Red_Card_Green;
  bool Red_Card_Red;
  bool Priority_Left;
  bool Priority_Right;
};

// Shows if new data is available for display
bool new_data = false;

// Initializes Message_Position
unsigned int message_pos = 0;

void setup() {

  FastLED.addLeds<WS2812, FOTL_DATA, GRB>(left, NUM_LEDS);
  FastLED.addLeds<WS2812, FOTR_DATA, GRB>(right, NUM_LEDS);
  FastLED.setMaxPowerInVoltsAndMilliamps(5, 2000); 

  Serial.setRxBufferSize(1024);
  Serial.begin(2400);  // initialize serial port
  delay(10);
  Serial.println();
  Serial.println();
  Serial.println();
  Serial.print("Initializing...");
  Serial.println(MY_NAME);
  Serial.println("Initialized.");
}

void loop() {

  EVERY_N_MILLISECONDS(200) {
    wdt_reset();
  }

  dataPacket packet;

  //  Check to see if anything is available in the serial receive buffer
  while (Serial.available() > 0) {

    // Create a place to hold the incoming message
    COUNTER = 0;
    MACHINE_OFF = false;
    static char message[MAX_MESSAGE_LENGTH];
    static char prev_message[MAX_MESSAGE_LENGTH];

    char inByte = Serial.read();

    // Message coming in (check if starting character)
    if (inByte == STARTING_BYTE) {
      // Resets message position
      message_pos = 0;
      // Stores the Byte in the message position
      message[message_pos] = inByte;
      //increments message position
      message_pos++;
    } else if (message_pos < (MAX_MESSAGE_LENGTH - 1)) {
      // Stores the Byte in the message position
      message[message_pos] = inByte;
      //increments message position
      message_pos++;
    } else if (message_pos == (MAX_MESSAGE_LENGTH - 1)) {
      message[message_pos] = inByte; // add the CRC 9th byte to the array
      byte checksum = 0;
      
      for (int i = 0; i < MAX_MESSAGE_LENGTH; i++) {
        if (i == 9) {
          checksum = checksum % 256;
        } else {
            checksum += message[i];
        }
      }
      if (message[9] == checksum) {
        // Prints the Message if different from previous
        // Excudes the internal use byte in position 7 or check sum.
        if (message[1] != prev_message[1] or message[2] != prev_message[2] or message[3] != prev_message[3] or message[4] != prev_message[4] or message[5] != prev_message[5] or message[6] != prev_message[6] or message[8] != prev_message[8]) {
          // Sets New Data to True
          Serial.print("The message in HEX is: ");
          for (int i = 0; i < MAX_MESSAGE_LENGTH; i++) {
            Serial.print(message[i], HEX);
            Serial.print(",");
          }
          Serial.print('\n');
          Serial.print("The checksum is: ");
          Serial.print(checksum, HEX);
          Serial.print('\n');

          new_data = true;

          // Assigns values from message to packet
          packet.Green_Light = bitRead(message[5], 3);
          packet.Red_Light = bitRead(message[5], 2);
          packet.White_Green_Light = bitRead(message[5], 1);
          packet.White_Red_Light = bitRead(message[5], 0);
          packet.Yellow_Green_Light = bitRead(message[5], 4);
          packet.Yellow_Red_Light = bitRead(message[5], 5);

          // Stuff not needed for repeaters
          packet.Yellow_Card_Green = bitRead(message[8], 2);
          packet.Yellow_Card_Red = bitRead(message[8], 3);
          packet.Red_Card_Green = bitRead(message[8], 0);
          packet.Red_Card_Red = bitRead(message[8], 1);

          packet.Priority_Right = bitRead(message[6], 2);
          packet.Priority_Left = bitRead(message[6], 3);

          // Stores the Score and Time
          packet.Right_Score = hex_string_to_int(message[1]);
          packet.Left_Score = hex_string_to_int(message[2]);
          packet.Seconds_Remaining = hex_string_to_int(message[3]);
          packet.Minutes_Remaining = hex_string_to_int(message[4]);

          // Sets Previous Message to Current Message
          for (int i = 0; i < MAX_MESSAGE_LENGTH; i++) {
            prev_message[i] = message[i];
          }

          // Resets Message Position
          message_pos = 0;

          // Clears the Serial Buffer if more than Max Buffer Bytes bytes in the buffer
          if (Serial.available() > MAX_SERIAL_BUFFER_BYTES) {
            Serial.println("...............Clearing the Serial Buffer...............");
            while (Serial.available() > 0) {
              char inByte = Serial.read();
            }
          }
          // Right fencer lights
          if (packet.Green_Light == 1) {
            fill_solid(right, NUM_LEDS, CRGB::Green); // On target right
            Serial.println("FOTR ON TARGET");
          } else if (packet.White_Green_Light == 1) {
            fill_solid(right, NUM_LEDS, CRGB::White); // Off target right
            Serial.println("FOTR OFF TARGET");
          } else {
            fill_solid(right, NUM_LEDS, CRGB::Black); // No light on right
            Serial.println("FOTR NO LIGHT");
          }
          // Left fencer lights
          if (packet.Red_Light == 1) {
            fill_solid(left, NUM_LEDS, CRGB::Red);
            Serial.println("FOTL ON TARGET");
          } else if (packet.White_Red_Light == 1) {
            fill_solid(left, NUM_LEDS, CRGB::White);
            Serial.println("FOTL OFF TARGET");
          } else {
            fill_solid(left, NUM_LEDS, CRGB::Black);
            Serial.println("FOTL NO LIGHT");
          }
          FastLED.show(); // Send the changes to the LEDs
        }
      } else {
      Serial.println("Wrong checksum! Throwing it out.");
      Serial.print('\n');
      Serial.print('\n');
      }
    } else {
        Serial.println("Unexpected Message Position, Reseting to zero.");
        message_pos = 0;
    }
  }
  COUNTER++;
  if (COUNTER > 10000) { // turn off the lights if 10,000 loops have passed and haven't gotten any serial connection.
    if (MACHINE_OFF == false) { // only turn the lights off the lights once if 10,000 loops have passed
      MACHINE_OFF = true;
      fill_solid(left, NUM_LEDS, CRGB::Black);
      fill_solid(right, NUM_LEDS, CRGB::Black);
      Serial.println("machine off");
      FastLED.show();
    }
    COUNTER = 0;
    MACHINE_OFF = true;
      }
}
