#include "ble_server.h"
#include "../terminal/terminal.h"
#include "../deep_sleep/deep_sleep.h"
#include "../led_strip/led_strip.h"

#define BLE_ADVERTISING_TIMEOUT_S 30

// BLE Service Characteristics
#define BLE_NAME "Bottle bird"
#define SERVICE_UUID "6E400001-B5A3-F393-E0A9-E50E24DCCA9E" // UART service UUID
#define CHARACTERISTIC_UUID "6E400002-B5A3-F393-E0A9-E50E24DCCA9E"

void addBluetoothRXMessageToQueue(String *bluetooth_message, uint16_t port_ticks);
void bleAdvertisingTimeout(void *parameters);
void bleTransmitTask(void *parameters);

// * Server Call Backs
class MyServerCallbacks : public BLEServerCallbacks
{
    // -- Local Variables
    TerminalMessage ble_server_debug_message;
    static const uint8_t serial_message_port_ticks = 0;

    // -- On client connect
    void onConnect(BLEServer *pServer)
    {
        //* 1. Delete timeout task
        vTaskDelete(app.rtos.ble_advertising_timeout_handle);

        bleServer.onClientConnect();

        ble_server_debug_message = TerminalMessage("Client connected", // TerminalMessage body
                                                   "BLE",              // System keyword
                                                   INFO,               // Terminal message type
                                                   micros());          // Timestamp

        addDebugMessageToQueue(&ble_server_debug_message);

        setColor("green");

        // TODO: Handle TX functionality here
        //  xTaskCreate(bleTransmitTask,
        //              "Bluetooth TX task",
        //              10000,
        //              NULL,
        //              2,
        //              NULL);
    };

    // -- On client disconnect
    void onDisconnect(BLEServer *pServer)
    {
        bleServer.onClientDisconnect();

        ble_server_debug_message = TerminalMessage("Client disconnected", // TerminalMessage body
                                                   "BLE",                 // System keyword
                                                   INFO,                  // Terminal message type
                                                   micros());             // Timestamp

        addDebugMessageToQueue(&ble_server_debug_message);

        // TODO: Add OTA functionality here
        bleServer.startAdvertising();

        xTaskCreatePinnedToCore(bleAdvertisingTimeout,
                                "BLE timeout",
                                3000,
                                NULL,
                                25,
                                &app.rtos.ble_advertising_timeout_handle,
                                0);
    }
} ServerCallbacks;

// * Characteristic Call Backs
class MyCallbacks : public BLECharacteristicCallbacks
{
    TerminalMessage ble_rx_message;
    String instruction = "";

    void onWrite(BLECharacteristic *pCharacteristic)
    {
        std::string rxValue = pCharacteristic->getValue();

        String incoming_bluetooth_message = "Incoming data (size: ";
        incoming_bluetooth_message += rxValue.length();
        incoming_bluetooth_message += ") ->\t";

        instruction = "";

        // Store incoming message in string
        if (rxValue.length() > 0)
        {
            for (int i = 0; i < rxValue.length(); i++)
            {
                instruction += rxValue[i];
                incoming_bluetooth_message += rxValue[i];
            }

            ble_rx_message = TerminalMessage(incoming_bluetooth_message, "BLE", INFO, micros());
            addDebugMessageToQueue(&ble_rx_message);

            addBluetoothRXMessageToQueue(&instruction, 0);
        }
    }

} CharacteristicCallbacks;

