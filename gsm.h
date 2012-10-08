#ifndef GSM_H
#define GSM_H

#include <Arduino.h>
#include <string.h>

#define num_to_char(number) ((number) < 10 ? \
                            ('0' + (number)) : \
                            (('A' - 10) + (number)))
 
#define first_four(byte) (0x0F & (byte))
#define last_four(byte) ((0xF0 & (byte)) >> 4)
 
#define hexdump_a(byte) num_to_char( last_four(byte))
#define hexdump_b(byte) num_to_char(first_four(byte))

#define BUFSIZE 75


class GSM : public SoftwareSerial {
  public:
    GSM(int rx_pin, int tx_pin);
    boolean send_sms(char *number, char *sms);
    char *get_signal();
    char *get_battery();
  private:
    char *to_pdu(char *number);
};


#endif
