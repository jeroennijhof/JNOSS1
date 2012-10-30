#ifndef FLYCAM_H
#define FLYCAM_H

#include <Arduino.h>
#include <Servo.h>

class FlyCam: public Servo {
  public:
    FlyCam();
    void set_mode(uint8_t mode);
    uint8_t mode();
    void record();
    boolean is_recording();
    void add_record_time(uint8_t seconds);
    uint16_t record_time();
    uint16_t pics();
    uint8_t MODE_CAM;
    uint8_t MODE_SER;
    uint8_t MODE_PIC;
  private:
    uint8_t _mode;
    boolean _recording;
    uint16_t _record_time; // in seconds
    uint16_t _pics;
};

#endif
