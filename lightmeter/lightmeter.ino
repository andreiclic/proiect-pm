// includes
#include <U8glib.h>
#include <Wire.h>
#include <BH1750.h>
#include <math.h>

// digital pins definitions
#define SET_ISO_BUTTON 2
#define SET_ND_BUTTON 3
#define ACTIVATE_ND_SWITCH 4
#define MEASURE_BUTTON 5
#define ROTARY_ENCODER_A 6
#define ROTARY_ENCODER_B 7
#define ROTARY_ENCODER_BUTTON 8
#define SCREEN_RST 9
#define SCREEN_CE 10
#define SCREEN_DC 11
#define SCREEN_DIN 12
#define SCREEN_CLK 13

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

// output states
#define SHOW_EV 0
#define SHOW_LUX 1

// button variables
int SET_ISO_BUTTON_state;
int SET_ND_BUTTON_state;
int MEASURE_BUTTON_state;

// switch variables
int ACTIVATE_ND_SWITCH_state;

// rotary encoder variables
int ROTARY_ENCODER_A_state;
int ROTARY_ENCODER_count;
int ROTARY_ENCODER_BUTTON_state;

// screen variables
U8GLIB_PCD8544 u8g(SCREEN_CLK, SCREEN_DIN, SCREEN_CE, SCREEN_DC, SCREEN_RST);

// light sensor variables
BH1750 LIGHT_SENSOR;
float lux;
float EV_100;
int EV_100_rounded;
int LIGHT_SENSOR_show_mode;

// ISO array
int ISO_values[15] = {1, 3, 6, 12, 25, 50, 100, 200, 400, 800, 1600, 3200, 6400, 12800, 25600};
int ISO_values_len = 15;
int ISO_index;

// ND filter array
int ND_values[13] = {2, 4, 8, 16, 32, 64, 128, 256, 512, 1024, 2048, 4096, 8192};
int ND_values_len = 13;
int ND_index;

// aperture array
char aperture_values[16][4] = {"1", "1.4", "2", "2.8", "4", "5.6", "8", "11", "16", "22", "32", "45", "64", "128", "181", "256"};
int aperture_values_len = 16;
int aperture_index;

// shutter speed array
char shutter_speed_values[25][7] = {"1/4000", "1/2000", "1/1000", "1/500", "1/250", "1/125", "1/60", "1/30", "1/15", "1/8", "1/4", "1/2", "1", "2", "4", "8", "16", "32", "1m4", "2m8", "4m16", "8m32", "17m4", "34m8", "1h8m16"};
int shutter_speed_len = 25;
int shutter_speed_index;

// state variables
bool input_ISO;
bool input_ND;
bool use_ND;
bool blink_state;
unsigned long blink_duration;
unsigned long blink_last;

void setup() {
  // SET ISO BUTTON init
  pinMode(SET_ISO_BUTTON, INPUT_PULLUP);
  SET_ISO_BUTTON_state = BUTTON_NOT_PRESSED;

  // SET ND BUTTON init
  pinMode(SET_ND_BUTTON, INPUT_PULLUP);
  SET_ND_BUTTON_state = BUTTON_NOT_PRESSED;

  // ACTIVATE ND SWITCH init
  pinMode(ACTIVATE_ND_SWITCH, INPUT_PULLUP);
  ACTIVATE_ND_SWITCH_state = digitalRead(ACTIVATE_ND_SWITCH);

  // MEASURE BUTTON init
  pinMode(MEASURE_BUTTON, INPUT);
  MEASURE_BUTTON_state = TOUCH_BUTTON_NOT_PRESSED;

  // ROTARY ENCODER init
  pinMode(ROTARY_ENCODER_A, INPUT);
  pinMode(ROTARY_ENCODER_B, INPUT);
  pinMode(ROTARY_ENCODER_BUTTON, INPUT_PULLUP);
  ROTARY_ENCODER_A_state = digitalRead(ROTARY_ENCODER_A);
  ROTARY_ENCODER_count = 0;
  ROTARY_ENCODER_BUTTON_state = BUTTON_NOT_PRESSED;

  // SCREEN init
  pinMode(SCREEN_RST, OUTPUT);
  pinMode(SCREEN_CE, OUTPUT);
  pinMode(SCREEN_DC, OUTPUT);
  pinMode(SCREEN_DIN, OUTPUT);
  pinMode(SCREEN_CLK, OUTPUT);

  // LIGHT SENSOR init
  Wire.begin();
  LIGHT_SENSOR.begin();
  LIGHT_SENSOR_show_mode = SHOW_EV;

  // variables init
  ISO_index = -1;
  ND_index = -1;
  aperture_index = -1;
  shutter_speed_index = -1;
  
  input_ISO = false;
  input_ND = false;
  use_ND = ACTIVATE_ND_SWITCH_state;
  
  blink_state = false;
  blink_duration = 750;
  blink_last = 0;

  // draw the initial screen
  u8g.firstPage();  
  do {
    draw();
  } while( u8g.nextPage() );

  // serial init for debugging
  Serial.begin(9600);
  Serial.println("Lightmeter is ready!");
}

