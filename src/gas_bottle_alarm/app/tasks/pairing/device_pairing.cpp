#include "device_pairing.h"

#include "../terminal/terminal.h"
#include "../bluetooth_server/ble_server.h"
#include "../settings/settings.h"
#include "../led_strip/led_strip.h"
#include "../time_keeper/time_keeper.h"
#include "../deep_sleep/deep_sleep.h"

#include "api.h"

#define AWAIT_CMD_TIMEOUT_S 30

void pairingTask();

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

    //* 3. Create tasks needed by this process if RTOS was setup correctly
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

    pairingTask();

    vTaskDelete(NULL);
}

void pairingTask()
{
    //* 1. Start the bluetooth server task
    xSemaphoreGive(app.rtos.start_ble_server);

    String incoming_command = "";
    long initial_time;

    TerminalMessage pairing_debug_message;

    long last_command_time = millis();
    bool settings_changed = false;

    while (millis() - last_command_time < AWAIT_CMD_TIMEOUT_S * 1000)
    {
        vTaskDelay(10);
        //* 1. Await bluetooth terminal
        if (xQueueReceive(app.rtos.ble_rx_message_queue, (void *)&incoming_command, 0) == pdTRUE)
        {
            TerminalMessage debug_message;
            String instruction = incoming_command;

            //* Check If command is pairing
            if (instruction == API.PAIRING_RQT_CMD)
            {
                initial_time = micros();

                pairing_debug_message = TerminalMessage("Pairing started", "PAI", INFO, micros());
                addDebugMessageToQueue(&pairing_debug_message);

                //* 1.1 Respond to mobile phone after 200ms.
                vTaskDelay(200 / portTICK_PERIOD_MS);
                addBluetoothTXMessageToQueue(&API.PAIRING_ACK_CMD);

                //* 1.2 Set LEDs to green & sounds buzzer for connected

                //* 1.3 Await for any incoming commands. Timeout is handled by BLE server
                last_command_time = millis(); // Reset timeout

                while (millis() - last_command_time < AWAIT_CMD_TIMEOUT_S * 1000)
                {
                    vTaskDelay(10);

                    if (xQueueReceive(app.rtos.ble_rx_message_queue, (void *)&incoming_command, 0) == pdTRUE)
                    {
                        //* 1.3.1 Decode Instruction
                        bool decode = API.decodeCommand(incoming_command);

                        if (decode)
                        {
                            settings_changed = true;
                            pairing_debug_message = TerminalMessage("Command decoded: " + incoming_command, "PAI", INFO, micros());
                            addDebugMessageToQueue(&pairing_debug_message);
                        }
                        else
                        {
                            pairing_debug_message = TerminalMessage("Data is not an API command", "PAI", INFO, micros());
                            addDebugMessageToQueue(&pairing_debug_message);

                            addBluetoothTXMessageToQueue(&API.NOT_VALID_CMD);
                        }
                    }
                }
            }
            else
            {
                debug_message = TerminalMessage("Not pairing commmand",
                                                "PAI", WARNING, micros());
                addDebugMessageToQueue(&debug_message);
            }
        }
    }

    //* If no command sent before timeout
    TerminalMessage timeout_message = TerminalMessage("No command timeout. Checking for changes...",
                                                      "PAI", WARNING, micros());

    addDebugMessageToQueue(&timeout_message);

    //* Save settings in timeout or disconnect
    if (settings_changed)
    {
        TerminalMessage start_save_message = TerminalMessage("Changes detected. Saving settings file to memory",
                                                             "PAI", WARNING, micros());

        addDebugMessageToQueue(&start_save_message);

        //* 1.3.2 Check if memory is initialized
        initial_time = micros();
        if (!spiffsMemory.isInitialized())
        {
            ESP_ERROR initialize_spiffs = spiffsMemory.begin();

            if (initialize_spiffs.on_error)
            {
                pairing_debug_message = TerminalMessage(initialize_spiffs.debug_message,
                                                        "PAI", ERROR, micros(), micros() - initial_time);

                addBluetoothTXMessageToQueue(&API.SPIFFS_ERR);
            }
        }

        //* 1.3.3 Save settings file to SPIFFS
        initial_time = micros();
        ESP_ERROR save_settings = saveSettings();

        if (save_settings.on_error)
        {
            pairing_debug_message = TerminalMessage(save_settings.debug_message,
                                                    "PAI", ERROR, micros(), micros() - initial_time);

            addBluetoothTXMessageToQueue(&API.SPIFFS_ERR);
        }

        else
        {
            pairing_debug_message = TerminalMessage("Settings saved correctly",
                                                    "PAI", INFO, micros(), micros() - initial_time);
        }
    }
    else
    {
        pairing_debug_message = TerminalMessage("No settings were changed",
                                                "PAI", INFO, micros());
    }

    addDebugMessageToQueue(&pairing_debug_message);

    vTaskDelay(10 / portTICK_PERIOD_MS); // Await terminal to flush

    terminal.end();

    startDeepSleep();

    vTaskDelete(NULL);
}
