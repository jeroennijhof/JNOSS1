#ifndef UNO_H
#define UNO_H

#include <Arduino.h>

#define BUFSIZE 16
#define PUVOLT 0.0048828125

class UNO {
  public:
    UNO(void);
    int get_bufsize();
    char *get_voltage(int pin, int multiply);
    char *get_temp(int pin, boolean fahrenheit);
    char *float_to_str(float value);
};

#endif
