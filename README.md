# Embedded Power Gateway

Custom Yocto-Based Embedded Linux System for Multi-Rail Power Monitoring and Control

---

## 1. Introduction

This project implements a complete embedded Linux system for real-time power monitoring and control using Raspberry Pi and Microchip PAC194x power monitoring ICs.

The system is designed to acquire electrical parameters (voltage, current, power) across multiple rails, publish telemetry over MQTT, and accept remote control commands. It is built and deployed using a custom Yocto Linux image, with the application integrated as a system service.

The work focuses on full-stack embedded development:

* hardware interfacing over I²C
* low-level data acquisition and scaling
* multi-threaded application design
* Linux system integration (systemd)
* Yocto-based OS customization and deployment

---

## 2. System Architecture

### 2.1 Functional Overview

```id="w3t6na"
                  +------------------------+
                  |     MQTT Broker        |
                  |   (External System)    |
                  +-----------+------------+
                              |
                              | MQTT (Pub/Sub)
                              |
+-----------------------------v-----------------------------+
|                Raspberry Pi (Yocto Linux)                |
|----------------------------------------------------------|
| User Space                                               |
|  - power-gateway (C application)                         |
|  - systemd service                                       |
|                                                          |
| Application Components                                   |
|  - Sensor acquisition (PAC194x over I²C)                 |
|  - MQTT client (libmosquitto)                            |
|  - GPIO control (LED)                                    |
|  - Logging subsystem                                     |
|                                                          |
| Kernel Space                                             |
|  - i2c-dev interface                                     |
|  - device tree configuration                             |
+-----------------------------+----------------------------+
                              |
                              | I²C Bus
                              |
        +---------------------+---------------------+
        |           PAC194x Devices (x3)            |
        +---------------------+---------------------+
                              |
                      Shunt Resistors (1Ω)
                              |
                     Power Rails (3.3V / 1.2V / 5V)
```

---

## 3. Hardware Design

* Raspberry Pi 5 configured as I²C master
* Three PAC194x devices for independent rail monitoring
* 1 Ω precision shunt resistors for current measurement

Each device monitors a dedicated rail. Measurements are verified against external instrumentation to validate scaling and accuracy.

---

## 4. Software Architecture

### 4.1 Application Design

The application is implemented in C with modular separation:

* `pac1944.c` – low-level I²C register access
* `mqtt_client.c` – MQTT communication
* `led_ctrl.c` – GPIO control
* `logger.c` – logging abstraction
* `main.c` – system orchestration

The design follows a multi-threaded model:

* acquisition thread: periodic sensor reads
* communication thread: MQTT publish/subscribe
* control thread: handles incoming commands

This ensures that telemetry and control paths operate independently without blocking.

---

### 4.2 Data Acquisition

The PAC194x provides raw ADC values which are converted in software.

```id="n2ffh2"
VBUS (V)   = 9.0 × (VBUS_raw / 65536)
VSENSE (V) = 0.1 × (VSENSE_raw / 65536)
Current (A)= VSENSE / Rsense
Power (W)  = VBUS × Current
```

Where:

* Rsense = 1 Ω
* ADC resolution = 16-bit

All conversions are performed explicitly to maintain full control over scaling and precision.

---

### 4.3 MQTT Communication

Telemetry is published in structured JSON:

```id="t6m1b4"
{
  "i2c_addr": "0x10",
  "channel": 1,
  "vbus_V": 5.01,
  "vsense_V": 0.0035,
  "current_A": 0.0035
}
```

Topic structure:

```id="4zh1py"
<i2c_addr>/ch<channel>
```

The system also subscribes to control topics for runtime interaction (e.g., LED control).

---

## 5. Embedded Linux Integration

### 5.1 Yocto Configuration

* Base distribution: Poky (Kirkstone)
* BSP layer: meta-raspberrypi
* Additional layers: meta-openembedded (networking, mosquitto)
* Custom layer: `meta-embedded-power-gateway`

The system is built entirely from source using BitBake.

---

### 5.2 Application Packaging

The application is integrated via a custom BitBake recipe:

```id="qzru1a"
recipes-power-gateway/power-gateway/power-gateway.bb
```

The recipe:

* compiles using Yocto toolchain
* installs binary to `/usr/bin`
* installs configuration to `/etc/power-gateway`
* installs systemd unit file
* enables service at boot

---

### 5.3 System Service

The application runs as a managed systemd service:

* automatic startup
* controlled restart policy
* runtime diagnostics via journal

```id="1lgx9m"
systemctl status power-gateway
journalctl -u power-gateway
```

---

### 5.4 Hardware Enablement

I²C support is explicitly enabled in the Yocto image:

* kernel configuration
* device tree enablement
* module availability (`i2c-dev`)

This required debugging of missing device nodes and integration at OS level.

---

## 6. Deployment Workflow

1. Build image using BitBake
2. Generate `.wic` image
3. Flash to SD card
4. Boot Raspberry Pi
5. Verify service and hardware interfaces
6. Validate telemetry over MQTT

---

## 7. Key Engineering Aspects

* Cross-compilation using Yocto toolchain
* Custom Linux image creation
* Hardware bring-up and debugging
* Integration of user-space application with OS services
* Real-time telemetry pipeline
* Concurrent processing using multi-threading

---

## 8. Future Work


* dynamic configuration via MQTT
* enhanced error handling and retry logic
* integration with cloud platforms
* performance profiling and optimization

---

## 9. Author

Farhan Mohammad Shaikh


---
