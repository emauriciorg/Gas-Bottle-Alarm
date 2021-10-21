#include "wireless_control.h"
#include "../settings/settings.h"

void wirelessControlMode()
{
    //* 5.0 Set CPU to full clock speed
    setCpuFrequencyMhz(240);

    //* 5.1 Interrupt wake up means user pressed the button
    terminal.printMessage(TerminalMessage("Wakeup caused by user button",
                                          "SLE", INFO, micros()));

    //* 5.2 Read settings from SPIFFS memory
    long initial_read_time = micros();

    ESP_ERROR get_settings_from_memory = getSettingsFromMemory();

    if (get_settings_from_memory.on_error)
    {
        terminal.printMessage(TerminalMessage(get_settings_from_memory.debug_message,
                                              "SLE", ERROR, micros()));

        abort();
    }

    terminal.printMessage(TerminalMessage("Settings file read from memory",
                                          "SLE", INFO, micros(), micros() - initial_read_time));

    //* 5.3 Start pairing or start main app
    if (device_settings.device_is_setup)
    {
        terminal.printMessage(TerminalMessage("Start pairing",
                                              "SLE", INFO, micros()));
    }
    else
    {
        terminal.printMessage(TerminalMessage("Starting main app",
                                              "SLE", INFO, micros()));
    }
}