/**
 * @file main.cpp
 * @brief Main application entry point for the digital float control system
 *
 * This file contains the program entry point that initializes the hardware
 * abstraction layer (BSP), sets up logging, creates the main Application
 * instance, and runs the application's main loop.
 * 
 * The digital float control system manages motors for aircraft landing gear
 * and rudder control, providing status indication via LEDs and handling user
 * input through switches.
 */

#include "bsp.h"
#include "logger.h"
#include "application.h"

/** @brief Static instance pointer for the singleton UartStream */
UartStream *UartStream::mInstance = nullptr;

/**
 * @brief Program entry point
 * @return Exit code (never reached in normal operation)
 * 
 * 
 * During normal operation, the function never returns as the system
 * continuously processes user input, monitors channel states, and
 * updates outputs through the application's spin() method.
 */
int main()
{
  // Initialize hardware abstraction layer
  Bsp bsp;
  
  // Set up logging over UART
  UartStream logStream(*bsp.uartBus);

  // Log application startup
  LOG << "Application BS";
  
  // Create main application controller
  Application app(bsp);
  
  // Main control loop - never exits during normal operation
  while(true) {
    app.spin();
  }

  // Return code (not reached in normal operation)
  return 0;
}