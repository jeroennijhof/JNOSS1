/* uno.cpp - library for Arduino UNO helping functions
 * 
 * Copyright (C) 2012, Jeroen Nijhof <jeroen@jeroennijhof.nl>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program - see the file LICENSE.
 */

#include "uno.h"

UNO::UNO(void) {
}

uint8_t UNO::get_bufsize() {
  return BUFSIZE;
}

char *UNO::float_to_str(float value) {
  static char str[BUFSIZE];
  int fp = (int)value;
  float sp = (value - fp);
  snprintf(str, BUFSIZE, "%d.%02d", fp, (int)(sp * 100));
  
  return str;
}

char *UNO::get_voltage(uint8_t pin, uint8_t multiply) {
  static char voltage[BUFSIZE];

  delay(100);
  snprintf(voltage, BUFSIZE, "%s", float_to_str((analogRead(pin) * PUVOLT) * multiply));

  return voltage;
}

char *UNO::get_temp(uint8_t pin, boolean fahrenheit) {
  static char temp[BUFSIZE];
  float value = 0;

  delay(100);
  if (fahrenheit) {
    value = ((analogRead(pin) * PUVOLT * 100) - 32) * (5.0/9.0);
  } else {
    value = analogRead(pin) * PUVOLT * 100;
  }
  snprintf(temp, BUFSIZE, "%s", float_to_str(value));

  return temp;
}

