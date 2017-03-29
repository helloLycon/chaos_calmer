#
# Copyright (C) 2016 OpenWrt.org
#
# This is free software, licensed under the GNU General Public License v2.
# See /LICENSE for more information.
#

define Profile/ABB300
        NAME:=Airbat B300 reference board
        PACKAGES:= \
		kmod-usb2 kmod-usb-serial kmod-usb-serial-option \
		kmod-usb-storage kmod-i2c-core kmod-i2c-gpio \
		kmod-fs-ext4 kmod-nls-utf8 fdisk e2fsprogs \
		comgt-ncm usb-modeswitch usbreset usbutils tcpdump-mini \
		wireless-tools 
		
endef

define Profile/ABB300/Description
        Package set optimized for the Airbat B300 reference board
endef
$(eval $(call Profile,ABB300))

define Profile/ABC500
        NAME:=Airbat C500 reference board
        PACKAGES:= \
		kmod-usb2 kmod-usb-serial kmod-usb-serial-option \
		kmod-usb-storage kmod-ath10k pciutils\
		kmod-fs-ext4 kmod-nls-utf8 fdisk e2fsprogs wireless-tools\
		comgt-ncm usb-modeswitch usbreset usbutils tcpdump-mini \

endef

define Profile/ABC500/Description
        Package set optimized for the Airbat C500 reference board
endef
$(eval $(call Profile,ABC500))

