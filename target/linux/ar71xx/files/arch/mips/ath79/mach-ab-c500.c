/*
 *  AIRBAT C500
 *
 *  Copyright (C) 2017 zhangjun <282272497@qq.com>
 *
 *  This program is free software; you can redistribute it and/or modify it
 *  under the terms of the GNU General Public License version 2 as published
 *  by the Free Software Foundation.
 */

#include <linux/gpio.h>
#include <linux/platform_device.h>

#include <asm/mach-ath79/ath79.h>
#include <asm/mach-ath79/ar71xx_regs.h>

#include "common.h"
#include "dev-eth.h"
#include "pci.h"
#include "dev-usb.h"
#include "dev-m25p80.h"
#include "dev-wmac.h"
#include "machtypes.h"
#include "pci-ab9531.h"

static const char *ab_c500_part_probes[] = {
        "airbat",
        NULL,
};

static struct flash_platform_data ab_c500_flash_data = {
        .part_probes    = ab_c500_part_probes,
};

static void __init ab_c500_setup(void)
{
	u8 *mac = (u8 *) KSEG1ADDR(0x1f01fc00);
	u8 *ee = (u8 *) KSEG1ADDR(0x1fff1000);
	u8 tmpmac[ETH_ALEN];

	ath79_register_m25p80(&ab_c500_flash_data);

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
	
	/* usb */
	ath79_register_usb();
	
	/* pcie */
	ab_pcie_init();
	ath79_register_pci();
}

MIPS_MACHINE(ATH79_MACH_AB_C500, "C500", "AIRBAT C500 reference board",
	     ab_c500_setup);
