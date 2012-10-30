#ifndef GPS_H
#define GPS_H

#include <Arduino.h>

#define BUFSIZE 128

class GPS: public SoftwareSerial {
  public:
    GPS(uint8_t rx_pin, uint8_t tx_pin);
    void start();
    char *get_info();
  private:
    void send_ubx(uint8_t *msg, uint8_t len);
    boolean get_ubx_ack(uint8_t *msg);
    char *get_pubx_item(uint8_t itemnr, char *pubx);
};

#endif
