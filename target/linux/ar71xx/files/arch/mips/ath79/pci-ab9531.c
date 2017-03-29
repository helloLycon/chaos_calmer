#include <linux/kernel.h>
#include <linux/bitops.h>
#include <asm/mach-ath79/ar71xx_regs.h>
#include <asm/mach-ath79/ath79.h>
#include <linux/delay.h>
#include "pci-ab9531.h"

static void __iomem *ath79_pcie_base;

void ab_pcie_pll_init(void)
{
	u32 nfrac_max, nfrac_min, nint, reg;
	u32 ref_div = 1;

	u32 bootstrap;
	/*ref clock 40M or 25M*/
	bootstrap = ath79_reset_rr(QCA953X_RESET_REG_BOOTSTRAP);
	if (bootstrap &	QCA953X_BOOTSTRAP_REF_CLK_40) {
	    nint = 29;
	    ref_div = 2;
	}
	else
	    nint = 23;

	/* Power down and bypass PLL, setup ref_div */
	reg = QCA_PLL_PCIE_PLL_CFG_PLLPWD_MASK |
	      QCA_PLL_PCIE_PLL_CFG_BYPASS_MASK |
	      (ref_div << QCA_PLL_PCIE_PLL_CFG_REFDIV_SHIFT);

	ath79_pll_wr(QCA953X_PLL_PCIE_CFG_REG,reg);
	udelay(100);

	/*
	 * Use dithering/SSC (spread spectrum clocking)
	 * as in QC/A SDK (for minimize/reduce EMI)
	 */
	nfrac_max = CONFIG_QCA_PCIE_PLL_NFRAC_MAX_VAL
		    << QCA_PLL_PCIE_PLL_DITHER_MAX_NFRAC_MAX_SHIFT;
	nfrac_max = nfrac_max & QCA_PLL_PCIE_PLL_DITHER_MAX_NFRAC_MAX_MASK;

	nfrac_min = CONFIG_QCA_PCIE_PLL_NFRAC_MIN_VAL
		    << QCA_PLL_PCIE_PLL_DITHER_MIN_NFRAC_MIN_SHIFT;
	nfrac_min = nfrac_max & QCA_PLL_PCIE_PLL_DITHER_MIN_NFRAC_MIN_MASK;

	reg = QCA_PLL_PCIE_PLL_DITHER_MAX_DITHER_EN_MASK |
	      QCA_PLL_PCIE_PLL_DITHER_MAX_USE_MAX_MASK |
	      (nint << QCA_PLL_PCIE_PLL_DITHER_MAX_NINT_MAX_SHIFT) | nfrac_max;
	ath79_pll_wr(QCA953X_PLL_PCIE_DITHER_DIV_MAX_REG, reg);

	reg = (nint << QCA_PLL_PCIE_PLL_DITHER_MIN_NINT_MIN_SHIFT) | nfrac_min;
	ath79_pll_wr(QCA953X_PLL_PCIE_DITHER_DIV_MIX_REG, reg);

	/* Power up PLL ... */
    //	qca_soc_reg_read_clear(QCA953X_PLL_PCIE_CFG_REG,
    //			       QCA_PLL_PCIE_PLL_CFG_PLLPWD_MASK);
	ath79_pll_wr((QCA953X_PLL_PCIE_CFG_REG), (ath79_pll_rr((QCA953X_PLL_PCIE_CFG_REG)) & ~(QCA_PLL_PCIE_PLL_CFG_PLLPWD_MASK)));
	udelay(100);

	/* ... and wait for update complete */
	do {
	    udelay(10);
	    reg = ath79_pll_rr(QCA953X_PLL_PCIE_CFG_REG);
	} while (reg & QCA_PLL_PCIE_PLL_CFG_UPDATING_MASK);

	/* Disable PLL bypassing */
	ath79_pll_wr((QCA953X_PLL_PCIE_CFG_REG), (ath79_pll_rr((QCA953X_PLL_PCIE_CFG_REG)) & ~(QCA_PLL_PCIE_PLL_CFG_BYPASS_MASK)));
	udelay(100);
}

/*
 * Takes out PCIE0 RC from reset
 */
static void qca_pcie0_rst_di(void)
{
	/* Take PCIE PHY and core out of reset */
	ath79_device_reset_clear(QCA953X_RESET_PCIE | QCA953X_RESET_PCIE_PHY);
	mdelay(10);

	/* Enable LTSSM */
	__raw_writel((__raw_readl(QCA_PCIE_RC0_CTRL_APP_REG) | (QCA_PCIE_RCX_CTRL_APP_LTSSM_EN_MASK)),QCA_PCIE_RC0_CTRL_APP_REG);
	/* Take EP out of reset (this sets PERSTn high) */
	__raw_writel(QCA_PCIE_RCX_CTRL_RST_EP_RST_L_MASK,QCA_PCIE_RC0_CTRL_RST_REG);
	mdelay(10);
}


static void qca_pcie0_rst_en(void)
{
	/* Reset PCIE PHY and core */
	ath79_device_reset_set(QCA953X_RESET_PCIE | QCA953X_RESET_PCIE_PHY);
	mdelay(10);

	/* Put EP in reset (this sets PERSTn low) */
	__raw_writel(0x0,QCA_PCIE_RC0_CTRL_RST_REG);
	mdelay(10);
}

void ab_pcie_init(void)
{
	if (soc_is_qca953x()){
		printk("start ab_pcie_init ...\n");
		ath79_pcie_base = ioremap_nocache(QCA953X_PCI_CTRL_BASE0,AR724X_PCI_CTRL_SIZE-1);
		ab_pcie_pll_init();
		qca_pcie0_rst_en();
		qca_pcie0_rst_di();
		iounmap(ath79_pcie_base);
	}
}
EXPORT_SYMBOL_GPL(ab_pcie_init);