void drawISO(void) {
  u8g.setFont(u8g_font_7x13);

  char iso[6];

  if ( ISO_index == -1 ) {
    sprintf(iso, "-");
  } else if ( 0 <= ISO_index && ISO_index < ISO_values_len ) {
    sprintf(iso, "%d", ISO_values[ISO_index]);
  }
  
  u8g.drawStr( 5, 13, iso);
}

void drawND(void) {
  u8g.setFont(u8g_font_7x13);

  char nd[5];

  if ( ND_index == -1 ) {
    sprintf(nd, "-"); 
  } else if ( 0 <= ND_index && ND_index < ND_values_len ) {
    sprintf(nd, "%d", ND_values[ND_index]);
  }
  
  u8g.drawStr( 50, 13, nd);
}

void drawEV_lux(void) {
  if ( LIGHT_SENSOR_show_mode == SHOW_EV ) {
    u8g.setFont(u8g_font_5x7);
    
    char ev[7];

    sprintf(ev, "EV %d", EV_100_rounded);

    u8g.drawStr( 27, 27, ev);
  } else if ( LIGHT_SENSOR_show_mode == SHOW_LUX ) {
    u8g.setFont(u8g_font_4x6);
    
    char lux_reading[9];
    
    sprintf(lux_reading, "%d lx", (int)lux);
    
    u8g.drawStr( 27, 27, lux_reading);
  }
}

void drawAperture(void) {
  u8g.setFont(u8g_font_7x13);
  
  u8g.drawStr( 15, 44, aperture_values[0]);
}

void drawShutterSpeed(void) {
  u8g.setFont(u8g_font_5x7);

  char shutter[8];
  sprintf(shutter, "%ss", shutter_speed_values[0]);
  
  u8g.drawStr( 46, 42, shutter);
}

void draw_layout(void) {
  // outer frame
  u8g.drawFrame(0, 0, 84, 48);

  // inner frame
  u8g.drawRFrame(24, 16, 36, 16, 6);

  // vertical lines
  u8g.drawLine(42, 0, 42, 16);
  u8g.drawLine(42, 32, 42, 48);

  // horizontal lines
  u8g.drawLine(0, 24, 24, 24);
  u8g.drawLine(60, 24, 84, 24);

  u8g.setFont(u8g_font_5x7);
  u8g.drawStr( 5, 23, "ISO");
  u8g.drawStr( 66, 23, "ND");
  u8g.drawStr( 62, 32, "TIME");

  u8g.setFont(u8g_font_10x20);
  u8g.drawStr( 4, 44, "F");
}  

void draw(void) {
  // constant layout
  draw_layout();

  // values
  if ( !input_ISO || blink_state ) {
    drawISO();  
  }

  if ( !input_ND || blink_state ) {
    drawND();
  }
  
  drawEV_lux();
  drawAperture();
  drawShutterSpeed();
}

