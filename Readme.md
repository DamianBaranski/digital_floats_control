
# Digital Floats Control System

The **Digital Floats Control System** is a device designed to manage and control the gear mechanisms in airplane floats, ensuring smooth transitions between landing on land or water. This document outlines the system's operational procedures, including self-test routines, test procedures, and LED indicator states.

## System Overview

The device features two switches and five RGB LEDs. The left switch controls the gear, while the right switch controls the rudder. Four LEDs display the status of the four gears, and the fifth LED shows the status of the rudder. The device can operate in three distinct modes:

1. **Land Mode (For takeoff/landing on land):** 
   - **Gear:** Extended 
   - **Rudder:** Retracted 
   - **Switch Positions:** Gear switch down, rudder switch inactive (either up or down).

2. **Water Mode (For takeoff/landing on water):**
   - **Gear:** Retracted
   - **Rudder:** Retracted
   - **Switch Positions:** Gear switch up, rudder switch up.

3. **Maneuver Mode (For maneuvering in water):**
   - **Gear:** Retracted
   - **Rudder:** Extended
   - **Switch Positions:** Gear switch up, rudder switch down.

## Power-Up and Self-Test Procedure

Upon powering up, the Digital Floats Control System automatically performs a self-test to ensure that all critical components are functioning correctly. The results of this self-test are displayed on the RGB LEDs.

- **Error Reporting:** Any issues detected during the self-test are indicated on the RGB LEDs with a specific error code.
- **Gear State Indication:** After the self-test completes, the current gear state is displayed on the RGB LEDs.

### Self-Test Procedure

The self-test includes the following steps:

1. **I2C Device Verification:** The system verifies connections to all registered I2C devices.
2. **RTC (Real-Time Clock) Check:** The system checks the accuracy of the RTC.
3. **External Memory Verification:** The system confirms the connection to external memory.

### Operational Test Procedure

The operational test procedure ensures the system functions correctly under controlled conditions. The procedure includes:

1. **I2C Device Verification:** The system verifies connections to all registered I2C devices.
2. **Initial Voltage and Current Check:** The system reads voltage and current values from the INA219 sensor, which should display 0mV and 0mA.
3. **Relay Activation:**
   - The system activates Relay A and Relay B.
   - It then reads the voltage and current values from the INA219 sensor again. The voltage should match the power supply voltage, and the current should remain at 0mA.
4. **Relay Deactivation:** The system deactivates Relay A and Relay B.

## LED Status Indicators

The system uses RGB LEDs to indicate the current status of the gear and rudder, as well as to report any warnings or errors. The LED states are as follows:

- **Green:** Gear is extended for landing on land; rudder is retracted.
- **Blue:** Gear is retracted for landing on water; rudder is extended.
- **Yellow (Blinking):** A warning has been detected. The number of blinks corresponds to the specific warning code.
- **Red (Blinking):** An error has been detected. The number of blinks corresponds to the specific error code.

## System Connections and Setup

The device supports four actuators for the gear and one actuator for the rudder. Each actuator must be equipped with two limit switches: one for the "up" position and one for the "down" position. The device supports both Normally Open (NO) and Normally Closed (NC) limit switches. These settings can be configured for each channel using the accompanying PC application.

## PC Application

A PC application is provided to configure the device, upgrade device firmware, and download logs. Before initial use, the user must configure parameters for each channel, including:

- **Maximum and Minimum Current for the Actuator:** Set the thresholds for the actuator's operating current.
- **Maximum and Minimum Time for Actuator Movement:** Define the time limits for the actuator's movement.
- **Actuator Direction:** Specify the direction in which the actuator should move.
- **Limit Switch Assignment:** Assign which limit switch corresponds to the "down" position and which to the "up" position.
- **Limit Switch Type:** Select the type of limit switch used (Normally Open (NO) or Normally Closed (NC)).

The application provides a user-friendly interface for making these configurations, ensuring that the Digital Floats Control System is properly set up for your specific needs.
