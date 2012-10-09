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
#define GSMRX 6
#define GSMTX 7
#define MICROSD 8
#define CAM 9
#define TMP36 0
#define LM35 1
#define BATT 2

#include <SoftwareSerial.h>
#include <SD.h>
#include <Servo.h>
#include "rtty.h"
#include "gsm.h"
#include "gps.h"
#include "uno.h"
#include "flycam.h"
 
#define DATASIZE 256

char data[DATASIZE];
int s_id = 0;

RTTY rtty(NTX2);
GSM gsm(GSMRX, GSMTX);
GPS gps(GPSRX, GPSTX);
UNO uno;
FlyCam flycam(CAM);
 
void setup() {                
  Serial.begin(9600);
  Serial.println("JNOSS1 initializing...");

  gps.start();
  Serial.println("gps initialized.");

  // needed for SPI to work
  pinMode(10, OUTPUT);
  if (!SD.begin(MICROSD)) {
    Serial.println("micro sd failed.");
  }
  Serial.println("micro sd initialized.");
  Serial.println("done.");
}
 
void loop() {
  // start filming
  if (flycam.mode() == flycam.MODE_CAM && !flycam.is_recording())
    flycam.record();

  // stop filming after 30 seconds and make pictures
  if (flycam.is_recording() && flycam.record_time() > 60) {
    flycam.record();
    flycam.set_mode(flycam.MODE_PIC);
    flycam.record();
    delay(1000);
    flycam.record();
    flycam.set_mode(flycam.MODE_CAM);
  }

  // get main battery voltage
  char battery[uno.get_bufsize()];
  snprintf(battery, uno.get_bufsize(), "%s", uno.get_voltage(BATT, 3));

  // get payload temperature
  char temp_intern[uno.get_bufsize()];
  snprintf(temp_intern, uno.get_bufsize(), "%s", uno.get_temp(LM35, false));

  // get external temperature
  char temp_extern[uno.get_bufsize()];
  snprintf(temp_extern, uno.get_bufsize(), "%s", uno.get_temp(TMP36, true));

  
  // $$callsign,sentence_id,(time,latitude,longitude,altitude,fix,speed,ascentrate,satellites),
  //      battery,temperature_internal,temperature_external,gsm_signal,gsm_battery,cam_record_time,cam_pics*CHECKSUM\n
  snprintf(data, DATASIZE, "$$jnoss1,%d,%s,%s,%s,%s,%s,%s,%d,%d", s_id, gps.get_info(), battery, temp_intern, temp_extern, gsm.get_signal(), gsm.get_battery(), flycam.total_record_time(), flycam.total_pics());

  // First log to micro sd, then sms and final rtty
  File flight_data = SD.open("flight.txt", FILE_WRITE);

  if (flight_data) {
    flight_data.println(data);
    flight_data.close();
  }
  Serial.println(data);
  //Serial.println(gsm.send_sms("31611111111", data));
  //rtty.send(data);

  s_id++;
  if (flycam.is_recording())
    flycam.add_record_time(8);
  delay(2000);
}