void bleServerTask(void *parameters)
{
    long initial_time = micros();
    TerminalMessage ble_debug_message;
    uint8_t debug_message_queue_ticks = 0;

    //* 1. Await binary semaphore
    xSemaphoreTake(app.rtos.start_ble_server, portMAX_DELAY);

    ble_debug_message = TerminalMessage("Starting Bluetooth Server", "BLE", INFO, micros());
    addDebugMessageToQueue(&ble_debug_message, debug_message_queue_ticks);

    //* 2. Begin Service
    // TODO: FFS settings not working
    // bleServer.begin("Bottle bird",
    //                 app.device_settings.ble_service_uuid,
    //                 app.device_settings.ble_characteristic_uuid,
    //                 &ServerCallbacks,
    //                 &CharacteristicCallbacks);

    bleServer.begin(BLE_NAME,
                    SERVICE_UUID,
                    CHARACTERISTIC_UUID,
                    &ServerCallbacks,
                    &CharacteristicCallbacks);

    // * 3. Set MTU size
    TerminalMessage mtu_size_message;
    initial_time = micros();
    ESP_ERROR mtu_set = bleServer.setMaxMTUsize(185); // -- Try to negotiate Max size MTU (iOS max. MTU is 185 bytes)

    if (mtu_set.on_error) // Catch error
    {
        mtu_size_message = TerminalMessage(mtu_set.debug_message,
                                           "BLE", WARNING, micros(), micros() - initial_time);
    }
    else
    {
        mtu_size_message = TerminalMessage("MTU size set to 185 bytes. Max throughput is ~ 11KB/s",
                                           "BLE", INFO, micros(), micros() - initial_time);
    }

    addDebugMessageToQueue(&mtu_size_message, debug_message_queue_ticks);

    // * 4. Start Advertising
    initial_time = micros();
    bleServer.startAdvertising();
    vTaskDelay(2 / portTICK_PERIOD_MS);

    TerminalMessage ble_advertising_message = TerminalMessage("Server is advertising. Waiting for client connection to notify",
                                                              "BLE", INFO, micros(), micros() - initial_time);

    addDebugMessageToQueue(&ble_advertising_message, debug_message_queue_ticks);

    // * Create server tasks
    xTaskCreatePinnedToCore(bleTransmitTask,
                            "BLE TX Task",
                            10000,
                            NULL,
                            22,
                            NULL,
                            0);

    xTaskCreatePinnedToCore(bleAdvertisingTimeout,
                            "BLE timeout",
                            3000,
                            NULL,
                            25,
                            &app.rtos.ble_advertising_timeout_handle,
                            0);

    vTaskDelete(NULL); // Delete Task
}

void addBluetoothTXMessageToQueue(String *bluetooth_message, uint16_t port_ticks)
{
    xSemaphoreTake(app.rtos.ble_tx_message_queue_mutex, port_ticks);
    xQueueSend(app.rtos.ble_tx_message_queue, (void *)bluetooth_message, 0);
    xSemaphoreGive(app.rtos.ble_tx_message_queue_mutex);
}

void addBluetoothRXMessageToQueue(String *bluetooth_message, uint16_t port_ticks = 0)
{
    xSemaphoreTake(app.rtos.ble_rx_message_queue_mutex, port_ticks);
    xQueueSend(app.rtos.ble_rx_message_queue, (void *)bluetooth_message, 0);
    xSemaphoreGive(app.rtos.ble_rx_message_queue_mutex);
}

void bleAdvertisingTimeout(void *parameters)
{
    while (1)
    {
        //* 1. Delay for certain time
        vTaskDelay((BLE_ADVERTISING_TIMEOUT_S * 1000) / portTICK_PERIOD_MS);

        //* 2. Start deep sleep if timeout
        terminal.printMessage(TerminalMessage("Bluetooth advertising timeout",
                                              "SLE", INFO, micros()));
        terminal.end();

        //esp.uart0.println("\n\n");
        //esp.uart0.print("Power off");

        startDeepSleep();
    }
}

void bleTransmitTask(void *parameters)
{
    long initial_time = micros(); // Track process time

    TerminalMessage ble_tx_task_debug_message;          // Create local task variable for debug message queue
    static const uint8_t serial_message_port_ticks = 0; // Queue tick timeout

    uint8_t ble_message_minimum_interval = 13;

    String ble_tx_message;

    ble_tx_task_debug_message = TerminalMessage("Bluetooth Low Energy TX active",                // Mesage
                                                "BLE", INFO, micros(), micros() - initial_time); // System, message type, timestamp, process time

    addDebugMessageToQueue(&ble_tx_task_debug_message, serial_message_port_ticks); // -- Add message to terminal queue

    while (1)
    {
        if (xQueueReceive(app.rtos.ble_tx_message_queue, (void *)&ble_tx_message, 0) == pdTRUE)
        {
            initial_time = micros();
            ESP_ERROR data_sent = bleServer.sendDataToClient((char *)ble_tx_message.c_str());

            if (data_sent.on_error)
            {
                ble_tx_task_debug_message = TerminalMessage(data_sent.debug_message,  // TerminalMessage body
                                                            "BLE",                    // System keyword
                                                            ERROR,                    // TerminalMessage type
                                                            micros(),                 // Timestamp
                                                            micros() - initial_time); // Process time

                addDebugMessageToQueue(&ble_tx_task_debug_message, serial_message_port_ticks); // -- Add message to terminal queue
            }
        }
        vTaskDelay(ble_message_minimum_interval / portTICK_PERIOD_MS);
    }

    vTaskDelete(NULL);
}