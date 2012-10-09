#ifndef FLYCAM_H
#define FLYCAM_H

#include <Arduino.h>
#include <Servo.h>

class FlyCam {  
  public:
    FlyCam(int pin);
    void set_mode(int mode);
    int mode();
    void record();
    boolean is_recording();
    void add_record_time(int seconds);
    int record_time();
    int total_record_time();
    int total_pics();
    int MODE_CAM;
    int MODE_SER;
    int MODE_PIC;
  private:
    int _mode;
    boolean _recording;
    int _record_time; // in seconds
    int _total_record_time; // in seconds
    int _total_pics;
};

#endif
