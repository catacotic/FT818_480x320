# FT-818-Touch Display
This is an external device designed to manage a set of features of the FT-818 transceiver. The system communicates with the radio by sending and receiving data through the serial communication port (ACC output). The module includes in a single enclosure: an ESP32 microcontroller (MCU), a 4-inch touchscreen, an internal LiPo battery, and several additional circuits (ON/OFF control, battery charger, external tuner output).

When the module is powered solely by the internal battery, it offers more than 3 hours of continuous operation (1100 mAh bat). This makes it suitable for portable operations (SOTA, POTA, contests, etc.). It also includes a charging control system that allows the module to be powered and the battery charged simultaneously by connecting it to an external power source (standard USB-C charger).

The microcontroller manages both communication with the transceiver and the 4-inch touchscreen. The system is based on the project developed by Peter Jonas (DL5DLA), available on GitHub [FT-817 Touch Display](https://github.com/dl5dla/FT-817-TouchDisplay), to which several improvements have been added:

Control circuit and functions for detecting the transceiver’s power state (on/off). In the original project, there was no power state detection, and the startup sequence had to be followed strictly: first power on the transceiver, then power the module. Also, turning off the transceiver with the module connected caused incorrect values to be displayed until it entered a cycle of repeated resets. The current circuit detects the transceiver's power state by reading the voltage from the analog band output, also available on the ACC connector.

Adaptation to a 4-inch display with a resolution of 480 x 320 pixels. The original project software was designed for 320x200 resolution, limiting its use to touchscreens of up to 3.2 inches.

Output for connecting the device to an external antenna tuner (such as ATU100 or similar), allowing activation during antenna tuning.

Power circuitry using an internal battery, enabling simultaneous charging and battery status monitoring.

From the many configuration functions available on the Yaesu FT-818, the most commonly used and/or most tedious to set up via the transceiver's own menu system have been selected. A single screen provides access to information and control of multiple configuration parameters.
