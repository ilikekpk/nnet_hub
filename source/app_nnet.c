#include "app_nnet.h"

#include "app_advertising.h"
#include "crc16.h"

#include "nrf_sdh.h"
#include "nrf_sdh_soc.h"
#include "nrf_sdh_ble.h"

void nnet_init(nnet_config_t* config)
{
    advertising_init(config->adv_interval, config->adv_duration);
}

void nnet_send_switch_message(nnet_config_t* config, uint8_t nnet_addr, bool switch_state)
{
    uint8_t packet[17] = {0};
    packet[0] = nnet_addr;                                   //node addr (not use in calc crc/encrypt)
    packet[1] = 0x01;                                        //packet type
    packet[14] = switch_state;                               //data
    
    uint16_t crc16 = crc16_compute(packet + 1, 16, NULL);    // + 1 skip header (CRC-16/CCITT-FALSE)
    
    packet[15] = crc16 >> 8 & 0xFF;
    packet[16] = crc16 & 0xFF;

    nrf_ecb_hal_data_t ecb_data = {0};
    
    memcpy(ecb_data.key, config->aes_key, 16);
    memcpy(ecb_data.cleartext, packet + 1, 16);
    
    sd_ecb_block_encrypt(&ecb_data);
    memcpy(packet + 1, ecb_data.ciphertext, 16);

    set_manuf(packet, 17);
    advertising_start();
}

