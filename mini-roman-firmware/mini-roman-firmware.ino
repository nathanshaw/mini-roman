/* Programmed in the fall of 2017 by Nathan Villicana-Shaw

  There should be an IR sensor on each of the four sides of the device
  Sensor 0 is the front, 1 is the back, 2 is left while 3 is right

  The front and back sensors will activate the belt as well as turn on
  their own channel of neo pixels.

  The side IR's will turn on their own channels of neo pixels

  PINOUT
  --------------------
  Smart Glass -
  Front           D2
  Left            D3
  Right           D4

  Neo Pixels - Powered via Arduino
  Motor           D5
  Floor           D6
  Roof            D7

  DC Motor Control - to breakout board
  Forward         D8
  Backward        D9

  IR Sensors - to sense someones presence
  Front (motor)   A0
  Left of motor   A1
  Right of motor  A2

  Controls (hidden on bottom of unit:
   Button1        D11
    to reset ambiant light levels?
   Button2        D12
   Pots           A3 // control the smart glass
                  A4 // moves the motor
                  A5
   ControlBox     D10 // this is true when the control box is attached

  Reset Button    D13

  TODO
  -----
  Make it so the motors run faster if there are several people around
  add a button to reset the ambiant lighting

*/
#include <Adafruit_NeoPixel.h>
/////////// Program State ///////////////////
#define PASSIVE 0
#define ACTIVE 1
#define TEST 2

int state = 0;
unsigned long last_state_change = 0;

////////// DEBUG and Print settings //////////
const bool p_state = false;
const bool p_motor_direction = false;
const bool p_motor_speed = false;
const bool p_IR = true;
const bool p_observed_thresh = false;
const bool p_observed = true;
const bool p_l_observed = true;
const bool p_neo_deltas = false;
const bool p_current_colors = false;
const bool p_target_colors = false;
const bool p_glass_states = true;
const bool p_time = false;

const bool p_text = true;

////////// Smart Glass //////////////////
const int glass_pins[] = {2, 3, 4};
int glass_states[] = {1, 1, 1};
const int attention_span = 25000; // how long the bot will continue to operate after a visitor leaves

///////// IR Sensors ////////////////////
const int ir_pins[] = {
  A0, A1, A2
};
int ir_values[] = {
  0, 0, 0
};
int last_ir_values[] = {
  0, 0, 0
};
// this list keeps track of if the mini-roman detects a presence on its four sides
int observed[] = {
  0, 0, 0
};
int last_observed[] = {
  0, 0, 0
};
unsigned long time_last_observed[] = {0, 0, 0};
const int observed_thresh_base = 90;
int observed_thresh[] = {400, 400, 400};
unsigned long last_enviro_reset = 0;
const float smoothing_amount = 0.8; // the larger the value the greater the smoothing effect

/////////// External Controller ///////////////////////
// is the controller box connected?
boolean controllerBox = false;
// buttons and pots from external controller
const int reset_button = 13;
const int controller_pin = 10;
const int but_pins[] = {11, 12};
boolean reset_button_state = false;
boolean last_button1_state = false;
boolean last_button2_state = false;
boolean last_reset_button_state = false;
boolean button1_state = false;
boolean button2_state = false;
// pots
const int pot_pins[] = {A3, A4, A5};
int pot_values[] = {0, 0, 0};
int last_pot_values[] =  {0, 0, 0};

///////////////// time //////////////////////
unsigned long now = 0;
// IR polling
unsigned long last_ir_poll = 0;
const unsigned long ir_polling_delay = 90;
// system printing
unsigned long last_system_print = 0;
const unsigned long system_print_delay = 90;
// 1000*60*60
const unsigned long HOUR = 3600000;
const unsigned long DAY = 86400000;

/////////////// the motor //////////////////////
const int motor_forward_pin = 8;
const int motor_back_pin = 9;

int motor_direction = 0; // 0 is off, 1 is forward, -1 is backward
int last_motor_direction = 0;

