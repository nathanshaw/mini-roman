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
  for (int i = 0; i < sizeof(observed) / sizeof(int); i++) {
    // set is_presence to true if someone is detected
    if (observed[i] > 0) {
      is_presence = true;
    }
    // check to see if a new presence was detected
    if (observed[i] > 0 && last_observed[i] == 0) {
      // Serial.println("new presence detected in controlMotor");
      if (i == 0) {
        // if the visitor is looking into the front portal
        target_motor_speed = motor_max_speed * subversive;
        motor_direction = subversive;
      } else if ( i == 1) {
        // if the visitor is looking into the left portal
        target_motor_speed = motor_max_speed * subversive;
        motor_direction = subversive;
      } else if ( i == 2) {
        // if the visitor is looking into the right portal
        target_motor_speed = motor_max_speed * -1 * subversive;
        motor_direction = subversive * -1;
      }
    }
  }
  constrainMotorSpeed();
  adjustMotorSpeed();
}
/*
  void controlMotor() {
  boolean is_presence = false;
  Serial.print(millis());
  Serial.print(" ");
  Serial.print(last_motor_on);
  Serial.print(" ");
  Serial.print(motor_on_time);
  if (millis() > last_motor_on + motor_on_time) {
    for (int i = 0; i < sizeof(observed) / sizeof(int); i++) {
      // set is_presence to true if someone is detected
      if (observed[i] == 1) {
        is_presence = true;
      }
      // check to see if a new presence was detected
      if (observed[i] > 0 && last_observed[i] == 0) {
        Serial.println("new presence detected in controlMotor");
        if (i == 0) {
          // if the visitor is looking into the front portal
          target_motor_speed = 255 * subversive;
          motor_direction = subversive;
        } else if ( i == 1) {
          // if the visitor is looking into the left portal
          target_motor_speed = 255 * subversive;
          motor_direction = subversive;
        } else if ( i == 2) {
          // if the visitor is looking into the right portal
          target_motor_speed = -255 * subversive;
          motor_direction = subversive * -1;
        }
      }
    }
    //last_motor_on = millis();
    if (is_presence == false) {
      target_motor_speed = 0;
      motor_direction = 0;
    }
    constrainMotorSpeed();
    adjustMotorSpeed();
  }
  // if no presence was found throughout the loop and enough time has passed to turn motor off
  }
*/

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
      analogWrite(motor_back_pin, motor_speed);
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
  /*
  else if (target_motor_speed > forward_motor_threshold && (motor_speed < forward_motor_threshold && motor_speed > reverse_motor_threshold)) {
    motor_speed = forward_motor_threshold;
  }
  else if (target_motor_speed < reverse_motor_threshold && (motor_speed > reverse_motor_threshold && motor_speed < forward_motor_threshold)) {
    motor_speed = reverse_motor_threshold;
  }
  */
}
