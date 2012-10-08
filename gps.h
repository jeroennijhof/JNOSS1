#ifndef GPS_H
#define GPS_H

#include <Arduino.h>

#define BUFSIZE 128

class GPS : public SoftwareSerial {
  public:
    GPS(int rx_pin, int tx_pin);
    void start();
    char *get_info();
  private:
    void send_ubx(uint8_t *msg, uint8_t len);
    boolean get_ubx_ack(uint8_t *msg);
    char *get_pubx_item(int itemnr, char *pubx);
};

#endif