const int motor_on_time = 15000; // in ms
unsigned long last_motor_on = 0; // the last time the motor was on
int motor_speed = 0;
int target_motor_speed = 0;

const int reverse_motor_threshold = -85;
const int forward_motor_threshold = 85;
const int motor_max_speed = 150;
const int motor_min_speed = 85;

///////////////////// NEO PIXELS //////////////////////////////////////////
// Parameter 1 = number of pixels in strip
// Parameter 2 = Arduino pin number (most are valid)
// Parameter 3 = pixel type flags, add together as needed:
//   NEO_KHZ800  800 KHz bitstream (most NeoPixel products w/WS2812 LEDs)
//   NEO_KHZ400  400 KHz (classic 'v1' (not v2) FLORA pixels, WS2811 drivers)
//   NEO_GRB     Pixels are wired for GRB bitstream (most NeoPixel products)
//   NEO_RGB     Pixels are wired for RGB bitstream (v1 FLORA pixels, not v2)
//   NEO_RGBW    Pixels are wired for RGBW bitstream (NeoPixel RGBW products)
/*
  Adafruit_NeoPixel lights = Adafruit_NeoPixel(16, 5);//, NEO_GRB + NEO_KHZ800); // MOTOR 16
  Adafruit_NeoPixel lights = Adafruit_NeoPixel(17 * 4, 6);//, 6, NEO_GRB + NEO_KHZ800); // floor 17*4
  Adafruit_NeoPixel lights = Adafruit_NeoPixel(10 * 4, 7);//, 7, NEO_GRB + NEO_KHZ800); // roof 10*4
*/
Adafruit_NeoPixel lights = Adafruit_NeoPixel(68 + 16 + 40, 5); //, 7, NEO_GRB + NEO_KHZ800); // roof 10*4

int neo_colors[9] = {
  int(random(120.0)),
  int(random(120.0)),
  int(random(120.0)),
  int(random(120.0)),
  int(random(120.0)),
  int(random(120.0)),
  int(random(120.0)),
  int(random(120.0)),
  int(random(120.0)),
};
int target_neo_colors[9] = {120, 120, 120, 120, 120, 120, 120, 120, 120}; // motor, floor, roof
int base_np_delta = 1;
int neo_deltas[9] = {1, 1, 1, 1, 1, 1, 1, 1, 1};
unsigned long last_updated_lights = 0;
unsigned long light_update_delay = 50;

////////////////////// SETUP ///////////////////////////////
void setupNeoPixels() {
  // NEO PIXELS
  if (p_text == true) {
    Serial.print("Setting up Neo Pixels");
  };
  lights.begin();
  lights.setBrightness(120); // 1 - 255
  lights.show();
  if (p_text == true) {
    Serial.println(" DONE");
  };
}


void setupTests() {
  // test motor, glass, lights
  // Serial.println("testing motor...");
  // testMotor(10000);
  if (p_text == true) {
    Serial.println("testing glass");
  };
  testGlass(200);
  testGlass(1000);
  if (p_text == true) {
    Serial.println("testing lights");
  };
  for (int i = 0; i < 3; i++) {
    testNeoP(120, false);
  };
  if (p_text == true) {
    Serial.println("setupTests finished");
  };
}

////////////////////////// CONTROLS ////////////////////////////

void checkResetButton() {
  /*
     This function both reads the state of the reset button
     and resets the ambiant light if the reset button has been pressed
  */
  last_reset_button_state = reset_button_state;
  reset_button_state = digitalRead(reset_button);
  if (reset_button_state == true && last_reset_button_state == false) {
    setObservedThresholds(2000);
    if (p_text == true) {
      Serial.println("Reset button pressed, resetting ambiant light levels...");
    };
    last_enviro_reset = now;
  }
}


void pollControllerBox() {
  last_button1_state = button1_state;
  button1_state = digitalRead(but_pins[0]);
  last_button2_state = button2_state;
  button2_state = digitalRead(but_pins[1]);
  for (int i = 0; i < 3; i++) {
    last_pot_values[i] = pot_values[i];
    pot_values[i] = analogRead(pot_pins[i]);
  }
}


