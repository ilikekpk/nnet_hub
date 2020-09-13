#include "app_advertising.h"

#include "nrf_sdh_ble.h"
#include "app_error.h"
#include "ble_nus.h"
#include "ble_advdata.h"

#define BLE_COMBINED_ADV_BLE_OBSERVER_PRIO 1                                    /**< Priority of observer (need for get ble events. */

#define ADV_PACK_MANUF_ID               0x0059                                  /**< First 2 bytes of manuf data. */
#define ADVERTISING_MANUF_DATA_SIZE     17                                      /**< Size of advertising_manuf_data buffer for handling manuf data. */

void ble_evt_combined_adv_handler(ble_evt_t const * p_evt, void * p_context);

NRF_SDH_BLE_OBSERVER(m_ble_combined_advertising_observer, BLE_COMBINED_ADV_BLE_OBSERVER_PRIO, ble_evt_combined_adv_handler, NULL); //configure observer of ble events

static uint8_t m_adv_handle = BLE_GAP_ADV_SET_HANDLE_NOT_SET;                   /**< Advertising handle used to identify an advertising set. */
static uint8_t enc_adv_buf[BLE_GAP_ADV_SET_DATA_SIZE_MAX];                      /**< Buffer for storing an encoded advertising set. */
static ble_gap_adv_data_t m_adv_data =                                          /**< Pointer to enc_adv_buf / enc_sr_buf (need for sdk setup) */
{
    .adv_data.p_data = enc_adv_buf,
    .scan_rsp_data.p_data = NULL,
    .adv_data.len = BLE_GAP_ADV_SET_DATA_SIZE_MAX,
    .scan_rsp_data.len = 0
};

void advertising_init(uint32_t adv_interval, uint16_t adv_duration)
{
    ret_code_t err_code;
    ble_gap_adv_params_t adv_params = {0};

    adv_params.properties.type = BLE_GAP_ADV_TYPE_NONCONNECTABLE_NONSCANNABLE_UNDIRECTED;    
    adv_params.duration = adv_duration;
    adv_params.p_peer_addr = NULL;
    adv_params.filter_policy = BLE_GAP_ADV_FP_ANY;
    adv_params.interval = adv_interval;

    err_code = sd_ble_gap_adv_set_configure(&m_adv_handle, &m_adv_data, &adv_params);
    APP_ERROR_CHECK(err_code);
  
}

void advertising_start(void)
{
    ret_code_t err_code = sd_ble_gap_adv_start(m_adv_handle, 1);
    APP_ERROR_CHECK(err_code);
    err_code = sd_ble_gap_tx_power_set(BLE_GAP_TX_POWER_ROLE_ADV, m_adv_handle, 4);
    APP_ERROR_CHECK(err_code);
}


void set_manuf(uint8_t* data, uint16_t size) //function for setup manuf data (after duration)
{
    if (size > ADVERTISING_MANUF_DATA_SIZE) return;

    ret_code_t err_code;

    ble_advdata_t advdata = {0};
    ble_advdata_manuf_data_t manuf_data = {0};
    
    manuf_data.company_identifier = ADV_PACK_MANUF_ID;
    manuf_data.data.p_data = data;
    manuf_data.data.size = ADVERTISING_MANUF_DATA_SIZE;

    advdata.name_type = BLE_ADVDATA_NO_NAME;
    advdata.include_appearance = false;
    advdata.flags = BLE_GAP_ADV_FLAGS_LE_ONLY_GENERAL_DISC_MODE; 
    advdata.p_manuf_specific_data = &manuf_data;

    err_code = ble_advdata_encode(&advdata, m_adv_data.adv_data.p_data, &m_adv_data.adv_data.len);
    APP_ERROR_CHECK(err_code);

}

void ble_evt_combined_adv_handler(ble_evt_t const * p_ble_evt, void * p_context)
{
    switch (p_ble_evt->header.evt_id)
    {
        case BLE_GAP_EVT_ADV_SET_TERMINATED: 
            break;
        default:
            break;
    }
}
