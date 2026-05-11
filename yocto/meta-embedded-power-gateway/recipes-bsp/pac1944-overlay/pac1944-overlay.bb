SUMMARY = "PAC1944 Device Tree Overlay for Raspberry Pi"
DESCRIPTION = "Device Tree overlay enabling PAC1944 devices on Raspberry Pi I2C bus"
LICENSE = "MIT"
LIC_FILES_CHKSUM = "file://${COMMON_LICENSE_DIR}/MIT;md5=0835ade698e0bcf8506ecda2f7b4f302"

SRC_URI = "file://pac1944-overlay.dts"

S = "${WORKDIR}"

DEPENDS += "dtc-native"

do_compile() {
    dtc -@ -I dts -O dtb -o pac1944-overlay.dtbo pac1944-overlay.dts
}

do_install() {
    install -d ${D}/boot/overlays
    install -m 0644 pac1944-overlay.dtbo ${D}/boot/overlays/
}

FILES:${PN} += "/boot/overlays/pac1944-overlay.dtbo"