void loop() {
  bool update_screen = false;
  
  if ( input_ISO || input_ND ) {
    unsigned long current_time = millis();
    
    if ( current_time - blink_last > blink_duration ) {
      blink_state = !blink_state;
      blink_last = current_time;

      update_screen = true;
    }
  }
  
  // read the button states
  int SET_ISO_BUTTON_state_tmp = digitalRead(SET_ISO_BUTTON);
  int SET_ND_BUTTON_state_tmp = digitalRead(SET_ND_BUTTON);
  int MEASURE_BUTTON_state_tmp = digitalRead(MEASURE_BUTTON);

  // read the switch state
  int ACTIVATE_ND_SWITCH_state_tmp = digitalRead(ACTIVATE_ND_SWITCH);

  // read the rotary encoder state
  int ROTARY_ENCODER_A_state_tmp = digitalRead(ROTARY_ENCODER_A);
  int ROTARY_ENCODER_BUTTON_state_tmp = digitalRead(ROTARY_ENCODER_BUTTON);

  // SET ISO BUTTON update
  if ( SET_ISO_BUTTON_state_tmp != SET_ISO_BUTTON_state ) {
    if ( SET_ISO_BUTTON_state_tmp == BUTTON_PRESSED ) {
      Serial.println("Set ISO button was pressed");

      input_ISO = !input_ISO;

      if ( input_ISO ) {
        blink_state = true;
        blink_last = millis();
        
        if ( input_ND ) {
          input_ND = !input_ND;
        }

        if ( ISO_index == -1 ) {
          ISO_index = 0;
        }
      }
    }

    update_screen = true;
    
    SET_ISO_BUTTON_state = SET_ISO_BUTTON_state_tmp;
  }

  // SET ND BUTTON update
  if ( use_ND && SET_ND_BUTTON_state_tmp != SET_ND_BUTTON_state ) {
    if ( SET_ND_BUTTON_state_tmp == BUTTON_PRESSED ) {
      Serial.println("Set ND button was pressed");

      input_ND = !input_ND;

      if ( input_ND ) {
        blink_state = true;
        blink_last = millis();
        
        if ( input_ISO ) {
          input_ISO = !input_ISO;
        }

        if ( ND_index == -1 ) {
          ND_index = 0;
        }
      } 
    }

    update_screen = true;
    
    SET_ND_BUTTON_state = SET_ND_BUTTON_state_tmp;
  }

  // ACTIVATE ND SWITCH update
  if ( ACTIVATE_ND_SWITCH_state_tmp != ACTIVATE_ND_SWITCH_state ) {
    if ( ACTIVATE_ND_SWITCH_state_tmp == SWITCH_ON ) {
      Serial.println("Activate ND switch is ON");
    }

    use_ND = !use_ND;

    if ( !use_ND ) {
      ND_index = -1;
      blink_state = false;
      input_ND = false;
      SET_ND_BUTTON_state = BUTTON_NOT_PRESSED;
    }

    update_screen = true;

    ACTIVATE_ND_SWITCH_state = ACTIVATE_ND_SWITCH_state_tmp;
  }

  // MEASURE BUTTON update
  if ( MEASURE_BUTTON_state_tmp != MEASURE_BUTTON_state ) {
    if ( MEASURE_BUTTON_state_tmp == TOUCH_BUTTON_PRESSED ) {
      Serial.println("Measure button was pressed");

      // LIGHT SENSOR update
      lux = LIGHT_SENSOR.readLightLevel();

      // no light
      if ( lux == 0.00f ) {
        EV_100 = -4.f;
      } else {
        EV_100 = (float)log((double)lux / 2.5f) / M_LN2;
      }
      
      EV_100_rounded = round(EV_100);

      Serial.print("EV = ");
      Serial.print(EV_100);
      Serial.print(", EV_rounded = ");
      Serial.print(EV_100_rounded);
      Serial.print(", lux = ");
      Serial.print(lux);
      Serial.println(" lx");

      
    }

    update_screen = true;
    
    MEASURE_BUTTON_state = MEASURE_BUTTON_state_tmp;
  }

  // ROTARY ENCODER update
  if ( ROTARY_ENCODER_A_state_tmp != ROTARY_ENCODER_A_state ) {
    if ( digitalRead(ROTARY_ENCODER_B) != ROTARY_ENCODER_A_state_tmp ) {
      Serial.println("+1");
      ROTARY_ENCODER_count++;

      if ( input_ISO ) {
        ISO_index = (int)fmin(ISO_index + 1, ISO_values_len - 1);
      } else if ( input_ND ) {
        ND_index = (int)fmin(ND_index + 1, ND_values_len - 1);
      }
    } else {
      Serial.println("-1");
      ROTARY_ENCODER_count--;

      if ( input_ISO ) {
        ISO_index = max(ISO_index - 1, 0);
      } else if ( input_ND ) {
        ND_index = max(ND_index - 1, 0);
      }
    }
    
    Serial.print("Position: ");
    Serial.println(ROTARY_ENCODER_count);

    update_screen = true;
    
    ROTARY_ENCODER_A_state = ROTARY_ENCODER_A_state_tmp;
  }

  if ( ROTARY_ENCODER_BUTTON_state_tmp != ROTARY_ENCODER_BUTTON_state ) {
    if ( ROTARY_ENCODER_BUTTON_state_tmp == BUTTON_PRESSED ) {
      Serial.println("Rotary encoder button was pressed");

      LIGHT_SENSOR_show_mode = 1 - LIGHT_SENSOR_show_mode;
    }

    update_screen = true;
    
    ROTARY_ENCODER_BUTTON_state = ROTARY_ENCODER_BUTTON_state_tmp;
  }

  // update the screen only if we need to
  if ( update_screen ) {
    u8g.firstPage();  
    do {
      draw();
    } while( u8g.nextPage() );
  }
}
