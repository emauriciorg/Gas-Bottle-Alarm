// #include "main_app.h"
// #include "../terminal/terminal.h"

// void startMainApp(void *parameters)
// {
//     long initial_time = micros();

//     //* 1. Await binary semaphore
//     xSemaphoreTake(app.rtos.start_main, portMAX_DELAY);

//     TerminalMessage pairing_debug_message = TerminalMessage("Starting main app", "MAI", INFO, micros());
//     addDebugMessageToQueue(&pairing_debug_message);

//     //* 2. Begin Accelerometer
//     initial_time = micros();

//     if (!accelerometer.begin(0x18))
//     {
//         terminal.printMessage(TerminalMessage("Error initializing accelerometer",
//                                               "SLE", INFO, micros()));
//         terminal.end();

//         esp.uart0.println("\n\n");
//         esp.uart0.print("Power off");

//         esp_deep_sleep_start();
//     }

//     TerminalMessage accel_init_message = TerminalMessage("Accelerometer initialized",
//                                                          "MAI", INFO, micros(), micros() - initial_time);
//     addDebugMessageToQueue(&accel_init_message);

//     //* 3. Set update frequency of accelerometer
//     accelerometer.setDataRate(LIS3DH_DATARATE_10_HZ);

//     //* 4. Read Z axis value
//     sensors_event_t event;
//     accelerometer.getEvent(&event);

//     TerminalMessage bottle_state_message;

//     if (event.acceleration.z > 0) // More than 0 is OFF positon
//     {
//         bottle_state_message = TerminalMessage("Bottle is closed", "MAI", INFO, micros());
//         addDebugMessageToQueue(&bottle_state_message);
//     }
//     else // Less than 0 is ON position
//     {
//         bottle_state_message = TerminalMessage("Bottle is open", "MAI", INFO, micros());
//         addDebugMessageToQueue(&bottle_state_message);
//     }

//     vTaskDelete(NULL);
// }