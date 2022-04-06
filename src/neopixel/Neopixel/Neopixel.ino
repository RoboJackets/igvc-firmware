#include <Adafruit_NeoPixel.h>
#ifdef __AVR__
 #include <avr/power.h> // Required for 16 MHz Adafruit Trinket
#endif

#define PIN        A1 // DO NOT CHANGE

#define NUMPIXELS 30 // Popular NeoPixel ring size

#define ENCODER_NO_CONNECTION [255,0,0,0]
#define ENCODER_INVALID_DATA [255,0,0,1]
#define ODRIVE_NO_CONNECTION [0,255,0,0]

// When setting up the NeoPixel library, we tell it how many pixels,
// and which pin to use to send signals. Note that for older NeoPixel
// strips you might need to change the third parameter -- see the
// strandtest example for more information on possible values.
Adafruit_NeoPixel pixels(NUMPIXELS, PIN, NEO_GRB + NEO_KHZ800);

#define DELAYVAL 10 // Time (in milliseconds) to pause between pixels

int state[4] = {0, 0, 255, 0};
void setup() {
  pixels.begin(); // INITIALIZE NeoPixel strip object (REQUIRED)
}

void loop() {
  pixels.clear(); // Set all pixel colors to 'off'
  state[0] = random(0, 255);
  state[1] = random(0, 255);
  state[2] = random(0, 255);
  
  for(int i=0; i<NUMPIXELS; i++) { // For each pixel...
    pixels.setPixelColor(i, pixels.Color(state[0], state[1], state[2]));
    //pixels.setBrightness((millis() % 255));

    pixels.show();   // Send the updated pixel colors to the hardware.

    delay(DELAYVAL); // Pause before next pass through loop
  }
    state[0] = random(0, 255);
  state[1] = random(0, 255);
  state[2] = random(0, 255);
  for (int i = NUMPIXELS; i >= 0; i--) {
    pixels.setPixelColor(i, pixels.Color(state[0], state[1], state[2]));
    pixels.show();   // Send the updated pixel colors to the hardware.

    delay(DELAYVAL); // Pause before next pass through loop
  }
}
