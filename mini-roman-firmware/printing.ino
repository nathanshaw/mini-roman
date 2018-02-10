///////////////////// COMMUNICATION ///////////////////////////////

void printNeoColors() {
  Serial.print(" curernt Cs : ");
  for (int i = 0; i < sizeof(neo_colors) / sizeof(int); i++) {
    Serial.print(neo_colors[i]);
    Serial.print(" ");
  }

}

void printSystemState() {
  now = millis();
  if (now > last_system_print + system_print_delay) {
    /* This function is intended to print out all
       relivent information about the mini-roman
    */
    if (p_state == true) {
      if (state == ACTIVE) {
        Serial.print("A ");
      } else if (state == PASSIVE) {
        Serial.print("P ");
      }
    }
    // print out the motor state
    if (p_motor_direction) {
      Serial.print("motor: ");
      Serial.print(motor_direction);
      Serial.print(" ");
    }
    if (p_motor_speed == true) {
      Serial.print("speed: ");
      Serial.print(motor_speed);
      Serial.print("/");
      Serial.print(target_motor_speed);
    }
    if (p_IR == true) {
      // print out the values of the IR sensors
      Serial.print(" IR: ");
      for (int i = 0; i < sizeof(ir_pins) / sizeof(int); i++) {
        Serial.print(ir_values[i]);
        Serial.print(" ");
        if (ir_values[i] < 10) {
          Serial.print(" ");
        }
        if (ir_values[i] < 100) {
          Serial.print(" ");
        }
      }
      Serial.print("");
    }
    if (p_observed_thresh == true) {
      Serial.print("obs_thresh: ");
      for (int i = 0; i < sizeof(ir_pins) / sizeof(int); i++) {
        Serial.print(observed_thresh[i]);
        Serial.print(" ");
      }
    }
    if (p_neo_deltas == true) {
      Serial.print(" neo deltas : ");
      for (int i = 0; i < sizeof(neo_deltas) / sizeof(int); i++) {
        Serial.print(neo_deltas[i]);
        Serial.print(" ");
      }
    }
    if (p_current_colors == true) {
      printNeoColors();
    }
    if (p_target_colors == true) {
      Serial.print("target Cs : ");
      for (int i = 0; i < sizeof(target_neo_colors) / sizeof(int); i++) {
        Serial.print(target_neo_colors[i]);
        Serial.print(" ");
      }
    }
    if (p_observed == true) {
      // print out if presence is detected
      Serial.print("Observed : ");
      for (int i = 0; i < sizeof(observed) / sizeof(int); i++) {
        Serial.print(observed[i]*100);
        Serial.print(" ");
      }

    }
    if (p_l_observed == true) {
      // print out if presence is detected
      Serial.print("LO: ");
      for (int i = 0; i < sizeof(last_observed) / sizeof(int); i++) {
        Serial.print(last_observed[i]*100);
        Serial.print(" ");
      }
    }
    if (p_glass_states == true) {
      Serial.print("Glass: ");
      for (int i = 0; i < sizeof(glass_states) / sizeof(int); i++) {
        Serial.print(glass_states[i]*100);
        Serial.print(" ");
      }
    }

    // external controller
    if (controllerBox == true) {
      Serial.print("CB:");
      Serial.print(button1_state);
      Serial.print(" ");
      Serial.print(button2_state);
      Serial.print(" ");
      Serial.print(pot_values[0]);
      Serial.print(" ");
      Serial.print(pot_values[1]);
      Serial.print(" ");
      Serial.print(pot_values[2]);
    }

    // print time
    if (p_time == true) {
      Serial.print(now);
    }
    // DONE
    Serial.println("");
    last_system_print = millis();
  }
}

