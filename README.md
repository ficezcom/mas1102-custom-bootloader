# MAS1102 Bare-Metal & Custom Bootloader Project

This project provides a bare-metal development framework, a custom bootloader, and firmware patching scripts for the **Maxio MAS1102** SSD controller. It allows you to run custom code directly on the controller, interact via UART, and perform low-level RAM operations.

---

## ⚠️ WARNING & DISCLAIMER

**THIS PROJECT IS FOR EDUCATIONAL AND RESEARCH PURPOSES ONLY.**
Modifying SSD firmware carries a high risk of permanently damaging (**bricking**) your hardware and causing irreversible data loss. 
* By using this code, you accept full responsibility for any damage to your hardware or loss of data.
* The original vendor firmware will be **completely overwritten/wiped** during this process. Ensure you have backups or are using a sacrificial drive.

---

## Getting Started & Configuration

### 1. Firmware Patching & Makefile Configuration


To avoid copyright issues, this repository does not include original proprietary firmware. You must provide your own valid firmware binary file and configure the build system.

Open the `Makefile` and locate the following line. Change the filename to match your original/target firmware file name:

`PATCHED = "QI\#FC\#1C.bin"`

Change this variable to match your specific original firmware filename

The patching scripts will use this file as the base. Memory addresses are standardized for the MAS1102 architecture, so no further address modifications should be required under normal circumstances.


###  2. Flashing the Patched Firmware

To upload the modified firmware to the SSD controller:

Open any up-to-date MPTool (Mass Production Tool) compatible with MAS1102.

* Configure your standard MPT settings as you normally would.

* Check/tick the Download checkbox in the tool.

* Flash the patched binary. (⚠️Reminder: This action erases the original factory firmware⚠️).

### Usage & Interaction
UART Communication
After successfully flashing and powering on the drive, connect to the controller's UART interface using a serial terminal (e.g., PuTTY, Minicom) configured with the correct baud rate.

Upon boot, you should see the string `Start` printed on the UART console.

Once this log appears, the device enters an interactive mode and is ready to accept low-level commands over the serial port.


## Bootloader Automation Script
We have provided a Python utility script (bootloader-comm.py) to interact with the custom bootloader seamlessly without manually typing raw serial commands.

Using bootloader-comm.py, you can perform the following operations:

Read Memory: Read data directly from specific RAM addresses.
Write & Execute: Write custom payloads/code into the RAM and trigger execution.
Memory Dump: Dump segments of the memory for analysis.


`bootloader-comm.py after 197 line, you can see how to use`



This project is licensed under the MIT License
