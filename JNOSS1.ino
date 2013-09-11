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
  Serial.println("JNOSS1 initializing...");
  
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
  /* start filming */
  if (flycam.mode() == flycam.MODE_CAM && !flycam.is_recording()) {
    flycam.record(); // Start filming
  }

  /* get main battery voltage */
  char battery[uno.get_bufsize()];
  snprintf(battery, uno.get_bufsize(), "%s", uno.get_voltage(BATT, 3));

  /* get payload temperature */
  char temp_intern[uno.get_bufsize()];
  snprintf(temp_intern, uno.get_bufsize(), "%s", uno.get_temp(LM35, false));

  /* get external temperature */
  char temp_extern[uno.get_bufsize()];
  snprintf(temp_extern, uno.get_bufsize(), "%s", uno.get_temp(TMP36, false));

  /* $$callsign,sentence_id,(time,latitude,longitude,altitude,fix,speed,ascentrate,satellites),
   *     battery,temperature_internal,temperature_external,gsm_signal,gsm_charging,gsm_battery,cam_record_time,cam_pics*CHECKSUM\n
   */
  snprintf(data, DATASIZE, "$$jnoss1,%d,%s,%s,%s,%s,%s,%s,%d,%d", s_id, gps.get_info(), battery, temp_intern, temp_extern, gsm.get_signal(), gsm.get_battery(), flycam.record_time(), flycam.pics());

  /* First log to micro sd, then rtty and final sms */
  if (!sdlog.log(data)) {
    Serial.println("Write failed.");
  }

  rtty.send(data);
  Serial.println(data);
  
  if (flycam.is_recording()) {
    // every cicle takes about 17 seconds
    flycam.add_record_time(17);
  }

  /* stop filming after 3 minutes and make pictures */
  if (flycam.is_recording() && (flycam.record_time() % 187) == 0) {
    flycam.record(); // Stop filming
    flycam.set_mode(flycam.MODE_PIC);
    flycam.record(); // Take picture
    flycam.record(); // Take another picture
    flycam.set_mode(flycam.MODE_CAM);
  }

  /* send sms every 2 cicles and only when we have signal */
  if (s_id % 2 == 0 && int(gsm.get_signal()) > 1) {
    gsm.send_sms("31612345678", data);
  }

  s_id++;
}

