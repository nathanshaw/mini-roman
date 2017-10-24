void detectVisitor() {
  int presence = 0;
  for (int i = 0; i < sizeof(ir_pins)/sizeof(int); i++) {
    presence = ir_values[i] - ambiant_light_levels[i];
    // Serial.print("presence is : ");
    // Serial.println(presence);
    if (presence > presence_threshold) {
      // Serial.print("PRESENCE DETECTED AT IR #");
      // Serial.print(i);
      last_observed[i] = observed[i];
      observed[i] = 1;
    } 
    else {
      last_observed[i] = observed[i];
      observed[i] = 0; 
    }
  }
}

void setAmbiantLightLevels() {
  for (int i = 0; i <  sizeof(ir_pins)/sizeof(int); i++) {
    
    ambiant_light_levels[i] = getAmbiantLight(ir_pins[i], 1000);  
    setColor(neop_motor.Color(random(255), random(255), random(255)));  
    delay(100); 
    setColor(neop_motor.Color(0, 0, 0));  

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
  while(millis() - start_millis < polling_time) {
    running_total += analogRead(pin);
    num_data_points++;
    delay(10);
  }
  int average = int(running_total/num_data_points);
  Serial.print(" IR sensor ");
  Serial.print(pin);
  Serial.print(" detected ambiant light value of ");
  Serial.print(average);
  Serial.print(" over a period of ");
  Serial.print(polling_time);
  Serial.println("ms");
  return average;
}

void pollIRs() {
  for (int i = 0; i < sizeof(ir_pins)/sizeof(int); i++) {
    last_ir_values[i] = ir_values[i];
    ir_values[i] = int((analogRead(ir_pins[i]) + last_ir_values[i])*0.5);
  } 
  last_ir_poll = millis();
}

