#include <stddef.h>
#include <string.h>
#include <errno.h>
#include <misc/printk.h>
#include <misc/byteorder.h>

#include <zephyr.h>
#include <zephyr/types.h>
#include <logging/log.h>
#include <logging/log_ctrl.h>

#include <settings/settings.h>

#include <bluetooth/bluetooth.h>
#include <bluetooth/hci.h>
#include <bluetooth/conn.h>
#include <bluetooth/uuid.h>
#include <bluetooth/gatt.h>

#include "cgms.h"
#include "rdb.h"
#include "../ble_types.h"
#include "../ble_utils.h"


// Values
static ble_meas_pkt_t measurement_value;

static u32_t trans_id_value;

// UUIDs for service

static struct bt_uuid_128 XBRIDGE2_SERVICE_UUID = BT_UUID_INIT_128(
    0xfb, 0x34, 0x9b, 0x5f, 0x80, 0x00, 0x00, 0x80,
    0x00, 0x10, 0x00, 0x00, 0xe0, 0xff, 0x00, 0x00);


/**
 * 
 * 
 * MEASUREMENT CHARACTERISITC
 * 
 * This characterisit is used to carry measurements, both during
 * normal operation, and as responce to RACP requests
 * 
 */
static struct bt_uuid_128 MEASUREMENT_CHAR_UUID = BT_UUID_INIT_128(
    0xfb, 0x34, 0x9b, 0x5f, 0x80, 0x00, 0x00, 0x80,
    0x00, 0x10, 0x00, 0x00, 0xe1, 0xff, 0x00, 0x00);

static struct bt_gatt_ccc_cfg measurement_ccc_cfg[10] = {};
static u8_t measurement_notify_enabled;


static void measurement_ccc_cfg_changed(const struct bt_gatt_attr *attr, u16_t value)
{
    measurement_notify_enabled = (value == BT_GATT_CCC_INDICATE) ? 1 : 0;
}

static ssize_t read_measurement(struct bt_conn *conn, const struct bt_gatt_attr *attr,
			 void *buf, u16_t len, u16_t offset)
{
	const ble_meas_pkt_t *value = attr->user_data;

	return bt_gatt_attr_read(conn, attr, buf, len, offset, value,
				 sizeof(*value));
}

static ssize_t write_measurement(struct bt_conn *conn, const struct bt_gatt_attr *attr, const void *buf, u16_t len, u16_t offset, u8_t flags) {
    
    //TODO: Read first byte to check that size is > 1
    // then use second byte to identify packet type
    // attr->user_data

    // Data we can use begin at value + offset. 
    // Size of package is len

    return len;
}

/**
 * 
 * 
 * SIMPLE CGMS SERVICE DECLARATION
 * 
 * 
 */
static struct bt_gatt_attr attrs[] = {
    BT_GATT_PRIMARY_SERVICE(&XBRIDGE2_SERVICE_UUID),
    BT_GATT_CHARACTERISTIC(&MEASUREMENT_CHAR_UUID.uuid,
			       BT_GATT_CHRC_READ | BT_GATT_CHRC_WRITE | BT_GATT_CHRC_NOTIFY,
			       BT_GATT_PERM_READ | BT_GATT_PERM_WRITE,
                   read_measurement, write_measurement, &measurement_value),
	BT_GATT_CCC(measurement_ccc_cfg, measurement_ccc_cfg_changed)
};

static struct bt_gatt_service simple_cgms_svc = BT_GATT_SERVICE(attrs);


/*******************************************
 * 
 * 
 * Utility functions
 * 
 * 
 * ****************************************/
u32_t calculateTimeDelay(u32_t timestamp) 
{
    return -1 * (timestamp - k_uptime_get_32()); // Multiply by -1 to ge tpositive value.
}

void set_measurement_value(meas_record_t* ptr) 
{
    measurement_value = (ble_meas_pkt_t) {
        .raw_value = ptr->raw,
        .filtered_value = ptr->filtered,
        .trans_bat = ptr->trans_batt,
        .bridge_bat = 100,
        .transmitter_id = trans_id_value,
        .measurement_delay = calculateTimeDelay(ptr->timestamp)
    };

    bt_gatt_notify(NULL, &attrs[1], &measurement_value, sizeof(measurement_value));
}


/******************************************
 * 
 * 
 * Public API for init and adding values
 * and state
 * 
 * 
 * ****************************************/

// Create service
void cgms_init(void)
{
    memset(&measurement_value, 0, sizeof(ble_meas_pkt_t));

    bt_gatt_service_register(&simple_cgms_svc);
}

void cgms_send_all() {
    // TODO: Not implemented
}

void cgms_add_measurement(dexcom_package_t reading)
{
    meas_record_t* rec_ptr;
    rdb_new(&rec_ptr);

    *rec_ptr = (meas_record_t) {
        .filtered = reading.filIsig,
        .raw = reading.rawIsig,
        .trans_batt = reading.batLevel,
        .timestamp = reading.timestamp,
    };

    set_measurement_value(rec_ptr);
}
