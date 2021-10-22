#pragma once

#include <Arduino.h>
#include "../../app.h"

class BottleBirdAPI
{
public:
    uint16_t number_of_commands_in_API = 16;

    //***** From Device
    String PAIRING_RQT_CMD = "PAIR_RQT";

    //***** To Device
    String PAIRING_ACK_CMD = "PAIR_ACK";
    String NOT_VALID_CMD = "NOT_VALID_CMD";

    // Errors
    String SPIFFS_ERR = "ERR_SPIFFS";

    bool decodeCommand(String command)
    {
        //* TODO: All the following need to be true for the device to activate
        String command_type = command.substring(0, 3);

        if (command_type.equals("NET"))
        {
            String network_command_type = command.substring(4, 7);

            if (network_command_type.equals("NAME"))
            {
            }

            if (network_command_type.equals("PASS"))
            {
            }
        }
        return true;

        return false;
    }

} API;