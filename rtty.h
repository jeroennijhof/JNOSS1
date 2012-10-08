#ifndef RTTY_H
#define RTTY_H

#include <Arduino.h>
#include <string.h>
#include <util/crc16.h>

class RTTY {  
  public:
    RTTY(int pin);
    void send(char *data);
    void send_byte(char c);
    void send_bit(int bit);
    uint16_t crc16_chksum(char *string);
};

#endif
