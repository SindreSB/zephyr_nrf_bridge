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
static ble_meas_rec_t measurement_value;

/**
 * Status value carries this information, [bits]:
 * [0-3]    : Number of missed readings
 * [4]      : Transmitter ID set
 * [5]      : Time value set
 * [6-7]    : RFU
 */
static u8_t status_value;
static u8_t missed_readings;

static u32_t trans_id_value;

static u32_t time_set;
static ble_time_t time_value;

static u32_t rcap_value;

// UUIDs for service

static struct bt_uuid_128 CMGS_SERVICE_UUID = BT_UUID_INIT_128(
    0xf0, 0xde, 0xbc, 0x9a, 0x78, 0x56, 0x34, 0x12,
    0x78, 0x56, 0x34, 0x12, 0x78, 0x56, 0x34, 0x10);


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
    0xf0, 0xde, 0xbc, 0x9a, 0x78, 0x56, 0x34, 0x12,
    0x78, 0x56, 0x34, 0x12, 0x78, 0x56, 0x34, 0x11);

static struct bt_gatt_ccc_cfg measurement_ccc_cfg[10] = {};
static u8_t measurement_notify_enabled;



static void measurement_ccc_cfg_changed(const struct bt_gatt_attr *attr, u16_t value)
{
    measurement_notify_enabled = (value == BT_GATT_CCC_INDICATE) ? 1 : 0;
}

static ssize_t read_measurement(struct bt_conn *conn, const struct bt_gatt_attr *attr,
			 void *buf, u16_t len, u16_t offset)
{
	const ble_meas_rec_t *value = attr->user_data;

	return bt_gatt_attr_read(conn, attr, buf, len, offset, value,
				 sizeof(*value));
}


/**
 * 
 * 
 * STATUS CHARACTERISITC
 * 
 * This characterisit is used to hold status information, 
 * though this info is also available in a measurement package
 * 
 */
static struct bt_uuid_128 STATUS_CHAR_UUID = BT_UUID_INIT_128(
    0xf0, 0xde, 0xbc, 0x9a, 0x78, 0x56, 0x34, 0x12,
    0x78, 0x56, 0x34, 0x12, 0x78, 0x56, 0x34, 0x12);



static ssize_t read_status(struct bt_conn *conn, const struct bt_gatt_attr *attr,
			 void *buf, u16_t len, u16_t offset)
{
	const u8_t *value = attr->user_data;

	return bt_gatt_attr_read(conn, attr, buf, len, offset, value,
				 sizeof(*value));
}


/**
 * 
 * 
 * TRANSMITTER ID CHARACTERISITC
 * 
 * This characterisit holds the transmitter ID the sender
 * should relay measurements from. 
 * 
 */
static struct bt_uuid_128 TRANS_ID_CHAR_UUID = BT_UUID_INIT_128(
    0xf0, 0xde, 0xbc, 0x9a, 0x78, 0x56, 0x34, 0x12,
    0x78, 0x56, 0x34, 0x12, 0x78, 0x56, 0x34, 0x13);


static ssize_t read_trans_id(struct bt_conn *conn, const struct bt_gatt_attr *attr,
			 void *buf, u16_t len, u16_t offset)
{
	const u32_t *value = attr->user_data;

	return bt_gatt_attr_read(conn, attr, buf, len, offset, value,
				 sizeof(*value));
}

static ssize_t write_trans_id(struct bt_conn *conn,
				const struct bt_gatt_attr *attr,
				const void *buf, u16_t len, u16_t offset,
				u8_t flags)
{
	u32_t *value = attr->user_data;

    if (offset + len > sizeof(trans_id_value)) {
		return BT_GATT_ERR(BT_ATT_ERR_INVALID_OFFSET);
	}

	memcpy(value + offset, buf, len);

	return len;
}

/**
 * 
 * 
 * TIME CHARACTERISITC
 * 
 * Used to set the real time, so the sender can adjust
 * the timestamp on measurements 
 * 
 */
static struct bt_uuid_128 TIME_CHAR_UUID = BT_UUID_INIT_128(
    0xf0, 0xde, 0xbc, 0x9a, 0x78, 0x56, 0x34, 0x12,
    0x78, 0x56, 0x34, 0x12, 0x78, 0x56, 0x34, 0x14);
    

static ssize_t read_time(struct bt_conn *conn, const struct bt_gatt_attr *attr,
			 void *buf, u16_t len, u16_t offset)
{
	const ble_time_t *value = attr->user_data;

	return bt_gatt_attr_read(conn, attr, buf, len, offset, value,
				 sizeof(*value));
}

static ssize_t write_time(struct bt_conn *conn,
				const struct bt_gatt_attr *attr,
				const void *buf, u16_t len, u16_t offset,
				u8_t flags)
{
	ble_time_t *value = attr->user_data;

    if (offset + len > sizeof(time_value)) {
		return BT_GATT_ERR(BT_ATT_ERR_INVALID_OFFSET);
	}

	memcpy(value + offset, buf, len);
    time_set = k_uptime_get_32();

	return len;
}


