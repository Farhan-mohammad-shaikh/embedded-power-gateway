# Embedded Power Gateway

Custom Yocto-Based Embedded Linux System for Real-Time Multi-Rail Power Monitoring using Raspberry Pi, Linux Kernel Driver Development, Device Tree Integration, and MQTT Telemetry.

---

## 1. Introduction

This project implements a production-style Embedded Linux power monitoring gateway built on Raspberry Pi 5 using the Yocto Project.

The system performs real-time acquisition of electrical parameters from multiple Microchip PAC1944 power monitoring ICs connected over I²C. Sensor telemetry is processed inside a custom Linux userspace application and published over MQTT for remote monitoring and control.

The project demonstrates a complete Embedded Linux workflow including:

* Yocto Linux image customization
* Linux kernel module development
* Device Tree Overlay integration
* sysfs-based kernel/userspace communication
* systemd service management
* MQTT telemetry infrastructure
* Embedded C application development
* Raspberry Pi BSP integration

The focus of this work is not only application development, but complete Linux-based embedded system integration from hardware bring-up to deployment.

---

## 2. System Architecture

### 2.1 Functional Overview

```text
                  +------------------------+
                  |     MQTT Broker        |
                  |   (External System)    |
                  +-----------+------------+
                              |
                              | MQTT
                              |
+-----------------------------v-----------------------------+
|                Raspberry Pi 5 (Yocto Linux)              |
|----------------------------------------------------------|
| Userspace                                                |
|  - power-gateway application                             |
|  - MQTT communication (libmosquitto)                    |
|  - systemd watchdog integration                         |
|  - LED control subsystem                                |
|                                                          |
| Linux Kernel                                             |
|  - Custom PAC1944 I2C kernel driver                     |
|  - sysfs interface                                      |
|  - I2C subsystem                                        |
|  - Device Tree overlay                                  |
|                                                          |
| Yocto Integration                                        |
|  - Custom layer: meta-embedded-power-gateway            |
|  - BitBake recipes                                      |
|  - BSP customization                                    |
+-----------------------------+----------------------------+
                              |
                              | I²C Bus
                              |
        +---------------------+---------------------+
        |                 PAC1944 Devices           |
        +---------------------+---------------------+
                              |
                     Power Rail Measurements
```

---

## 3. Hardware Design

* Raspberry Pi 5 configured as I²C master
* Three PAC1944 power monitoring ICs
* I²C communication bus
* Precision shunt resistors
* Linux-based embedded target platform

The Raspberry Pi acts as the I²C master while the PAC1944 devices perform current, voltage, and power measurements across multiple rails.

---

## 4. Software Architecture

### 4.1 Application Design

The userspace gateway application is implemented in C with modular separation:

* `main.c` – system orchestration
* `pac1944.c` – sysfs sensor access
* `mqtt_client.c` – MQTT communication
* `led_ctrl.c` – LED runtime control
* `logger.c` – logging abstraction

The design follows a multi-threaded model:

* telemetry acquisition
* MQTT communication
* LED control handling
* watchdog servicing

This architecture prevents blocking behavior between telemetry and communication paths.

---

### 4.2 Data Acquisition

The PAC1944 driver exports raw sensor values through sysfs interfaces.

The userspace application reads these values and converts them into electrical measurements.

```text
VBUS (V)   = 9.0 × (VBUS_raw / 65536)
VSENSE (V) = 0.1 × (VSENSE_raw / 65536)
Current (A)= VSENSE / Rsense
Power (W)  = VBUS × Current
```

Where:

* Rsense = 1 Ω
* ADC resolution = 16-bit

All conversions are performed explicitly in software to maintain full control over scaling and precision.

---

### 4.3 MQTT Communication

Telemetry is published in structured JSON format:

```json
{
  "channel": 1,
  "vbus_raw": 40120,
  "vsense_raw": 122,
  "vpower_raw": 412001,
  "vbus_V": 5.012,
  "current_A": 0.003,
  "power_W": 0.015
}
```

