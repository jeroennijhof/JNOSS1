#ifndef UNO_H
#define UNO_H

#include <Arduino.h>

#define BUFSIZE 16
#define PUVOLT 0.0048828125

class UNO {
  public:
    UNO(void);
    uint8_t get_bufsize();
    char *get_voltage(uint8_t pin, uint8_t multiply);
    char *get_temp(uint8_t pin, boolean fahrenheit);
    char *float_to_str(float value);
};

#endif
