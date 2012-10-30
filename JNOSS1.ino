/* JNOSS1.ino - Main Arduino file for HAB project JNOSS1
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

#define GPSRX 3
#define GPSTX 4
#define NTX2 5
#define GSMRX 7
#define GSMTX 6
#define MICROSD 8
#define CAM 9
#define TMP36 0
#define LM35 1
#define BATT 2

#include <SoftwareSerial.h>
#include <Servo.h>
#include <SDLogger.h>
#include "rtty.h"
#include "gsm.h"
#include "gps.h"
#include "uno.h"
#include "flycam.h"
 
#define DATASIZE 256

char data[DATASIZE];
uint16_t s_id = 0;

SDLogger sdlog;
RTTY rtty(NTX2);
GSM gsm(GSMRX, GSMTX);
GPS gps(GPSRX, GPSTX);
UNO uno;
FlyCam flycam;
 
void setup() {                
  Serial.begin(9600);
  Serial.println("JNOSS1 initializing...press key");
  while (!Serial.available()){}
  Serial.println(freeRam());
  
  gps.start();
  Serial.println("gps initialized.");
  
  flycam.attach(CAM);
  Serial.println("flycam initialized.");

  if (!sdlog.init()) {
    Serial.println("SD initialization failed.");
  } else {
    Serial.println("SD initialized.");
  }
  if (!sdlog.format()) {
    Serial.println("Format failed.");
  }
  Serial.println("...done.");
}
 
void loop() {
  Serial.println(freeRam());
  /* start filming */
  if (flycam.mode() == flycam.MODE_CAM && !flycam.is_recording()) {
    flycam.record(); // Start filming
  }

  /* stop filming after 30 seconds and make pictures */
  if (flycam.is_recording() && (s_id % 10) == 0) {
    flycam.record(); // Stop filming
    flycam.set_mode(flycam.MODE_PIC);
    flycam.record(); // Take picture
    flycam.record(); // Take another picture
    flycam.set_mode(flycam.MODE_CAM);
  }

  Serial.println(freeRam());
  /* get main battery voltage */
  char battery[uno.get_bufsize()];
  snprintf(battery, uno.get_bufsize(), "%s", uno.get_voltage(BATT, 3));

  /* get payload temperature */
  char temp_intern[uno.get_bufsize()];
  snprintf(temp_intern, uno.get_bufsize(), "%s", uno.get_temp(LM35, false));

  /* get external temperature */
  char temp_extern[uno.get_bufsize()];
  snprintf(temp_extern, uno.get_bufsize(), "%s", uno.get_temp(TMP36, true));

  Serial.println(freeRam());
  /* $$callsign,sentence_id,(time,latitude,longitude,altitude,fix,speed,ascentrate,satellites),
   *     battery,temperature_internal,temperature_external,gsm_signal,gsm_battery,cam_record_time,cam_pics*CHECKSUM\n
   */
  snprintf(data, DATASIZE, "$$jnoss1,%d,%s,%s,%s,%s,%s,%s,%d,%d", s_id, gps.get_info(), battery, temp_intern, temp_extern, gsm.get_signal(), gsm.get_battery(), flycam.record_time(), flycam.pics());

  /* First log to micro sd, then sms and final rtty */
  if (!sdlog.log(data)) {
    Serial.println("Write failed.");
  }

  Serial.println(freeRam());
  Serial.println(data);
  rtty.send(data);
  Serial.println(freeRam());
  //gsm.send_sms("31611111111", data);

  s_id++;
  if (flycam.is_recording()) {
    flycam.add_record_time(8);
  }
  delay(2000);
}

int freeRam () {
  extern int __heap_start, *__brkval; 
  int v; 
  return (int) &v - (__brkval == 0 ? (int) &__heap_start : (int) __brkval); 
}

