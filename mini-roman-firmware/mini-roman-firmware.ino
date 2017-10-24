/* This code is for reading a potentiometer connected to Analog pin 0
 * Programmed in the fall of 2017 by Nathan Villicana-Shaw
 
 There should be an IR sensor on each of the four sides of the device
 Sensor 0 is the front, 1 is the back, 2 is left while 3 is right
 
 The front and back sensors will activate the belt as well as turn on
 their own channel of neo pixels.
 
 The side IR's will turn on their own channels of neo pixels
 
 TODO
 -----
 
 have program recheck the ambiant light every hour
 have the program check past IR states along with current IR states to 
 determine if there is a presence
 Add in all the code needed for the NEO pixels
 Make it so the motors run faster if there are several people around
 
 add a button to reset the ambiant lighting
 
 */
 #include <Adafruit_NeoPixel.h>
 
 // neopixels
 const int neo_pins[] = {2, 3, 4, 7};
 // Parameter 1 = number of pixels in strip
// Parameter 2 = Arduino pin number (most are valid)
// Parameter 3 = pixel type flags, add together as needed:
//   NEO_KHZ800  800 KHz bitstream (most NeoPixel products w/WS2812 LEDs)
//   NEO_KHZ400  400 KHz (classic 'v1' (not v2) FLORA pixels, WS2811 drivers)
//   NEO_GRB     Pixels are wired for GRB bitstream (most NeoPixel products)
//   NEO_RGB     Pixels are wired for RGB bitstream (v1 FLORA pixels, not v2)
//   NEO_RGBW    Pixels are wired for RGBW bitstream (NeoPixel RGBW products)
Adafruit_NeoPixel neop_motor = Adafruit_NeoPixel(16, neo_pins[0], NEO_GRB + NEO_KHZ800);

// the feedback RGB led
const int led_pins[] = {
  9, 10, 11};

// button for resetting the IR ambiant light
const int button = 8;
boolean last_button_state = 0;
boolean current_button_state = 0;

const int ir_pins[] = {
  A0, A1, A2, A3};   
int ir_values[] = {
  0, 0, 0, 0};  
int last_ir_values[] = {
  0, 0, 0, 0};
int ambiant_light_levels[] = {
  0, 0, 0, 0};
// this list keeps track of if the mini-roman detects a presence on its four sides
int observed[] = {
  0, 0, 0, 0};
int last_observed[] = {
  0, 0, 0, 0};

unsigned long now = 0;
unsigned long last_ir_poll = 0;
unsigned long last_system_print = 0;
const unsigned long ir_polling_delay = 50;
const unsigned long system_print_delay = 250;

const int presence_threshold = 200;

// the motor
const int motor_forward_pin = 5;
const int motor_back_pin = 6;
int motor_direction = 0; // 0 is off, 1 is forward, -1 is backward
int last_motor_direction = 0;
const int motor_on_time = 5000;
unsigned long last_motor_on = 0;
int motor_speed = 0;
int target_motor_speed = 0;

int reverse_motor_threshold = -50;
int forward_motor_threshold = 50;

void setup() {
  for (int i = 0; i <  sizeof(led_pins)/sizeof(int); i++) {
    pinMode(led_pins[i], OUTPUT);
  }
  setColor(neop_motor.Color(0, 0, 255)); 
  // neo pixels
  neop_motor.begin();
  setLed(0, 0, 255); // turn LED to blue for the setup
  Serial.begin(57600);
  pinMode(ir_pins[0], INPUT);  
  pinMode(ir_pins[1], INPUT);
  pinMode(ir_pins[2], INPUT);
  pinMode(ir_pins[3], INPUT);
  pinMode(motor_back_pin, OUTPUT);
  pinMode(motor_forward_pin, OUTPUT);
  delay(100);
  setAmbiantLightLevels();
    setLed(0, 0, 0); // turn led off after boot
}

void printSystemState() {
  // print out the motor state
  Serial.print("motor direction : ");
  Serial.print(motor_direction);
  Serial.print(" ");
  Serial.print("speed : ");
  Serial.print(motor_speed);
  Serial.print("   ");

  // print out the values of the IR sensors
  Serial.print("IR : ");
  for (int i = 0; i < sizeof(ir_pins)/sizeof(int); i++) {
    Serial.print(ir_values[i]);
    Serial.print(" ");
  } 

  // print out if presence is detected
  Serial.print(" Observed : ");
  for (int i = 0; i < sizeof(observed)/sizeof(int); i++) {
    Serial.print(observed[i]);
  }
  Serial.println("");
  last_system_print = millis();
}

void setLed(int red, int green, int blue) {
  analogWrite(led_pins[0], red); 
  analogWrite(led_pins[1], green); 
  analogWrite(led_pins[2], blue);
}

void loop() {
  now = millis();
  if (now > last_ir_poll + ir_polling_delay) {
    pollIRs();
    detectVisitor();
    controlMotor();
  }
  if (now > last_system_print + system_print_delay) {
    printSystemState();
  }
  // check button
  last_button_state = current_button_state;
  current_button_state = digitalRead(button);
  if (digitalRead(button) == true && last_button_state == false) {
    setAmbiantLightLevels();
    Serial.println("Button pressed, resetting ambian light levels...");
  } 
}
