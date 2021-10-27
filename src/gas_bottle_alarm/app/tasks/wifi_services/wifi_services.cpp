#include "wifi_services.h"
#include <WiFi.h>

#include <Firebase_ESP_Client.h>
#include <addons/TokenHelper.h>

#include "../terminal/terminal.h"

// #define WIFI_SSID "FBISurveillanceVan"
// #define WIFI_PASSWORD "500Bucks"

#define WIFI_SSID "ANZE Suspension"
#define WIFI_PASSWORD "ANZE109511"

#define WIFI_CONNECTION_ATTEMPT_TIMEOUT_S 10

// https://github.com/mobizt/Firebase-ESP-Client
#define API_KEY "AIzaSyBUlTCiCJ08R57OydcRIX01La4_Man89Cc"
#define FIREBASE_PROJECT_ID "anze---bottle-bird"
#define USER_EMAIL "mateo.segura413@gmail.com"
#define USER_PASSWORD "M@teosegace2021"

//*********************************************** DATA BASE OBJECTS

FirebaseData fbdo;
FirebaseAuth authentication;
FirebaseConfig database_config;

//*********************************************** Function Definitions (In this file only)
ESP_ERROR testWIFIconnection(); // WiFi
ESP_ERROR connectToWiFi();      // WiFi

ESP_ERROR writeDatabaseDocument(String document_path, FirebaseJson document); // Firestore
ESP_ERROR readDatabaseDocument(String document_path);                         // Firestore
ESP_ERROR deleteDatabaseDocument(String document_path);                       // Firestore

//*********************************************** Main
ESP_ERROR updateFirebase()
{
    long initial_time;
    ESP_ERROR err;

    //* 1. Connect to WiFi
    ESP_ERROR connect_wifi = connectToWiFi();

    if (connect_wifi.on_error)
    {
        err.on_error = true;
        err.debug_message = connect_wifi.debug_message;
        return err;
    }

    //* 2. Database setup. All fields required
    database_config.api_key = API_KEY;                           // 2.1 Assign the API Key
    authentication.user.email = USER_EMAIL;                      // 2.2 Assign User Email
    authentication.user.password = USER_PASSWORD;                // 2.3 Assign User Password
    database_config.token_status_callback = tokenStatusCallback; // see addons/TokenHelper.h. I do not know what this is.
    //*TODO: Get database & authentication information from SPIFFS settings file

    //* 3. Begin database
    Firebase.begin(&database_config, &authentication);
    Firebase.reconnectWiFi(false);

    //* 4. Create document path
    String document_path = "users/" + String(USER_EMAIL);

    //* 5. Delete document from database
    initial_time = micros();
    ESP_ERROR delete_database_file = deleteDatabaseDocument(document_path);

    if (delete_database_file.on_error)
    {
        err.on_error = true;
        err.debug_message = delete_database_file.debug_message;
        return err;
    }

    terminal.printMessage(TerminalMessage("Document delete succesful", "FIR", INFO, micros(), micros() - initial_time));

    //* 6. Create document
    FirebaseJson data_file;
    data_file.set("fields/unit_mac_address/stringValue", device_settings.unit_mac_address);
    data_file.set("fields/bottle_is_open/booleanValue", device_settings.bottle_is_open);
    data_file.set("fields/number_of_times_opened/integerValue", device_settings.number_of_times_opened);
    data_file.set("fields/opened_bottle_time/stringValue", device_settings.opened_bottle_time.toString());
    data_file.set("fields/closed_bottle_time/stringValue", device_settings.closed_bottle_time.toString());

    //* 7. Write document to database
    initial_time = micros();
    ESP_ERROR update_database = writeDatabaseDocument(document_path, data_file);

    if (update_database.on_error)
    {
        err.on_error = true;
        err.debug_message = update_database.debug_message;
        return err;
    }

    terminal.printMessage(TerminalMessage("Document write succesful", "FIR", INFO, micros(), micros() - initial_time));

    //* TODO: Check for OTA

    return err;
}

//*********************************************** Function definition
ESP_ERROR connectToWiFi()
{
    ESP_ERROR err;

    long initial_time = micros();

    //* 1. Begin WiFi
    WiFi.begin(WIFI_SSID, WIFI_PASSWORD);

    terminal.printMessage(TerminalMessage("Attempting WiFi connection ..., this might take some time", "WIF", INFO, micros()));

    //* 2. Attempt connection
    while (WiFi.status() != WL_CONNECTED && micros() - initial_time < WIFI_CONNECTION_ATTEMPT_TIMEOUT_S * 1000000)
    {
        vTaskDelay(30 / portTICK_PERIOD_MS);
    }

    if (WiFi.status() != WL_CONNECTED)
    {
        err.on_error = true;
        err.debug_message = "Wifi Connection Timeout";
        return err;
    }

    //* 3. Display local IP to terminal & return
    terminal.printMessage(TerminalMessage("WiFi connection successful. IP address: " + WiFi.localIP().toString(), "WIF", INFO, micros(), micros() - initial_time));

    return err;
}

ESP_ERROR writeDatabaseDocument(String path, FirebaseJson document)
{
    ESP_ERROR err;

    //* Check if database token is ready
    if (Firebase.ready())
    {
        terminal.printMessage(TerminalMessage("Attempting database document write", "FIR", INFO, micros()));

        long initial_time = micros();
        if (Firebase.Firestore.createDocument(&fbdo, FIREBASE_PROJECT_ID, "" /* databaseId can be (default) or empty */, path.c_str(), document.raw()))
        {
            terminal.printMessage(TerminalMessage("Database updated succesfully", "FIR", INFO, micros(), micros() - initial_time));
            esp.uart0.printf("\n\n%s\n\n", fbdo.payload().c_str());
        }
        else
        {
            err.on_error = true;
            err.debug_message = fbdo.errorReason();
        }
    }
    else
    {
        err.on_error = true;
        err.debug_message = "Failed to create database token";
    }

    return err;
}

ESP_ERROR readDatabaseDocument()
{
    ESP_ERROR err;

    return err;
}

ESP_ERROR deleteDatabaseDocument(String document_path)
{
    ESP_ERROR err;

    //* Check if database token is ready
    if (Firebase.ready())
    {
        //* 3. Attempt to send file
        terminal.printMessage(TerminalMessage("Attempting database document delete", "FIR", INFO, micros()));

        if (!Firebase.Firestore.deleteDocument(&fbdo, FIREBASE_PROJECT_ID, "" /* databaseId can be (default) or empty */, document_path.c_str()))
        {
            err.on_error = true;
            err.debug_message = fbdo.errorReason();
        }
    }
    else
    {
        err.on_error = true;
        err.debug_message = "Failed to create token: " + document_path;
    }

    return err;
}