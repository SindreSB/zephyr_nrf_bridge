#include <zephyr/types.h>
#include <stddef.h>
#include <string.h>
#include <errno.h>
#include <misc/printk.h>
#include <misc/byteorder.h>
#include <zephyr.h>

#include <settings/settings.h>

#include <bluetooth/bluetooth.h>
#include <bluetooth/hci.h>
#include <bluetooth/conn.h>
#include <bluetooth/uuid.h>
#include <bluetooth/gatt.h>

static struct bt_uuid_128 CGM_UUID = BT_UUID_INIT_128(
    0xf0, 0xde, 0xbc, 0x9a, 0x78, 0x56, 0x34, 0x12,
    0x78, 0x56, 0x34, 0x12, 0x78, 0x56, 0x34, 0x12);

static struct bt_uuid_128 GL_UUID = BT_UUID_INIT_128(
    0xf1, 0xde, 0xbc, 0x9a, 0x78, 0x56, 0x34, 0x13,
    0x78, 0x56, 0x34, 0x12, 0x78, 0x56, 0x34, 0x13);

static struct bt_gatt_ccc_cfg cgm_ccc_cfg[10] = {};
static u8_t notify_enabled;
static u32_t filtered_value;


static void cgm_ccc_cfg_changed(const struct bt_gatt_attr *attr, u16_t value)
{
    notify_enabled = (value == BT_GATT_CCC_INDICATE) ? 1 : 0;
}

static ssize_t read_cgm(struct bt_conn *conn, const struct bt_gatt_attr *attr,
			 void *buf, u16_t len, u16_t offset)
{
	const char *value = attr->user_data;

	return bt_gatt_attr_read(conn, attr, buf, len, offset, value,
				 sizeof(*value));
}

/* Battery Service Declaration */
static struct bt_gatt_attr attrs[] = {
	BT_GATT_PRIMARY_SERVICE(&CGM_UUID),
	BT_GATT_CHARACTERISTIC(&GL_UUID.uuid,
			       BT_GATT_CHRC_READ | BT_GATT_CHRC_NOTIFY,
			       BT_GATT_PERM_READ, 
                   read_cgm, NULL, &filtered_value),
	BT_GATT_CCC(cgm_ccc_cfg, cgm_ccc_cfg_changed),
};

static struct bt_gatt_service cgm_svc = BT_GATT_SERVICE(attrs);

void cgm_init(void)
{
	bt_gatt_service_register(&cgm_svc);
}

void cgm_notify(u32_t value)
{
    filtered_value = value;

    bt_gatt_notify(NULL, &attrs[1], &value, sizeof(value));
}