void controllerBoxControl() {
  // POT 0 controls the Controller Box
  if (pot_values[0] < 200) {
    glass_states[0] = 0;
    glass_states[1] = 0;
    glass_states[2] = 0;
  } else if (pot_values[0] < 400) {
    glass_states[0] = 1;
    glass_states[1] = 0;
    glass_states[2] = 0;
  } else if (pot_values[0] < 600) {
    glass_states[0] = 0;
    glass_states[1] = 1;
    glass_states[2] = 0;
  } else if (pot_values[0] < 800) {
    glass_states[0] = 0;
    glass_states[1] = 0;
    glass_states[2] = 1;
  } else {
    glass_states[0] = 1;
    glass_states[1] = 1;
    glass_states[2] = 1;
  }

  analogWrite(motor_forward_pin, pot_values[1]);
}

///////////////////////// IRs ///////////////////////////////////
void detectVisitor() {
  /*
     This funciton attempts to detect the presence of visitors in the gallery
  */
  int presence = 0;
  // for each IR sensor....
  for (int i = 0; i < sizeof(ir_pins) / sizeof(int); i++) {
    // if x time has passed since presence was lost and the glass is on
    if (now > time_last_observed[i] + attention_span && glass_states[i] == 0) {
      // make the glass opaque
      glass_states[i] = 1;
      updateSmartGlass();
      // announce that the attention was lost...
      if (p_text == true) {
        Serial.print("attention lost for obs : ");
      };
      if (p_text == true) {
        Serial.print(i);
      };
      if (p_text == true) {
        Serial.print(" : ");
      };
      if (p_text == true) {
        Serial.println(now);
      };
    }
    // if the current IR value is greater than the threshold
    if (ir_values[i] > observed_thresh[i]) {
      last_observed[i] = observed[i]; // update last_observed value
      observed[i] = 1; // if the value is greater than thresh we detect someone
      time_last_observed[i] = now; // make the time_last_observed equal to now...
    }
    else {
      // even if noone is detected we need to updated last_observed as well as set observed to 0
      last_observed[i] = observed[i];
      observed[i] = 0;
    }

    // if someone leaves
    if (observed[i] == false && last_observed[i] == true) {
      if (p_text == true) {
        Serial.print("someone left ");
      };
      if (p_text == true) {
        Serial.print(i);
      };
      if (p_text == true) {
        Serial.print(" : ");
      };
      if (p_text == true) {
        Serial.println(time_last_observed[i]);
      };
      // make the glass see through..
      glass_states[i] = 0;
      updateSmartGlass();
    }
    else if (observed[i] == true && last_observed[i] == false) {
      if (p_text == true) {
        Serial.print("New visitor detected : ");
      };
      if (p_text == true) {
        Serial.print(i);
      };
      if (p_text == true) {
        Serial.print(" : ");
      };
      if (p_text == true) {
        Serial.println(time_last_observed[i]);
      };
      glass_states[i] = 0; // make the glass tranparent
      updateSmartGlass();
      state = ACTIVE;
      last_state_change = now;
    }
  }
}


void setObservedThresholds(int ms) {
  /*
     Goes through each of the IR sensors and sets the ambiant light
     Will take the average of all readings over a period of "ms" milliseconds
     after each of the IR sensors is calibrated the function will exit
  */
  for (int i = 0; i <  sizeof(ir_pins) / sizeof(int); i++) {
    observed_thresh[i] = getAmbiantLight(ir_pins[i], ms) + observed_thresh_base;
  }
}


