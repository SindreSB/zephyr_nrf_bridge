#include <zephyr.h>
#include <misc/printk.h>
#include <zephyr/types.h>
#include <settings/settings.h>
#include <stddef.h>
#include <string.h>
#include <errno.h>
#include <misc/byteorder.h>

#include <bluetooth/bluetooth.h>
#include <bluetooth/hci.h>
#include <bluetooth/conn.h>
#include <bluetooth/uuid.h>
#include <bluetooth/gatt.h>
#include <bluetooth/controller.h>

#include <autoconf.h>

#include <device.h>
#include <gpio.h>

#include "dexcom/receiver.h"
#include "gatt/services/cgms.h"

/**********************************
 * 
 * 
 * BLE
 * 
 * 
 * *******************************/

struct bt_conn *default_conn;

static const struct bt_data ad[] = {
	BT_DATA_BYTES(BT_DATA_FLAGS, (BT_LE_AD_GENERAL | BT_LE_AD_NO_BREDR)),
	BT_DATA_BYTES(BT_DATA_UUID16_ALL, 0x0d, 0x18, 0x0f, 0x18, 0x05, 0x18),
};

static void connected(struct bt_conn *conn, u8_t err)
{
	if (err) {
		printk("Connection failed (err %u)\n", err);
	} else {
		default_conn = bt_conn_ref(conn);
		printk("Connected\n");
		cgms_send_all();
	}
}

static void disconnected(struct bt_conn *conn, u8_t reason)
{
	printk("Disconnected (reason %u)\n", reason);

	if (default_conn) {
		bt_conn_unref(default_conn);
		default_conn = NULL;
	}
}

static struct bt_conn_cb conn_callbacks = {
	.connected = connected,
	.disconnected = disconnected,
};

static void bt_ready(int err)
{
	if (err) {
		printk("Bluetooth init failed (err %d)\n", err);
		return;
	}

	printk("Bluetooth initialized\n");

	cgms_init();

	err = bt_le_adv_start(BT_LE_ADV_CONN_NAME, ad, ARRAY_SIZE(ad), NULL, 0);
	if (err) {
		printk("Advertising failed to start (err %d)\n", err);
		return;
	}

	printk("Advertising successfully started\n");
}

static void auth_cancel(struct bt_conn *conn)
{
	char addr[BT_ADDR_LE_STR_LEN];

	bt_addr_le_to_str(bt_conn_get_dst(conn), addr, sizeof(addr));

	printk("Pairing cancelled: %s\n", addr);
}

static struct bt_conn_auth_cb auth_cb_display = {
	.cancel = auth_cancel,
};

/*****************************************
 *
 * 
 * 
 * Dexcom receiver
 * 
 * 
 * 
 * **************************************/

void onDexcomPackageReceived(dexcom_package_t *pkt) {
	printk("Timestamp: %d\n", pkt->timestamp);
	printk("Raw: %d\n", pkt->rawIsig);
	printk("Fil: %d\n", pkt->filIsig);

	cgms_add_measurement(*pkt);
}

#ifdef CONFIG_CC2500
DEXCOM_RECEIVER(dexcom_receiver_ctx);
DEXCOM_RECEIVER_TIMER(dexcom_receiver_ctx);
DEXCOM_INTERRUPT_HANDLER(dexcom_receiver_ctx);

dexcom_package_t *pkt;
void init_dexcom() {
    dexcom_receiver_ctx.timeout_timer = &dexcom_receiver_ctx_timer;
    dexcom_receiver_ctx.gdo0_interrupt_handler = &dexcom_receiver_ctx_int_handler;
	dexcom_receiver_ctx.package_callback = onDexcomPackageReceived;
}

void run_cgm() {
	init_dexcom();
    start_cc2500(&dexcom_receiver_ctx);
}
#else
void init_dexcom() {}

void run_cgm() {
	dexcom_package_t simulated_package = {
		.timestamp = 0,
		.transmitterId = 0x01,
		.rawIsig = 72000,
		.filIsig =72000,
		.batLevel = 214
	};

    while(1) {
        k_sleep(K_SECONDS(CONFIG_CC2500_SIMULATED_TRANS_SPACING));

		simulated_package.timestamp = k_uptime_get_32();
		
		cgms_add_measurement(simulated_package);

    }
}
#endif

void main(void)
{
	u8_t addr[6] = {0x64, 0x40, 0x59, 0x22, 0xd2, 0xe7};
	bt_ctlr_set_public_addr(addr);

    int err;
	err = bt_enable(bt_ready);
	if (err) {
		printk("Bluetooth init failed (err %d)\n", err);
		return;
	}

	bt_conn_cb_register(&conn_callbacks);
    bt_conn_auth_cb_register(&auth_cb_display);

	run_cgm();

}
