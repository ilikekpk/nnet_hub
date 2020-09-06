#ifndef APP_NNET_H
#define APP_NNET_H

#include <stdint.h>
#include <stdbool.h>

typedef struct { 
    
    uint16_t adv_interval;
    uint16_t adv_duration;

    uint8_t aes_key[16];

} nnet_config_t;

void nnet_init(nnet_config_t* config);
void nnet_send_switch_message(nnet_config_t* config, uint8_t nnet_addr, bool switch_state);

#endif
