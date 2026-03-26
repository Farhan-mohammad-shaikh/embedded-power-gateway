SUMMARY = "Power Gateway Application"
DESCRIPTION = "MQTT-based sensor gateway for Raspberry Pi"
LICENSE = "MIT"
LIC_FILES_CHKSUM = "file://${COMMON_LICENSE_DIR}/MIT;md5=0835ade698e0bcf8506ecda2f7b4f302"
SRC_URI = "file://Makefile \
           file://main.c \
           file://app_config.c \
           file://led_ctrl.c \
           file://logger.c \
           file://mqtt_client.c \
           file://pac1944.c \
           file://app_config.h \
           file://led_ctrl.h \
           file://logger.h \
           file://main.h \
           file://mqtt_client.h \
           file://pac1944.h \
           file://power-gateway.conf \
           file://power-gateway.service \
"
S = "${WORKDIR}"

do_compile() {
    oe_runmake
}
do_install() {
    install -d ${D}${bindir}
    install -m 0755 power-gateway ${D}${bindir}/power-gateway

    install -d ${D}${sysconfdir}/power-gateway
    install -m 0644 power-gateway.conf ${D}${sysconfdir}/power-gateway/power-gateway.conf

    install -d ${D}${systemd_system_unitdir}
    install -m 0644 power-gateway.service ${D}${systemd_system_unitdir}/power-gateway.service
}

inherit systemd

DEPENDS = "mosquitto"

SYSTEMD_SERVICE:${PN} = "power-gateway.service"
SYSTEMD_AUTO_ENABLE = "enable"
