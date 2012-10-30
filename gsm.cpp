/* gsm.cpp - library for gsm via serial
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

#include "SoftwareSerial.h"
#include "gsm.h"

GSM::GSM(uint8_t rx_pin, uint8_t tx_pin):SoftwareSerial(rx_pin, tx_pin) {
  begin(9600);
}

boolean GSM::send_sms(char *number, char *sms) {
  size_t sms_len, x;
  char c, l;
  long i, n;
  char output[BUFSIZE] = "";
  uint8_t idx = 0;
 
  if (!listen())
    flush();

  sms_len = strlen(sms);
  i = sms_len * 7;
 
  // Round i up to a multiple of 8
  if (i & 0x07) i = (i & ~0x07) + 0x08;
 
  // Calculate the number of message octets
  i = i / 8;
 
  // Set operating mode to PDU
  println("AT");
  delay(1000);
  println("AT+CMGF=0");
  delay(1000);
  print("AT+CMGS=");
  println(i + 14);
  delay(1000);
  print("001100");
  write(hexdump_a(strlen(number) & 0xFF)); // phone number size
  write(hexdump_b(strlen(number) & 0xFF)); // in hex
  print("91");
  print(to_pdu(number)); // convert phone number to pdu
  print("0000AA");
  write(hexdump_a(sms_len & 0xFF));
  write(hexdump_b(sms_len & 0xFF));
 
  l = 0;
  n = 0;
 
  for (x = 0; x < sms_len; x++) {
    if (sms[x] == '$')  sms[x] = 0x02;
 
    n |= (sms[x] & 0x7F) << l;
    l += 7;
 
    if (l >= 8) {
      write(hexdump_a(n & 0xFF));
      write(hexdump_b(n & 0xFF));
      l -= 8;
      n >>= 8;
    }
  }
 
  if (l != 0) {
    write(hexdump_a(n & 0xFF));
    write(hexdump_b(n & 0xFF));
  }
  write(0x1A);
  println();

  while (available()) {
    if (idx == (BUFSIZE-1)) {
      break;
    }
    output[idx] = read();
    idx++;
  }
  if (strstr(output, ">") == NULL) {
    return false;
  }

  return true;
}

char *GSM::get_signal() {
  static char buffer[3] = "";
  char output[BUFSIZE] = "";
  char *ptr;
  uint8_t idx = 0;

  if (!listen())
    flush();
  
  println("AT+CSQ");
  delay(1000);
  while (available()) {
    if (idx == (BUFSIZE-1)) {
      break;
    }
    output[idx] = read();
    idx++;
  }

  if ((ptr = strstr(output, "+CSQ: ")) == NULL) {
    return "-1";
  }

  strncpy(buffer, ptr+6, 2);
  buffer[2] = '\0';
  if (buffer[1] == ',')
    buffer[1] = '\0';

  return buffer;
}

char *GSM::get_battery() {
  static char buffer[6] = "";
  char output[BUFSIZE] = "";
  char *ptr;
  uint8_t idx = 0;

  if (!listen())
    flush();
  
  println("AT+CBC");
  delay(1000);
  while (available()) {
    if (idx == (BUFSIZE-1)) {
      break;
    }
    output[idx] = read();
    idx++;
  }

  if ((ptr = strstr(output, "+CBC: ")) == NULL) {
    return "-1,-1";
  }

  strncpy(buffer, ptr+6, 5);
  buffer[5] = '\0';
  if (buffer[4] != '0')
    buffer[4] = '\0';

  return buffer;
}

char *GSM::to_pdu(char *number) {
  static char pdu[BUFSIZE] = "";
  char _number[BUFSIZE] = "";
  size_t x;

  if (strlen(number) & 1) {
    // number is odd so add a F
    snprintf(_number, BUFSIZE, "%sF", number);
  } else {
    snprintf(_number, BUFSIZE, "%s", number);
  }

  for (x = 0; x < strlen(_number); x++) {
    pdu[x] = _number[x+1];
    pdu[x+1] = _number[x];
    x++;
  }
  pdu[x] = '\0';

  return pdu;
}