int getAmbiantLight(int pin, int polling_time) {
  /*
    This function polls a specific IR sensor for polling_time ms
    and returns the average reading over the period of time

    This can be used to set the ambiant light levels for each IR sensor
    incresing the accuracy of the sensors.
  */
  unsigned long start_millis = millis();
  int num_data_points = 0;
  unsigned long running_total = 0;
  while (millis() - start_millis < polling_time) {
    running_total += analogRead(pin);
    num_data_points++;
    delay(4);
  }
  int average = int(running_total / num_data_points);
  if (p_text == true) {
    Serial.print(" IR sensor ");
  };
  if (p_text == true) {
    Serial.print(pin);
  };
  if (p_text == true) {
    Serial.print(" detected ambiant light value of ");
  };
  if (p_text == true) {
    Serial.print(average);
  };
  if (p_text == true) {
    Serial.print(" over a period of ");
  };
  if (p_text == true) {
    Serial.print(polling_time);
  };
  if (p_text == true) {
    Serial.println("ms");
  };
  return average;
}

void pollIRs(boolean smooth) {
  /* read each of the IR sensors, and then apply a simple Low Pass, if the
      smooth boolean is set to true.
  */
  if (now > last_ir_poll + ir_polling_delay) {
    for (int i = 0; i < sizeof(ir_pins) / sizeof(int); i++) {
      last_ir_values[i] = ir_values[i];
      if (smooth == true) {
        ir_values[i] = int((analogRead(ir_pins[i]) * (1.0 - smoothing_amount) + last_ir_values[i]) * smoothing_amount);
      }
      else {
        ir_values[i] = analogRead(ir_pins[i]);
        if (ir_values[i] < 1) {
          ir_values[i] = last_ir_values[i];
        }
      }
    }
    last_ir_poll = now;
    detectVisitor(); // see if a visitor is present
  }
}


void hourlyAmbiantLightReset() {
  /*
      check to see if it has been over an hour sense enviro_reset
      if it has then run the setAmbiantLightLevels() function, if not then
      simply exit
      should be called once every few minutes or so
  */
  // TODO needs to check to ensure that no visitor is currently being detected
  if (now > last_enviro_reset + HOUR) {
    setObservedThresholds(1000);
    if (p_text == true) {
      Serial.println("An Hour has passed sense the last enviro reset: reset light levels");
    };
    last_enviro_reset = now;
  }
}

void(* resetFunc) (void) = 0; //declare reset function @ address 0

////////////// GLASS /////////////

void updateSmartGlass() {
  for (int i = 0; i < sizeof(glass_states) / sizeof(int); i++) {
    digitalWrite(glass_pins[i], glass_states[i]);
  }
}

void testGlass(int t) {
  for (int i = 0; i < 3; i++) {
    digitalWrite(glass_pins[i], LOW);
    delay(t);
    digitalWrite(glass_pins[i], HIGH);
    delay(t);
  }
}

void setup() {
  randomSeed(analogRead(A5));
  Serial.begin(57600);
  if (p_text == true) {
    Serial.println("starting boot sequence");
  };

  setupNeoPixels();
  // pinMode for IR sensors, Motor Control, and Smart Glass (via relays)
  pinMode(ir_pins[0], INPUT); pinMode(ir_pins[1], INPUT); pinMode(ir_pins[2], INPUT);
  // motor
  pinMode(motor_back_pin, OUTPUT); pinMode(motor_forward_pin, OUTPUT);
  // smart glass
  pinMode(glass_pins[0], OUTPUT); pinMode(glass_pins[1], OUTPUT); pinMode(glass_pins[2], OUTPUT);
  setObservedThresholds(500);

  //////////////////// TESTs ///////////////////////////
  setupTests();

  // check to see if the controller box is attached
  if (digitalRead(controller_pin) == true) {
    controllerBox = true;
    if (p_text == true) {
      Serial.println("Controller Box Detected");
    };
  }
  else {
    if (p_text == true) {
      Serial.println("No External Controller Detected");
    };
  }

  if (p_text == true) {
    Serial.println("SETUP LOOP FINISHED");
  };
  if (p_text == true) {
    Serial.println("---------------------------------");
  };
}

