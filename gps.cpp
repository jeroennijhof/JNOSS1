/* gps.cpp - gps library for uBlox via serial
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
#include "gps.h"

GPS::GPS(uint8_t rx_pin, uint8_t tx_pin):SoftwareSerial(rx_pin, tx_pin) {
  begin(9600);
}

void GPS::start() {
  // Wait for uBlox to become ready
  delay(1500);
  
  // Disable all NMEA messages, using $PUBX only
  println("$PUBX,40,GLL,0,0,0,0*5C");
  println("$PUBX,40,GGA,0,0,0,0*5A");
  println("$PUBX,40,GSA,0,0,0,0*4E");
  println("$PUBX,40,RMC,0,0,0,0*47");
  println("$PUBX,40,GSV,0,0,0,0*59");
  println("$PUBX,40,VTG,0,0,0,0*5E");
  
  delay(1500); // Wait for the GPS to process all commands
  
  // Set the navigation mode (Airborne, 1G)
  uint8_t set_nav5[] = { 0xB5, 0x62, 0x06, 0x24, 0x24, 0x00, 0xFF, 0xFF, 0x06, 0x03, 0x00, 0x00, 0x00, 0x00, 0x10, 0x27, 0x00, 0x00, 0x05, 0x00, 0xFA, 0x00, 0xFA, 0x00, 0x64, 0x00, 0x2C, 0x01, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x16, 0xDC };
  send_ubx(set_nav5, sizeof(set_nav5)/sizeof(uint8_t));
  
  while (get_ubx_ack(set_nav5) != true) {
    delay(1000);
  }
  delay(1000);
}

char *GPS::get_info() {
  static char info[BUFSIZE] = "";
  char buffer[BUFSIZE] = "";
  uint8_t idx = 0;

  if (!listen())
    flush();

  println("$PUBX,00*33");
  delay(1200);  

  while (available()) {
    if (idx == (BUFSIZE-1)) {
      break;
    }
    buffer[idx] = read(); 
    idx++;
  }
  buffer[idx] = '\0';
  
  /* $PUBX,00,hhmmss.ss,Latitude,N,Longitude,E,AltRef,NavStat,Hacc,Vacc,
   *    SOG,COG,Vvel,ageC,HDOP,VDOP,TDOP,GU,RU,DR,*cs
   * convert to:
   *    time,latitude,longitude,altitude,fix,speed,ascentrate,satellites
   */
  snprintf(info, 7, "%s", get_pubx_item(2, buffer, "0.0"));
  strncat(info, ",", 1);
  strncat(info, get_pubx_item(3, buffer, "0.0"), 12);
  strncat(info, ",", 1);
  strncat(info, get_pubx_item(5, buffer, "0.0"), 12);
  strncat(info, ",", 1);
  strncat(info, get_pubx_item(7, buffer, "0.0"), 12);
  strncat(info, ",", 1);
  strncat(info, get_pubx_item(8, buffer, "NF"), 2);
  strncat(info, ",", 1);
  strncat(info, get_pubx_item(11, buffer, "0.0"), 7);
  strncat(info, ",", 1);
  strncat(info, get_pubx_item(13, buffer, "0.0"), 7);
  strncat(info, ",", 1);
  strncat(info, get_pubx_item(18, buffer, "0"), 2);

  return info;
}

void GPS::send_ubx(uint8_t *msg, uint8_t len) {
  for (uint8_t i = 0; i < len; i++)
    write(msg[i]);
  println();
}

boolean GPS::get_ubx_ack(uint8_t *msg) {
  uint8_t b;
  uint8_t ackByteID = 0;
  uint8_t ackPacket[10];
  unsigned long startTime = millis();
 
  // Construct the expected ACK packet    
  ackPacket[0] = 0xB5; // header
  ackPacket[1] = 0x62; // header
  ackPacket[2] = 0x05; // class
  ackPacket[3] = 0x01; // id
  ackPacket[4] = 0x02; // length
  ackPacket[5] = 0x00;
  ackPacket[6] = msg[2]; // ACK class
  ackPacket[7] = msg[3]; // ACK id
  ackPacket[8] = 0; // CK_A
  ackPacket[9] = 0; // CK_B
 
  // Calculate the checksums
  for (uint8_t i = 2; i < 8; i++) {
    ackPacket[8] = ackPacket[8] + ackPacket[i];
    ackPacket[9] = ackPacket[9] + ackPacket[8];
  }
 
  while (1) {
    if (ackByteID > 9) {
      // All packets in order!
      return true;
    }
 
    // Timeout if no valid response in 3 seconds
    if (millis() - startTime > 3000) { 
      return false;
    }
 
    if (available()) {
      b = read();
 
      if (b == ackPacket[ackByteID]) { 
        ackByteID++;
      } else { 
        ackByteID = 0; // invalid order
      }
    }
  }
}

char *GPS::get_pubx_item(uint8_t itemnr, char *pubx, char *dflt) {
  static char item[BUFSIZE] = "";
  char buffer[BUFSIZE] = "";
  char *ptr;
  uint8_t x;

  snprintf(buffer, BUFSIZE, "%s", pubx);
  for (x = 0; x < itemnr; x++) {
    ptr = strstr(buffer, ",");
    snprintf(buffer, BUFSIZE, "%s", ptr+1);
  }
  ptr = strstr(buffer, ",");
  snprintf(item, strlen(buffer)-strlen(ptr)+1, "%s", buffer);
  
  if (strlen(item) == 0) {
    snprintf(item, strlen(dflt)+1, "%s", dflt);
  }

  return item;
}