/**
 * 
 * 
 * RACP CHARACTERISITC
 * 
 * Used to set the real time, so the sender can adjust
 * the timestamp on measurements 
 * 
 */
static struct bt_uuid_128 RACP_CHAR_UUID = BT_UUID_INIT_128(
    0xf0, 0xde, 0xbc, 0x9a, 0x78, 0x56, 0x34, 0x12,
    0x78, 0x56, 0x34, 0x12, 0x78, 0x56, 0x34, 0x15);

static struct bt_gatt_ccc_cfg rcap_ccc_cfg[10] = {};
static u8_t rcap_notify_enabled;

static void racp_ccc_cfg_changed(const struct bt_gatt_attr *attr, u16_t value)
{
    rcap_notify_enabled = (value == BT_GATT_CCC_INDICATE) ? 1 : 0;
}

static ssize_t write_racp(struct bt_conn *conn,
				const struct bt_gatt_attr *attr,
				const void *buf, u16_t len, u16_t offset,
				u8_t flags)
{
	u32_t *value = attr->user_data;

    if (offset + len > sizeof(rcap_value)) {
		return BT_GATT_ERR(BT_ATT_ERR_INVALID_OFFSET);
	}

	memcpy(value + offset, buf, len);

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
    BT_GATT_PRIMARY_SERVICE(&CMGS_SERVICE_UUID),
    BT_GATT_CHARACTERISTIC(&MEASUREMENT_CHAR_UUID.uuid,
			       BT_GATT_CHRC_READ | BT_GATT_CHRC_NOTIFY,
			       BT_GATT_PERM_READ, 
                   read_measurement, NULL, &measurement_value),
	BT_GATT_CCC(measurement_ccc_cfg, measurement_ccc_cfg_changed),
    BT_GATT_CHARACTERISTIC(&STATUS_CHAR_UUID.uuid,
			       BT_GATT_CHRC_READ,
			       BT_GATT_PERM_READ, 
                   read_status, NULL, &status_value),
    BT_GATT_CHARACTERISTIC(&TRANS_ID_CHAR_UUID.uuid,
			       BT_GATT_CHRC_READ | BT_GATT_CHRC_WRITE,
			       BT_GATT_PERM_READ | BT_GATT_PERM_WRITE, 
                   read_trans_id, write_trans_id, &trans_id_value),
    BT_GATT_CHARACTERISTIC(&TIME_CHAR_UUID.uuid,
			       BT_GATT_CHRC_READ | BT_GATT_CHRC_WRITE,
			       BT_GATT_PERM_READ | BT_GATT_PERM_WRITE, 
                   read_time, write_time, &time_value),
    BT_GATT_CHARACTERISTIC(&RACP_CHAR_UUID.uuid,
			       BT_GATT_CHRC_WRITE | BT_GATT_CHRC_NOTIFY,
			       BT_GATT_PERM_WRITE, 
                   NULL, write_racp, &rcap_value),
	BT_GATT_CCC(rcap_ccc_cfg, racp_ccc_cfg_changed),
};


static struct bt_gatt_service simple_cgms_svc = BT_GATT_SERVICE(attrs);


/*******************************************
 * 
 * 
 * Utility functions
 * 
 * 
 * ****************************************/

s16_t calculateMinFromTimeSet(u32_t timestamp) 
{
    return (timestamp - time_set) / ( 1000 * 60 );
}

void set_measurement_value(meas_record_t* ptr) 
{
    measurement_value = (ble_meas_rec_t) {
        .filtered = ptr->filtered,
        .raw = ptr->raw,
        .trans_batt = ptr->trans_batt,
        .time = calculateMinFromTimeSet(ptr->timestamp),
        .status = status_value
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
    time_value = (ble_time_t) {
        .year = 2019,
        .month = 4,
        .day = 6,
        .hour = 10,
        .minute = 43,
        .second = 30
    };
    time_set = 1;

    status_value = 1;

    measurement_value = (ble_meas_rec_t) {
        .filtered = 1,
        .raw = 2,
        .trans_batt = 100,
        .time = 0,
        .status = status_value
    };

    trans_id_value = 2;
    rcap_value = 4;

    bt_gatt_service_register(&simple_cgms_svc);
}

void cgms_failed_reading()
{
    missed_readings++;
}

void cgms_add_measurement(dexcom_package_t reading)
{
    meas_record_t* rec_ptr;
    rdb_new(&rec_ptr);

    *rec_ptr = (meas_record_t) {
        .filtered = intToSfloat(reading.filIsig),
        .raw = intToSfloat(reading.rawIsig),
        .trans_batt = reading.batLevel,
        .timestamp = reading.timestamp,
    };

    missed_readings = 0;

    set_measurement_value(rec_ptr);
}
