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

FlyCam::FlyCam(): Servo() {
  MODE_CAM = 0;
  MODE_SER = 1;
  MODE_PIC = 2;
  _mode = 0;
  _recording = false;
  _record_time = 0; // in seconds
  _pics = 0;
}

void FlyCam::set_mode(uint8_t mode) {
  while (_mode != mode) {
    write(180);
    delay(3800);
    write(0);
    delay(800);
    _mode++;
    if (_mode > MODE_PIC)
      _mode = 0;
  }
}

uint8_t FlyCam::mode() {
  return _mode;
}

void FlyCam::record() {
  write(180);
  delay(800);
  write(0);
  delay(2800);
  if (_recording||_mode != MODE_CAM) {
    _recording = false;
  } else {
    _recording = true;
  }
  if (_mode == MODE_PIC)
    _pics++;
}

boolean FlyCam::is_recording() {
  return _recording;
}

void FlyCam::add_record_time(uint8_t seconds) {
  _record_time += seconds;
}

uint16_t FlyCam::record_time() {
  return _record_time;
}

uint16_t FlyCam::pics() {
  return _pics;
}
