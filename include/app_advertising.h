#ifndef APP_ADVERTISING
#define APP_ADVERTISING

#include <stdint.h>
#include <stdbool.h>

void advertising_init(uint32_t adv_interval, uint16_t adv_duration);
void advertising_start(void);
void set_manuf(uint8_t* data, uint16_t size);


#endif
