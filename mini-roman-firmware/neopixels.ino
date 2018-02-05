///////////////////// NEO PIXELS //////////////////////////////
void updateLights() {
  int t_colors = 0;
  // if no one is detected then the lights cycle through different colors
  if (now > last_updated_lights + light_update_delay) {
    last_updated_lights = now;
    if (observed[0] + observed[1] + observed[2] == 0) {
      // update neoP color values with delta
      // int t_colors = 0;
      for (int i = 0; i < 9; i++) {
        // if the color is within 1 point of the target color
        if (neo_colors[i] > target_neo_colors[i] - 1 && neo_colors[i] < target_neo_colors[i] + 1) {
          t_colors++;
          neo_deltas[i] = 0;
          // Serial.print("setting neo_delta to 0 ::: t_colors = ");
          // Serial.println(t_colors);
        }
        // for each color for each of the motor/roof/floor etc...
        // add the corresponding neo_delta
        neo_colors[i] = neo_colors[i] + neo_deltas[i];
        // if the color is less than 0 or more than 255 flip the polarity of the delta and set to 0 or 255
        if (neo_colors[i] < 0) {
          Serial.print(i);
          Serial.print("\t");
          Serial.println(F("VENTURED BELOW 0 "));
          neo_colors[i] = 0;
          neo_deltas[i] = 1;
          Serial.print(F("NEO DELTA CHANGED TO : "));
          Serial.println(neo_deltas[i]);
        } else if (neo_colors[i] > 255) {
          Serial.print(i);
          Serial.print("\t");
          Serial.print(F(" VENTURED ABOVE 255 "));
          neo_colors[i] = 255;
          neo_deltas[i] = -1;
          Serial.print(F("NEO DELTA CHANGED TO : "));
          Serial.println(neo_deltas[i]);
        }
        // if all three colors are cool then choose a new target color
        if (t_colors == 9) {
          // Serial.println(F("t_colors is equal to 9"));
          /*
            Serial.print(" : ");
            Serial.print(target_neo_colors[i]);
            Serial.print(" ");
            Serial.print(target_neo_colors[i + 1][1]);
            Serial.print(" ");
            Serial.println(target_neo_colors[i][2]);
          */
          for (int i = 0; i < 9; i++) {
            target_neo_colors[i] = random(255);
            if (target_neo_colors[i] > neo_colors[i]) {
              neo_deltas[i] = 1;
            } else if (target_neo_colors[i] < neo_colors[i]) {
              neo_deltas[i] = -1;
            } else {
              neo_deltas[i] = 0;
            }
          }
        }
      }
      // setAllNeoP(50, 50, 50);
      setNeoP();
    }
    else {
      Serial.println("VIEWING LIGHTS ACTIVATED");
      // setAllNeoP(50, 50, 50);
      // state = ACTIVE;
    }
  }
}

void setNeoDeltas(int target) {
  for (int i = 0; i < 9; i++) {
    neo_deltas[i] = target;
  }
}

void moveToWhite() {
  for (int i = 0; i < 9; i++) {
    neo_colors[i] += neo_deltas[i];
    if (neo_colors[i] > 255) {
      neo_colors[i] = 255;
    }
  }
  setNeoP();
}

void setAllNeoP(int r, int g, int b) {
  setColor(lights.Color(r, g, b));
}


void setNeoP() {
  setColor(lights.Color(int(neo_colors[0]), int(neo_colors[1]), int(neo_colors[2])));
  setMotorColor(lights.Color(int(neo_colors[3]), int(neo_colors[4]), int(neo_colors[5])));
  setFloorColor(lights.Color(int(neo_colors[6]), int(neo_colors[7]), int(neo_colors[8])));
}


void testNeoP(int t, boolean p) {
  int r, g, b;
  r = int(random(255));
  g = int(random(255));
  b = int(random(255));
  if (p == true) {
    Serial.print(r);
    Serial.print(".");
    Serial.print(g);
    Serial.print(".");
    Serial.print(b);
    Serial.print("\t");
  }
  setColor(lights.Color(r, g, b));
  delay(t);
  r = int(random(255));
  g = int(random(255));
  b = int(random(255));
  if (p == true) {
    Serial.print(r);
    Serial.print(".");
    Serial.print(g);
    Serial.print(".");
    Serial.print(b);
    Serial.print("\t");
  }
  setColor(lights.Color(r, g, b));
  delay(t);
  r = int(random(255));
  g = int(random(255));
  b = int(random(255));
  if (p == true) {
    Serial.print(r);
    Serial.print(".");
    Serial.print(g);
    Serial.print(".");
    Serial.print(b);
    Serial.print("\t");
  }
  setColor(lights.Color(r, g, b));
  delay(t);
  Serial.println();
}


// Fill the dots one after the other with a color
void colorWipe(Adafruit_NeoPixel lights, uint32_t c, uint8_t wait) {
  for (uint16_t i = 0; i < lights.numPixels(); i++) {
    lights.setPixelColor(i, c);
    lights.show();
    delay(wait);
  }
}

void setMotorColor (uint32_t c) {
  for (uint16_t i = lights.numPixels() - 16; i < lights.numPixels(); i++) {
       lights.setPixelColor(i, c);
  }
  lights.show();
}

void setFloorColor (uint32_t c) {
    for (uint16_t i = 0; i < 68; i++) {
       lights.setPixelColor(i, c);
  }
  lights.show();
}

void setColor (uint32_t c) {
  //Serial.print(F(" | Color "));
  for (uint16_t i = 68; i < lights.numPixels()-16; i++) {
    lights.setPixelColor(i, c);
  }
  lights.show();
}

/*
  void setMotorColor (uint32_t c) {
  for (uint16_t i = 0; i < lights.numPixels(); i++) {
    lights.setPixelColor(i, c);
  }
  lights.show();
  }

  void setRoofColor (uint32_t c) {
  for (uint16_t i = 0; i < lights.numPixels(); i++) {
    lights.setPixelColor(i, c);
  }
  lights.show();
  }

  void setFloorColor (uint32_t c) {
  for (uint16_t i = 0; i < lights.numPixels(); i++) {
    lights.setPixelColor(i, c);
  }
  lights.show();
  }
*/

// Input a value 0 to 255 to get a color value.
// The colours are a transition r - g - b - back to r.
uint32_t Wheel(byte WheelPos) {
  WheelPos = 255 - WheelPos;
  if (WheelPos < 85) {
    return lights.Color(255 - WheelPos * 3, 0, WheelPos * 3);
  }
  if (WheelPos < 170) {
    WheelPos -= 85;
    return lights.Color(0, WheelPos * 3, 255 - WheelPos * 3);
  }
  WheelPos -= 170;
  return lights.Color(WheelPos * 3, 255 - WheelPos * 3, 0);
}

