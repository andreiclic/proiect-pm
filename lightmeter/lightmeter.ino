#include <U8glib.h>
#include <Wire.h>
#include <BH1750.h>

// digital pins definitions
#define SET_ISO_BUTTON 2
#define SET_ND_BUTTON 3
#define ACTIVATE_ND_SWITCH 4
#define MEASURE_BUTTON 5
#define ROTARY_ENCODER_A 6
#define ROTARY_ENCODER_B 7
#define SCREEN_RST 8
#define SCREEN_CE 9
#define SCREEN_DC 10
#define SCREEN_DIN 11
#define SCREEN_CLK 12

// analog pins definitions
#define LIGHT_SENSOR_SDA 4
#define LIGHT_SENSOR_SCL 5 

// button states
#define BUTTON_PRESSED 0
#define BUTTON_NOT_PRESSED 1
#define TOUCH_BUTTON_PRESSED 1
#define TOUCH_BUTTON_NOT_PRESSED 0

// switch states
#define SWITCH_ON 1
#define SWITCH_OFF 0

// button variables
int SET_ISO_BUTTON_state;
int SET_ND_BUTTON_state;
int MEASURE_BUTTON_state;

// switch variables
int ACTIVATE_ND_SWITCH_state;

// rotary encoder variables
int ROTARY_ENCODER_A_state;
int ROTARY_ENCODER_count;

// screen variables
U8GLIB_PCD8544 u8g(SCREEN_CLK, SCREEN_DIN, SCREEN_CE, SCREEN_DC, SCREEN_RST);

// light sensor variables
BH1750 LIGHT_SENSOR;

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

  // ROTARY ENCODER init
  pinMode(ROTARY_ENCODER_A, INPUT);
  pinMode(ROTARY_ENCODER_B, INPUT);
  ROTARY_ENCODER_A_state = digitalRead(ROTARY_ENCODER_A);
  ROTARY_ENCODER_count = 0;

  // SCREEN init
  pinMode(SCREEN_RST, OUTPUT);
  pinMode(SCREEN_CE, OUTPUT);
  pinMode(SCREEN_DC, OUTPUT);
  pinMode(SCREEN_DIN, OUTPUT);
  pinMode(SCREEN_CLK, OUTPUT);

  // LIGHT SENSOR init
  Wire.begin();
  LIGHT_SENSOR.begin();

  // serial init for debugging
  Serial.begin(9600);
  Serial.println("Lightmeter is ready!");
}

void draw(void) {
  // graphic commands to redraw the complete screen should be placed here  
  u8g.setFont(u8g_font_6x10);
  u8g.drawStr( 0, 22, "Hello World!");

  // outer frame
  u8g.drawFrame(0, 0, 84, 48);

  // inner frame
  u8g.drawFrame(30, 18, 21, 12);

  // 
}

void loop() {
  // read the button states
  int SET_ISO_BUTTON_state_tmp = digitalRead(SET_ISO_BUTTON);
  int SET_ND_BUTTON_state_tmp = digitalRead(SET_ND_BUTTON);
  int MEASURE_BUTTON_state_tmp = digitalRead(MEASURE_BUTTON);

  // read the switch state
  int ACTIVATE_ND_SWITCH_state_tmp = digitalRead(ACTIVATE_ND_SWITCH);

  // read the rotary encoder state
  int ROTARY_ENCODER_A_state_tmp = digitalRead(ROTARY_ENCODER_A);

  // read the light sensor value
  float lux = LIGHT_SENSOR.readLightLevel();

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

  // ROTARY ENCODER update
  if ( ROTARY_ENCODER_A_state_tmp != ROTARY_ENCODER_A_state ) {
    if ( digitalRead(ROTARY_ENCODER_B) != ROTARY_ENCODER_A_state_tmp ) {
      Serial.println("+1");
      ROTARY_ENCODER_count++;
    } else {
      Serial.println("-1");
      ROTARY_ENCODER_count--;
    }
    Serial.print("Position: ");
    Serial.println(ROTARY_ENCODER_count);
    
    ROTARY_ENCODER_A_state = ROTARY_ENCODER_A_state_tmp;
  }

  // LIGHT SENSOR update
  if ( MEASURE_BUTTON_state == TOUCH_BUTTON_PRESSED ) {
    Serial.print("Light: ");
    Serial.print(lux);
    Serial.println("lx");
  }

  u8g.firstPage();  
  //do {
    draw();
  //} while( u8g.nextPage() );
}
