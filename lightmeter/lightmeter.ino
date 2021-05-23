// digital pins definitions
#define SET_ISO_BUTTON 2
#define SET_ND_BUTTON 3
#define ACTIVATE_ND_SWITCH 4
#define MEASURE_BUTTON 5

// button states
#define BUTTON_PRESSED 0
#define BUTTON_NOT_PRESSED 1
#define TOUCH_BUTTON_PRESSED 1
#define TOUCH_BUTTON_NOT_PRESSED 0

// switch states
#define SWITCH_ON 1
#define SWITCH_OFF 0

// button states
int SET_ISO_BUTTON_state;
int SET_ND_BUTTON_state;
int MEASURE_BUTTON_state;

// switch states
int ACTIVATE_ND_SWITCH_state;

void setup() {
  // SET ISO BUTTON init
  pinMode(SET_ISO_BUTTON, INPUT_PULLUP);
  SET_ISO_BUTTON_state = BUTTON_NOT_PRESSED;

  // SET ND BUTTON init
  pinMode(SET_ND_BUTTON, INPUT_PULLUP);
  SET_ND_BUTTON_state = BUTTON_NOT_PRESSED;

  // ACTIVATE ND SWITCH init
  pinMode(ACTIVATE_ND_SWITCH, INPUT_PULLUP);
  ACTIVATE_ND_SWITCH_state = SWITCH_OFF;

  // MEASURE BUTTON init
  pinMode(MEASURE_BUTTON, INPUT);
  MEASURE_BUTTON_state = TOUCH_BUTTON_NOT_PRESSED;

  // serial init for debugging
  Serial.begin(9600);
  Serial.println("Lightmeter is ready!");
}

void loop() {
  // read the button states
  int SET_ISO_BUTTON_state_tmp = digitalRead(SET_ISO_BUTTON);
  int SET_ND_BUTTON_state_tmp = digitalRead(SET_ND_BUTTON);
  int MEASURE_BUTTON_state_tmp = digitalRead(MEASURE_BUTTON);

  // read the switch state
  int ACTIVATE_ND_SWITCH_state_tmp = digitalRead(ACTIVATE_ND_SWITCH);

  // SET ISO BUTTON update
  if ( SET_ISO_BUTTON_state_tmp != SET_ISO_BUTTON_state ) {
    if ( SET_ISO_BUTTON_state_tmp == BUTTON_PRESSED ) {
      Serial.println("Set ISO button was pressed");
    }
    
    SET_ISO_BUTTON_state = SET_ISO_BUTTON_state_tmp;
  }

  // SET ND BUTTON update
  if ( SET_ND_BUTTON_state_tmp != SET_ND_BUTTON_state ) {
    if ( SET_ND_BUTTON_state_tmp == BUTTON_PRESSED ) {
      Serial.println("Set ND button was pressed");
    }
    
    SET_ND_BUTTON_state = SET_ND_BUTTON_state_tmp;
  }

  // ACTIVATE ND SWITCH update
  if ( ACTIVATE_ND_SWITCH_state_tmp != ACTIVATE_ND_SWITCH_state ) {
    if ( ACTIVATE_ND_SWITCH_state_tmp == SWITCH_ON ) {
      Serial.println("Activate ND switch is ON");
    }

    ACTIVATE_ND_SWITCH_state = ACTIVATE_ND_SWITCH_state_tmp;
  }

  // MEASURE BUTTON update
  if ( MEASURE_BUTTON_state_tmp != MEASURE_BUTTON_state ) {
    if ( MEASURE_BUTTON_state_tmp == TOUCH_BUTTON_PRESSED ) {
      Serial.println("Measure button was pressed");
    }
    
    MEASURE_BUTTON_state = MEASURE_BUTTON_state_tmp;
  }

}
