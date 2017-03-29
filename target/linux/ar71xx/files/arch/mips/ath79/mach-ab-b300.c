/*
 *  Airbat B300
 *
 *  Copyright (C) 2017 zhangjun <282272497@qq.com>
 *
 *  This program is free software; you can redistribute it and/or modify it
 *  under the terms of the GNU General Public License version 2 as published
 *  by the Free Software Foundation.
 */

#include <linux/gpio.h>
#include <linux/platform_device.h>

#include <linux/i2c.h>
#include <linux/i2c-gpio.h>

#include <asm/mach-ath79/ath79.h>
#include <asm/mach-ath79/ar71xx_regs.h>

#include "common.h"
#include "dev-eth.h"
#include "dev-gpio-buttons.h"
#include "dev-m25p80.h"
#include "dev-wmac.h"
#include "dev-usb.h"
#include "machtypes.h"

#define TP_WR841NV9_GPIO_I2C_SDA 2
#define TP_WR841NV9_GPIO_I2C_SCL 1

#define AB_B300_GPIO_BTN_RESET	14
#define AB_B300_GPIO_BTN_WIFI	16

#define AB_B300_KEYS_POLL_INTERVAL	20	/* msecs */
#define AB_B300_KEYS_DEBOUNCE_INTERVAL (3 * AB_B300_KEYS_POLL_INTERVAL)

static const char *ab_b300_part_probes[] = {
	"airbat",
	NULL,
};

static struct flash_platform_data ab_b300_flash_data = {
	.part_probes	= ab_b300_part_probes,
};

static struct gpio_keys_button ab_b300_gpio_keys[] __initdata = {
	{
		.desc		= "Reset button",
		.type		= EV_KEY,
		.code		= KEY_RESTART,
		.debounce_interval = AB_B300_KEYS_DEBOUNCE_INTERVAL,
		.gpio		= AB_B300_GPIO_BTN_RESET,
		.active_low	= 1,
	}, {
		.desc		= "WIFI button",
		.type		= EV_KEY,
		.code		= KEY_RFKILL,
		.debounce_interval = AB_B300_KEYS_DEBOUNCE_INTERVAL,
		.gpio		= AB_B300_GPIO_BTN_WIFI,
		.active_low	= 1,
	}
};

/* support for i2c mode*/
static struct i2c_gpio_platform_data ab_b300_i2c_gpio_data = { 
	.sda_pin    = TP_WR841NV9_GPIO_I2C_SDA,
	.scl_pin    = TP_WR841NV9_GPIO_I2C_SCL,

};

static struct platform_device ab_b300_i2c_gpio_device = {
	.name   = "i2c-gpio",
	.id	= 0,
	.dev = {
		.platform_data	= &ab_b300_i2c_gpio_data,
	}
};

static void __init ab_b300_setup(void)
{
	u8 *mac = (u8 *) KSEG1ADDR(0x1f01fc00);
	u8 *ee = (u8 *) KSEG1ADDR(0x1fff1000);
	u8 tmpmac[ETH_ALEN];

	ath79_register_m25p80(&ab_b300_flash_data);

	ath79_setup_ar933x_phy4_switch(false, false);

	ath79_register_mdio(0, 0x0);

	/* LAN */
	ath79_eth1_data.phy_if_mode = PHY_INTERFACE_MODE_GMII;
	ath79_eth1_data.duplex = DUPLEX_FULL;
	ath79_switch_data.phy_poll_mask |= BIT(4);
	ath79_init_mac(ath79_eth1_data.mac_addr, mac, 0);
	ath79_register_eth(1);

	/* WAN */
	ath79_switch_data.phy4_mii_en = 1;
	ath79_eth0_data.phy_if_mode = PHY_INTERFACE_MODE_MII;
	ath79_eth0_data.duplex = DUPLEX_FULL;
	ath79_eth0_data.speed = SPEED_100;
	ath79_eth0_data.phy_mask = BIT(4);
	ath79_init_mac(ath79_eth0_data.mac_addr, mac, 1);
	ath79_register_eth(0);

	ath79_init_mac(tmpmac, mac, 0);
	ath79_register_wmac(ee, tmpmac);


	platform_device_register(&ab_b300_i2c_gpio_device);
	ath79_register_usb();

	ath79_register_gpio_keys_polled(1, AB_B300_KEYS_POLL_INTERVAL,
					ARRAY_SIZE(ab_b300_gpio_keys),
					ab_b300_gpio_keys);
}

MIPS_MACHINE(ATH79_MACH_AB_B300, "B300", "AIRBAT B300 reference board",
	     ab_b300_setup);
