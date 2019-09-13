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

// the controls for the physical air ducts
#define VENT_A 11
#define VENT_B 12

// i believe the relays are reversed, so switch these. for debugging now HIGH=open is nice.
#define VENT_OPEN     HIGH
#define VENT_CLOSED    LOW
 
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

  pinMode(VENT_A, OUTPUT);
  pinMode(VENT_B, OUTPUT);
}


typedef struct NestState {
  int G;
  int W1;
  int Y1;
} NestState;

// silly cycle through 3 output LEDs for now
// TODO: delete me
void   debugCycle3OutputLEDs() {
  digitalWrite(HVACOUT_Y1, LOW);
  digitalWrite(HVACOUT_G, HIGH);
  delay(300 /* in ms */);

  digitalWrite(HVACOUT_G, LOW);
  digitalWrite(HVACOUT_W1, HIGH);
  delay(300);

  digitalWrite(HVACOUT_W1, LOW);
  digitalWrite(HVACOUT_Y1, HIGH);
  delay(300);

}

// Helper methods for querying the state of a given nest
bool isHeating(struct NestState nest) {
  return nest.W1;
}

bool isCooling(struct NestState nest) {
  return nest.Y1;
}

bool isBlowing(struct NestState nest) {
  return nest.G;
}

void setHVACFromNest(NestState nest) {
  digitalWrite(HVACOUT_G, nest.G);
  digitalWrite(HVACOUT_W1, nest.W1);
  digitalWrite(HVACOUT_Y1, nest.Y1);
}

void setVent(int vent, int setting) {
  digitalWrite(vent, setting);
}

// runs repeatedly
#define OUTBUF_LEN (80)
void loop() {
  NestState nesta, nestb;
  nesta = {digitalRead(NESTA_G), digitalRead(NESTA_W1), digitalRead(NESTA_Y1)};
  nestb = {digitalRead(NESTB_G), digitalRead(NESTB_W1), digitalRead(NESTB_Y1)};

  char outbuf[OUTBUF_LEN];
  snprintf(outbuf, OUTBUF_LEN, "Nest A: G:%d W1:%d Y1:%d.     Nest B: G:%d W1:%d Y1:%d",
    nesta.G, nesta.W1, nesta.Y1, nestb.G, nestb.W1, nestb.Y1);
  Serial.println(outbuf);


  // both are heading
  if ((isHeating(nesta) && isHeating(nestb))
   || (isCooling(nesta) && isCooling(nestb))) {
    setHVACFromNest(nesta);
    setVent(VENT_A, VENT_OPEN);
    setVent(VENT_B, VENT_OPEN);
    // assert isBlowing(nestA) && isBlowing(nestB)

    snprintf(outbuf, OUTBUF_LEN, "\tCase 1: Both thermostats are %s",
      (isHeating(nesta) ? "heating" : "cooling"));
    Serial.println(outbuf);
  } else if (isHeating(nesta) || isCooling(nesta)) {
    // if 2 thermostats are in different Heat/AC modes, "A" wins
    // assert(isBlowing(nesta))
    setHVACFromNest(nesta);
    setVent(VENT_A, VENT_OPEN);
    setVent(VENT_B, VENT_CLOSED);

    snprintf(outbuf, OUTBUF_LEN, "\tCase 2: Thermostat A is %s, it wins.",
      (isHeating(nesta) ? "heating" : "cooling"));
    Serial.println(outbuf);
  } else if (isHeating(nestb) || isCooling(nestb)) {
    // B is commanding either heat or cool, so do room B only.
    // assert(isBlowing(nestb)
    setHVACFromNest(nestb);
    setVent(VENT_A, VENT_CLOSED);
    setVent(VENT_B, VENT_OPEN);
    
    snprintf(outbuf, OUTBUF_LEN, "\tCase 3: Thermostat B is %s, it wins.",
      (isHeating(nestb) ? "heating" : "cooling"));
    Serial.println(outbuf);
  } else if (isBlowing(nesta) && isBlowing(nestb)) {
    setHVACFromNest(nesta);
    setVent(VENT_A, VENT_OPEN);
    setVent(VENT_B, VENT_OPEN);
    
    Serial.println("\tCase 4: Both thermostats are blowing.");
  } else if (isBlowing(nesta)) {
    setHVACFromNest(nesta);
    setVent(VENT_A, VENT_OPEN);
    setVent(VENT_B, VENT_CLOSED);

    Serial.println("\tCase 5: Thermostat A is blowing, it wins.");
  } else if (isBlowing(nestb)) {
    setHVACFromNest(nestb);
    setVent(VENT_A, VENT_CLOSED);
    setVent(VENT_B, VENT_OPEN);

    Serial.println("\tCase 6: Thermostat B is blowing, it wins.");
  } else { // neither thermostat doing anything...
    // assert everything is off;
    setHVACFromNest(nesta);
    setVent(VENT_A, VENT_OPEN);
    setVent(VENT_B, VENT_OPEN);
    Serial.println("\tCase 7: Neither thermostat doing anything.");
  }

  delay(1000);  
}
