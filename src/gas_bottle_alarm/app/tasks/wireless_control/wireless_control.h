#pragma once

#include "gas_bottle_alarm/app/app.h"

//*
//* Wireless Control Mode
//*
//* Description: During this mode the unit will begin the bluetooth server, and wait for a connection
//*              from the user. If no connection is stablished before the timeout, the device will
//*              start deep sleep mode again.
//*
//*              If a bluetooth connection is stablished, the device will send & receive commands
//*              according to the API of the device.
//*
//*              If no commands have been received after a time out period, the unit will start deep
//*              sleep mode again, and the bluetooth connection will be terminated.
//*

void wirelessControlMode();