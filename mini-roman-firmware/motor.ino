//////////////////////// MOTOR ////////////////////////////////
void testMotor(int t) {
  analogWrite(motor_forward_pin, 255);
  delay(t);
  analogWrite(motor_forward_pin, 120);
  delay(t);
  analogWrite(motor_forward_pin, 0);
  delay(t);
  analogWrite(motor_back_pin, 50);
  delay(t);
  analogWrite(motor_back_pin, 255);
  delay(t);
  analogWrite(motor_back_pin, 125);
  delay(t);
  analogWrite(motor_back_pin, 0);
}

void controlMotor() {
  boolean is_presence = false;
  if (now > last_motor_on + motor_on_time) {
    for (int i = 0; i < sizeof(observed) / sizeof(int); i++) {
      // set is_presence to true if someone is detected
      if (observed[i] == 1) {
        is_presence = true;
      }
      // check to see if a new presence was detected
      if (observed[0] > 0 && last_observed[0] == 0) {
        target_motor_speed = 255;
        motor_direction = 1;
        last_motor_on = millis();
      }
      else if (observed[i] > 0 && last_observed[i] == 0) {
        target_motor_speed = -255;
        motor_direction = -1;
        last_motor_on = millis();
      }
    }
  }
  // if no presence was found throughout the loop and enough time has passed to turn motor off
  if (is_presence == false && now > last_motor_on + motor_on_time) {
    target_motor_speed = 0;
    motor_direction = 0;
  }
  constrainMotorSpeed();
  adjustMotorSpeed();
}

void adjustMotorSpeed() {
  // now actually rev the motor up to the correct speed
  if (motor_speed < target_motor_speed) {
    if (motor_speed >= 0) {
      // increment the motor speed by one
      motor_speed = motor_speed + 1;
      analogWrite(motor_back_pin, 0);
      analogWrite(motor_forward_pin, motor_speed);
    }
    else {
      // if the motor is currently moving backwards then
      motor_speed = motor_speed + 1;
      analogWrite(motor_forward_pin, 0);
      analogWrite(motor_back_pin, motor_speed * -1);
    }
  }
  else if (motor_speed > target_motor_speed) {
    if (motor_speed >= 0) {
      // increment the motor speed by one
      motor_speed = motor_speed - 1;
      analogWrite(motor_back_pin, 0);
      analogWrite(motor_forward_pin, motor_speed);
    }
    else {
      // if the motor is currently moving backwards then
      motor_speed = motor_speed - 1;
      analogWrite(motor_forward_pin, 0);
      analogWrite(motor_back_pin, motor_speed * -1);
    }
  }
  if (motor_speed == target_motor_speed && target_motor_speed != 0) {
    Serial.println("motor speed equals target motor speed, maintaining for 30 seconds");
    digitalWrite(glass_pins[0], LOW);
    digitalWrite(glass_pins[1], LOW);
    digitalWrite(glass_pins[2], LOW);
    delay(300);
    digitalWrite(glass_pins[0], glass_states[0]);
    digitalWrite(glass_pins[1], glass_states[1]);
    digitalWrite(glass_pins[2], glass_states[2]);
    target_motor_speed = 0;
  }
}

void constrainMotorSpeed() {
  // make sure that motor_speed never rests between 50 and -50
  if (target_motor_speed == 0 &&
      ((motor_speed < forward_motor_threshold && motor_speed > 0)
       || (motor_speed > reverse_motor_threshold && motor_speed < 0))) {
    motor_speed = 0;
    analogWrite(motor_forward_pin, 0);
    analogWrite(motor_back_pin, 0);
  }
  else if (target_motor_speed > forward_motor_threshold && (motor_speed < forward_motor_threshold && motor_speed > reverse_motor_threshold)) {
    motor_speed = forward_motor_threshold;
  }
  else if (target_motor_speed < reverse_motor_threshold && (motor_speed > reverse_motor_threshold && motor_speed < forward_motor_threshold)) {
    motor_speed = reverse_motor_threshold;
  }
}
