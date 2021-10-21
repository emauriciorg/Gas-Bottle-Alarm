#include "device_pairing.h"

#include "../terminal/terminal.h"
#include "../bluetooth_server/ble_server.h"
#include "../settings/settings.h"
#include "../led_strip/led_strip.h"
#include "../time_keeper/time_keeper.h"

#define PAIRING_TIMEOUT_S 30

void wirelessPairingMode()
{
    //* 1. Print App title
    esp.uart0.println("\n\n");
    esp.uart0.println("************************************************************************************************************");
    esp.uart0.println("*                                          ANZE - Gas Bottle Alarm                                         *");
    esp.uart0.println("*                                                                                                          *");
    esp.uart0.println("*                                           Wireless Pairing Mode                                          *");
    esp.uart0.println("************************************************************************************************************");
    esp.uart0.println("\n\n");

    //* 2. Setup RTOS first. Always.
    ESP_ERROR rtos_initialized = app.setupRTOS();

    if (rtos_initialized.on_error)
    {
        esp.uart0.println(rtos_initialized.debug_message);
        esp.uart0.println("\n\n");
        esp.uart0.println("Stopping boot.");
        while (1)
        {
        }
    }

    //* 3. Create tasks if RTOS was setup correctly
    xTaskCreatePinnedToCore(setupTerminal,
                            "Terminal Setup",
                            10000,
                            NULL,
                            1,
                            NULL,
                            1);

    xTaskCreatePinnedToCore(bleServerTask,
                            "BLE server Task",
                            10000,
                            NULL,
                            25,
                            NULL,
                            0);

    xTaskCreatePinnedToCore(setupLED_Strip,
                            "LED Setup",
                            1000,
                            NULL,
                            25,
                            NULL,
                            1);

    xTaskCreatePinnedToCore(setupTimeKeeper,
                            "Time Keeper",
                            10000,
                            NULL,
                            1,
                            NULL,
                            1);

    // xTaskCreatePinnedToCore(setupDeepSleepManager,
    //                         "Deep Sleep Setup",
    //                         10000,
    //                         NULL,
    //                         24,
    //                         NULL,
    //                         1);

    // xTaskCreatePinnedToCore(setupSPIFFS,
    //                         "Deep Sleep Setup",
    //                         30000,
    //                         NULL,
    //                         23,
    //                         NULL,
    //                         0);

    // xTaskCreatePinnedToCore(pairingTask,
    //                         "Pairing Task",
    //                         10000,
    //                         NULL,
    //                         22,
    //                         NULL,
    //                         0);

    // xTaskCreatePinnedToCore(startMainApp,
    //                         "Main app",
    //                         10000,
    //                         NULL,
    //                         21,
    //                         NULL,
    //                         1);

    //* 2. Start the bluetooth server task
    xSemaphoreGive(app.rtos.start_ble_server);

    vTaskDelete(NULL);
}

// void pairingTask(void *parameters)
// {
//     TerminalMessage pairing_debug_message;
//     uint8_t debug_message_queue_ticks = 0;

//     //* 1. Await binary semaphore
//     xSemaphoreTake(app.rtos.start_pairing, portMAX_DELAY);

//     pairing_debug_message = TerminalMessage("Starting pair up process", "PAI", INFO, micros());
//     addDebugMessageToQueue(&pairing_debug_message, debug_message_queue_ticks);

//     //* 2. Start the bluetooth server task
//     xSemaphoreGive(app.rtos.start_ble_server);

//     //* 3. Create a task that awaits an instruction. If it never get's it, the sleep timer will
//     //*    expire & the device will go back to sleep

//     TerminalMessage debug_message;
//     String incoming_instruction;

//     while (1)
//     {
//         //* 1. Await initial pairing request
//         if (xQueueReceive(app.rtos.ble_rx_message_queue, (void *)&incoming_instruction, portMAX_DELAY) == pdTRUE)
//         {
//             String temp = incoming_instruction;

//             if (temp == "PAIRING")
//             {
//                 debug_message = TerminalMessage("Pairing started",
//                                                 "PAI", INFO, micros());

//                 bool initial_pairing_time = millis();

//                 device_settings.device_is_setup = true;

//                 ESP_ERROR save_settings_on_setup = saveSettings();

//                 TerminalMessage setup_debug_message;

//                 if (save_settings_on_setup.on_error)
//                 {
//                     setup_debug_message = TerminalMessage(save_settings_on_setup.debug_message,
//                                                           "PAI", ERROR, micros(), micros() - initial_pairing_time);
//                 }
//                 else
//                 {
//                     setup_debug_message = TerminalMessage("Device was setup correctly",
//                                                           "PAI", INFO, micros(), micros() - initial_pairing_time);
//                 }

//                 addDebugMessageToQueue(&setup_debug_message);

//                 //* Start main
//                 xSemaphoreGive(app.rtos.start_main); // * Start Main App if paired
//                 vTaskDelete(NULL);
//             }
//             else
//             {
//                 debug_message = TerminalMessage("Not pairing commmand",
//                                                 "PAI", WARNING, micros());
//             }

//             addDebugMessageToQueue(&debug_message);
//         }
//     }

//     vTaskDelete(NULL);
// }