void loop() {
  if (state == PASSIVE) {
    now = millis();
    /////////// LIGHTS /////////////
    updateLights(); // if noone is detected the  cycle theough different colors if someone is detected immedietly switch to viewing color.

    ////////// SENSORS /////////////
    pollIRs(true); // true is for smoothing // have the IR sensors read everything around them
    // checkResetButton(); // check to see if the reset button has been pressed

    /////////// MOTORS /////////////
    //controlMotor(); // move the motors if someone is detected

    ////// CONTROL BOX /////////////
    /*
      if (controllerBox == true) {
      pollControllerBox();
      }; // if the control box is detected, poll the control box...
    */
    ////// MAINTAINANCE ////////////
    // hourlyAmbiantLightReset(); // once an hour reset the ambiant light levels

    /////// DEBUG PRINTING /////////
    printSystemState(); // print, print, print

    // If it has been over a day... reset the system
    if (millis() > DAY) {
      resetFunc();
    }
  } else if (state == ACTIVE) {
    // MOVE the color of the lights slowly to the viewing color
    // then start revving up the motor let the motor go through about a single rotation then
    // still poll the IR sensors, use the readings to turn on the other viewing ports... dont allow the viewing ports to turn off in thi mode
    // the motor should be moving during all of this mode...

    //////////////////////// cycle the light to a white
    if (p_text == true) {
      Serial.println("-------------------------------");
    };
    if (p_text == true) {
      Serial.println("ENTERING INTO ACTIVE STATE");
    };
    setNeoDeltas(1); // set neo Deltas to 1 so they climb up to 255
    while (neo_colors[0] < 255 && neo_colors[1] < 255 && neo_colors[2] < 255) {
      moveToWhite();
      pollIRs(true); // this might cause trouble...
      now = millis();
      printSystemState();
      delay(44);
    }

    /////////////////////// Turn on all the glass
    if (p_text == true) {
      Serial.println("Making glass transparent");
    };
    glass_states[0] = 0; // instead I should poll the IR sensors and
    glass_states[1] = 0;
    glass_states[2] = 0;
    updateSmartGlass();
    now = millis();
    printSystemState();
    /////////////////////// Get the motor moving
    if (p_text == true) {
      Serial.println("Get the motor moving");
    };
    analogWrite(motor_back_pin, 0);
    for (int i = motor_min_speed; i < motor_max_speed; i++) {
      analogWrite(motor_forward_pin, i);
      delay(50);
    }
    if (p_text == true) {
      Serial.println("Motor at full speed");
    };
    // basically wait for 30 seconds while also polling the IR sensors
    for (int i = 0; i < 300; i++) {
      pollIRs(true);
      now = millis();
      printSystemState();
      delay(100);
    }
    if (p_text == true) {
      Serial.println("Entering into the possible extended motor rotation period");
    };
    bool test = true;
    while (test == true) {
      pollIRs(true);
      now = millis();
      printSystemState();

      if (observed[0] + observed[1] + observed[2] == 0) {
        test = false;
      } else {
        if (p_text == true) {
          Serial.println("They still looking... extending motor by a second");
          delay(4000);
        }
      }
    }

    if (p_text == true) {
      Serial.println("Rev down the motor");
    };
    for (int i = motor_max_speed; i > motor_min_speed; i--) {
      analogWrite(motor_forward_pin, i);
      delay(50);
    }
    if (p_text == true) {
      Serial.println("Motor off");
    };
    analogWrite(motor_forward_pin, 0);
    delay(3000);

    last_state_change = now;
    state = PASSIVE;
    if (p_text == true) {
      Serial.println("Blocking out glass again");
    };
    glass_states[0] = 1; // instead I should poll the IR sensors and
    glass_states[1] = 1;
    glass_states[2] = 1;
    updateSmartGlass();
    if (p_text == true) {
      Serial.println("LEAVING ACTIVE STATE");
    };
    if (p_text == true) {
      Serial.println("-------------------------------");
    };
  } else if (state == TEST) {
    pollControllerBox();
    controllerBoxControl();
  }
}

