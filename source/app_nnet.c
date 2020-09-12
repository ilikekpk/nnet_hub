#include "app_nnet.h"

#include "app_advertising.h"
#include "crc16.h"

#include "nrf_sdh.h"
#include "nrf_sdh_soc.h"
#include "nrf_sdh_ble.h"

#include "cifra_eax_aes.h"

static cf_aes_context cf_aes_config = {
    .rounds = AES128_ROUNDS
};

void nnet_init(nnet_config_t* config)
{
    cf_aes_init(&cf_aes_config, config->aes_key, 16);
    advertising_init(config->adv_interval, config->adv_duration);
}

void nnet_send_switch_message(nnet_config_t* config, uint8_t nnet_addr, bool switch_state)
{

    uint8_t packet[17] = {0};
    packet[0] = nnet_addr;                                   //node addr (not use in calc crc/encrypt)
    packet[1] = 0x01;                                        //packet type
    packet[14] = switch_state;                               //data
    
    uint16_t crc16 = crc16_compute(packet + 1, 16, NULL);    // + 1 skip nned_addr (CRC-16/CCITT-FALSE)
    
    packet[15] = crc16 >> 8 & 0xFF;
    packet[16] = crc16 & 0xFF;

    uint8_t out[17] = {0};

    cf_aes_encrypt(&cf_aes_config, packet + 1, out);
    
    memcpy(packet + 1, out, 16);

    set_manuf(packet, 17);                                   // 1 - node_addr and 16 - aes block
    advertising_start();
}

