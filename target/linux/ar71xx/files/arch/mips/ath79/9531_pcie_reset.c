#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/types.h>
#include <linux/fcntl.h>
#include <linux/miscdevice.h>
#include <asm/mach-ath79/ar71xx_regs.h>
#include <asm/mach-ath79/ath79.h>
#include <linux/init.h>
#include <linux/fs.h>
#include <linux/major.h>
#include <linux/blkdev.h>
#include <linux/module.h>
#include <linux/raw.h>
#include <linux/capability.h>
#include <linux/uio.h>
#include <linux/cdev.h>
#include <linux/device.h>
#include <linux/mutex.h>
#include <linux/gfp.h>
#include <linux/compat.h>
#include <linux/vmalloc.h>
#include <linux/delay.h>
#include "pci-ab9531.h"

static void __iomem *ath79_pcie_base;
#define QCA_PCIE_RC0_CTRL_RST_REG		(ath79_pcie_base + 0x18)

#define QCA_PCIE_RCX_CTRL_RST_EP_RST_L_SHIFT		2
#define QCA_PCIE_RCX_CTRL_RST_EP_RST_L_MASK		BIT(QCA_PCIE_RCX_CTRL_RST_EP_RST_L_SHIFT)

static ssize_t reset_pcie_write(struct file *file, const char __user *userbuf,
                     size_t bytes, loff_t *off)
{
		char sw=0;
		int ret;
		ret=copy_from_user(&sw,userbuf,1);
		printk("%c\n",sw);
		ath79_pcie_base = ioremap_nocache(QCA953X_PCI_CTRL_BASE0,AR724X_PCI_CTRL_SIZE-1);
		if (ath79_pcie_base == NULL)
			return -ENOMEM;
		if(sw == '0'){
				/* Put EP in reset (this sets PERSTn low) */
				__raw_writel(0x0,QCA_PCIE_RC0_CTRL_RST_REG);
				printk("pcie reset low \n");
		}
		else{   
				/* Take EP out of reset (this sets PERSTn high) */
				__raw_writel(QCA_PCIE_RCX_CTRL_RST_EP_RST_L_MASK,QCA_PCIE_RC0_CTRL_RST_REG);
				printk("pcie reset high \n");
		}
		
		iounmap(ath79_pcie_base);
		return bytes;
}




static struct file_operations pcie_reset_fops = {
        .owner  =  THIS_MODULE,
        .write  =  reset_pcie_write,
};

static struct miscdevice pcie_reset_dev = {
	MISC_DYNAMIC_MINOR,
	"pcie_reset",
	&pcie_reset_fops
};

static int __init reset_pcie_init(void)
{
		if (misc_register(&pcie_reset_dev))
			return -1;
		return 0;
}

static void __exit reset_pcie_exit(void)
{
		misc_deregister(&pcie_reset_dev);
}

module_init(reset_pcie_init);
module_exit(reset_pcie_exit);

MODULE_LICENSE("GPL");
MODULE_AUTHOR("zhangjun");
MODULE_DESCRIPTION("pcie reset channel");
MODULE_SUPPORTED_DEVICE("c500 for celing");

