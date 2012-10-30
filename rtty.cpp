/* rtty.cpp - RTTY library for the NTX2 module.
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

#include "rtty.h"

uint8_t _pin = 0;

RTTY::RTTY(uint8_t pin) {
  pinMode(pin, OUTPUT);
  _pin = pin;
}

void RTTY::send(const char *data) {
  char c;
  uint8_t i = 0;

  c = *data++;
  while (c != '\0') {
    send_byte(c);
    c = *data++;
  }
  
  // Send chksum
  char chksum_str[6];
  snprintf(chksum_str, 6, "*%04X\n", crc16_chksum(data));
  c = chksum_str[i];
  while (c != '\0') {
    send_byte(c);
    c = chksum_str[++i];
  }
} 
 
void RTTY::send_byte(char c) {
  uint8_t i;
 
  send_bit(0); // Start bit
 
  // Send bits for for char LSB first	
  for (i=0; i<7; i++) { // Change this here 7 or 8 for ASCII-7 / ASCII-8
    if (c & 1)
      send_bit(1);
    else
      send_bit(0);	
    c = c >> 1;
  }
 
  send_bit(1); // Stop bit
  send_bit(1); // Stop bit
}
 
void RTTY::send_bit(uint8_t bit) {
  if (bit) {
    // high
    digitalWrite(_pin, HIGH);
  } else {
    // low
    digitalWrite(_pin, LOW);
  }
 
  //delayMicroseconds(3333); // 300 baud
  delayMicroseconds(13325); // 75 baud
  //delayMicroseconds(10000); // base delay for 45/50
  //delayMicroseconds(10000); // 50 baud
  //delayMicroseconds(12222); // 45 baud
}
 
uint16_t RTTY::crc16_chksum(const char *str) {
  size_t i;
  static uint16_t crc;
  uint8_t c;
 
  crc = 0xFFFF;
 
  // Calculate checksum ignoring the first two $s
  for (i = 2; i < strlen(str); i++) {
    c = str[i];
    crc = _crc_xmodem_update (crc, c);
  }
 
  return crc;
}
