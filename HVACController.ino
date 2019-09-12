// Pin 0 is labelled "RX0" // can only be used for digital (in only) if we're not using serial (e.g. no Serial.begin() call)
// Pin 1 is labelled "TX1" // can only use for digital (out only) if we're not using serial // ditto
// Pins 3-13 are labelled "D3" - "D13" respectively

// first nest
#define NESTA_G   2 // green (fan)
#define NESTA_W1  3 // white (heat)
#define NESTA_Y1  4 // yellow (a/c)

// second nest
#define NESTB_G   5
#define NESTB_W1  6
#define NESTB_Y1  7

// output lines to the HVAC unit
#define HVACOUT_G   8
#define HVACOUT_W1  9
#define HVACOUT_Y1 10

// runs once
void setup() {
  Serial.begin(57600);

  // read from the 2 nests, write control signals to the relays to send to the furnace
  pinMode(NESTA_G, INPUT);
  pinMode(NESTA_W1, INPUT);
  pinMode(NESTA_Y1, INPUT);
  
  pinMode(NESTB_G, INPUT);
  pinMode(NESTB_W1, INPUT);
  pinMode(NESTB_Y1, INPUT);

  pinMode(HVACOUT_G, OUTPUT);
  pinMode(HVACOUT_W1, OUTPUT);
  pinMode(HVACOUT_Y1, OUTPUT);

  Serial.println("Hello from setup...");
}



// runs repeatedly
void loop() {
  // silly cycle through 3 output LEDs for now
  Serial.println("Hello from loop()...");
  digitalWrite(HVACOUT_Y1, LOW);
  digitalWrite(HVACOUT_G, HIGH);
  delay(1000 /* in ms */);

  digitalWrite(HVACOUT_G, LOW);
  digitalWrite(HVACOUT_W1, HIGH);
  delay(1000 /* in ms */);

  digitalWrite(HVACOUT_W1, LOW);
  digitalWrite(HVACOUT_Y1, HIGH);
  delay(1000);
}
