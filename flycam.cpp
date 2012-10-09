/* flycam.cpp - FlyCam library for the flycamone eco v2
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

#include "flycam.h"
 
Servo fcam;

FlyCam::FlyCam(int pin) {
  MODE_CAM = 0;
  MODE_SER = 1;
  MODE_PIC = 2;
  _mode = 0;
  _recording = false;
  _record_time = 0; // in seconds
  _total_record_time = 0; // in seconds
  _total_pics = 0;
  
  fcam.attach(pin);
}

void FlyCam::set_mode(int mode) {
  while (_mode != mode) {
    fcam.write(180);
    delay(3500);
    fcam.write(0);
    delay(500);
    _mode++;
    if (_mode > MODE_PIC)
      _mode = 0;
  }
}

int FlyCam::mode() {
  return _mode;
}

void FlyCam::record() {
  fcam.write(180);
  delay(500);
  fcam.write(0);
  delay(500);
  if (_recording||_mode != MODE_CAM) {
    _recording = false;
    _total_record_time += _record_time;
    _record_time = 0;
  } else
    _recording = true;
  if (_mode == MODE_PIC)
    _total_pics++;
}

boolean FlyCam::is_recording() {
  return _recording;
}

void FlyCam::add_record_time(int seconds) {
  _record_time += seconds;
}

int FlyCam::record_time() {
  return _record_time;
}

int FlyCam::total_record_time() {
  return _total_record_time;
}

int FlyCam::total_pics() {
  return _total_pics;
}