The system also subscribes to runtime control topics for external interaction such as LED control commands.

---

## 5. Linux Kernel Driver Development

A custom PAC1944 Linux kernel module was developed for hardware integration.

### 5.1 Driver Features

* I²C client driver implementation
* Device Tree matching
* sysfs interface creation
* automatic driver probing
* kernel logging using `dev_info()`
* PAC1944 register acquisition

---

### 5.2 sysfs Interface

The driver exports measurement data through sysfs:

```text
/sys/bus/i2c/devices/1-0010/vbus_raw
/sys/bus/i2c/devices/1-0010/vsense_raw
/sys/bus/i2c/devices/1-0010/vpower_raw
```

The userspace application accesses sensor data through these kernel interfaces.

This demonstrates a production-style Linux driver architecture:

```text
Hardware -> Kernel Driver -> sysfs -> Userspace Application
```

---

## 6. Device Tree Overlay Integration

A custom Device Tree Overlay was implemented to enable PAC1944 devices on the Raspberry Pi I²C bus.

### 6.1 Overlay Features

* I²C bus enablement
* PAC1944 node registration
* Device compatibility matching
* automatic kernel driver probing

### Example Device Tree Node

```dts
pac1944@10 {
    compatible = "microchip,pac1944-custom";
    reg = <0x10>;
    status = "okay";
};
```

The overlay is compiled into a `.dtbo` file during the Yocto build process and automatically loaded during boot.

---

## 7. Embedded Linux Integration

### 7.1 Yocto Configuration

* Base distribution: Poky (Kirkstone)
* BSP layer: meta-raspberrypi
* Additional layers: meta-openembedded
* Custom layer: `meta-embedded-power-gateway`

The system is built entirely from source using BitBake.

---

### 7.2 Application Packaging

The application is integrated via a custom BitBake recipe:

```text
recipes-power-gateway/power-gateway.bb
```

The recipe:

* cross-compiles the application
* installs binary to `/usr/bin`
* installs configuration to `/etc/power-gateway`
* installs systemd unit file
* enables automatic startup at boot

---

### 7.3 Kernel Driver Recipe

The PAC1944 kernel module is integrated using a dedicated BitBake recipe:

```text
recipes-kernel/pac1944-driver.bb
```

The recipe:

* builds external kernel modules
* installs `.ko` driver module
* enables automatic module loading at boot

---

### 7.4 Device Tree Overlay Recipe

The PAC1944 Device Tree Overlay is integrated through:

```text
recipes-bsp/pac1944-overlay.bb
```

The recipe:

* compiles `.dts` into `.dtbo`
* deploys overlay into Raspberry Pi boot partition
* enables overlay loading during boot

---

### 7.5 systemd Service Integration

The application runs as a managed systemd service.

Features include:

* automatic startup
* restart-on-failure policy
* watchdog monitoring
* runtime supervision
* journal logging

```bash
systemctl status power-gateway
journalctl -u power-gateway -f
```

---

## 8. Deployment Workflow

1. Build Yocto image using BitBake
2. Generate `.wic` image
3. Flash image to SD card
4. Boot Raspberry Pi
5. Load Device Tree overlay
6. Auto-load PAC1944 kernel driver
7. Start systemd service
8. Publish telemetry over MQTT

---

## 9. Key Engineering Aspects

* Embedded Linux development
* Yocto Project integration
* BitBake recipe development
* Linux kernel module development
* Device Tree overlays
* I²C subsystem integration
* sysfs driver architecture
* systemd service management
* MQTT networking
* POSIX threading
* Embedded C programming
* Raspberry Pi BSP customization
* Cross-compilation workflows
* Hardware bring-up and debugging

---

## 10. Future Work

* migration to Industrial I/O (IIO) subsystem
* dynamic runtime configuration
* improved fault handling and retry logic
* persistent telemetry logging
* secure MQTT communication (TLS)
* cloud dashboard integration
* performance profiling and optimization

---

## 11. Author

Farhan Mohammad Shaikh
