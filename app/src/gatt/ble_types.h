#pragma once
#include <zephyr/types.h>

/**
 * @brief A measurement record to transmitted according to
 * xBridge2 protocol.
 */
typedef struct ble_meas_pkt {
    u8_t size;
    u8_t packet_type;
    u32_t raw_value;
    u32_t filtered_value;
    u8_t trans_bat;
    u8_t bridge_bat;
    u32_t transmitter_id;
    u32_t measurement_delay;
    
    // Protocol level is part of spec, but not used in xDrip
    // also, not including it reduces package size to 20 bytes.
    //u8_t protocol_level;

} __attribute__((packed)) ble_meas_pkt_t;

/**
 * @brief A beacon package sent from the device to the app
 * to indicate which transmitter id the bridge is filtering on.
 */
typedef struct ble_beacon_pkt {
    u8_t size;
    u8_t packet_type;
    u32_t transmitter_id;
    u8_t protocol_level;
} __attribute__((packed)) ble_beacon_pkt_t;

/**
 * @brief A transmitter id package sent form the app to device.
 * 
 */
typedef struct ble_trans_id_pkt {
    u8_t size;
    u8_t packet_type;
    u32_t transmitter_id;
} __attribute__((packed)) ble_trans_id_pkt_t;

/**
 * @brief A measurement acknowledge package sent from app to
 * device to confirm reception of package. 
 * 
 */
typedef struct ble_meas_ack_pkt {
    u8_t size;
    u8_t packet_type; 
} __attribute__((packed)) ble_meas_ack_pkt_t;
