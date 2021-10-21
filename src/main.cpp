#include <Arduino.h>
#include <gas_bottle_alarm/app/app.h>

// https://docs.espressif.com/projects/esp-idf/en/latest/esp32/api-guides/ulp_macros.html
// https://github.com/wardjm/esp32-ulp-i2c

BottleBirdApp app;

void setup()
{
    app.begin();
}

void loop() {}

// //************************************************************TEST CODE GOES HERE************************************************************//

// /**
//  * Created by K. Suwatchai (Mobizt)
//  *
//  * Email: k_suwatchai@hotmail.com
//  *
//  * Github: https://github.com/mobizt
//  *
//  * Copyright (c) 2021 mobizt
//  *
//  */

// // This example shows how to create a document in a document collection. This operation required Email/password, custom or OAUth2.0 authentication.

// #if defined(ESP32)
// #include <WiFi.h>
// #elif defined(ESP8266)
// #include <ESP8266WiFi.h>
// #endif
// #include <Firebase_ESP_Client.h>

// // Provide the token generation process info.
// #include <addons/TokenHelper.h>

// /* 1. Define the WiFi credentials */
// #define WIFI_SSID "FBISurveillanceVan"
// #define WIFI_PASSWORD "500Bucks"

// /* 2. Define the API Key */
// #define API_KEY "AIzaSyBUlTCiCJ08R57OydcRIX01La4_Man89Cc"

// /* 3. Define the project ID */
// #define FIREBASE_PROJECT_ID "anze---bottle-bird"

// /* 4. Define the user Email and password that alreadey registerd or added in your project */
// #define USER_EMAIL "mateo.segura413@gmail.com"
// #define USER_PASSWORD "M@teosegace2021"

// // Define Firebase Data object
// FirebaseData fbdo;

// FirebaseAuth auth;
// FirebaseConfig config;

// unsigned long dataMillis = 0;
// int count = 0;

// void createFile(); // Create

// void appendArray();         // Commit
// void incrementFieldValue(); // Commit
// void setUpdateDelete();     // Commit

// void setup()
// {

//     Serial.begin(115200);

//     WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
//     Serial.print("Connecting to Wi-Fi");
//     while (WiFi.status() != WL_CONNECTED)
//     {
//         Serial.print(".");
//         delay(300);
//     }
//     Serial.println();
//     Serial.print("Connected with IP: ");
//     Serial.println(WiFi.localIP());
//     Serial.println();

//     /* Assign the api key (required) */
//     config.api_key = API_KEY;

//     /* Assign the user sign in credentials */
//     auth.user.email = USER_EMAIL;
//     auth.user.password = USER_PASSWORD;

//     /* Assign the callback function for the long running token generation task */
//     config.token_status_callback = tokenStatusCallback; // see addons/TokenHelper.h

//     Firebase.begin(&config, &auth);

//     Firebase.reconnectWiFi(true);
// }

// void loop()
// {
//     //* Create a new file
//     // createFile();

//     //* Commit to file
//     // appendArray();
//     // incrementFieldValue();
//     setUpdateDelete();

//     //* Delete file
// }

// void createFile()
// {

//     if (Firebase.ready() && (millis() - dataMillis > 60000 || dataMillis == 0))
//     {
//         dataMillis = millis();

//         FirebaseJson content;

//         // We will create the nested document in the parent path "a0/b0/c0
//         // a0 is the collection id, b0 is the document id in collection a0 and c0 is the collection id in the document b0.
//         // and d? is the document id in the document collection id c0 which we will create.
//         String documentPath = "a0/b0/c0/d" + String(count);

//         // If the document path contains space e.g. "a b c/d e f"
//         // It should encode the space as %20 then the path will be "a%20b%20c/d%20e%20f"

//         // double
//         content.set("fields/myDouble/doubleValue", 123.45678);

//         // boolean
//         content.set("fields/myBool/booleanValue", true);

//         // integer
//         content.set("fields/myInteger/integerValue", "911");

//         // null
//         content.set("fields/myNull/nullValue"); // no value set

//         String doc_path = "projects/";
//         doc_path += FIREBASE_PROJECT_ID;
//         doc_path += "/databases/(default)/documents/coll_id/doc_id"; // coll_id and doc_id are your collection id and document id

//         // reference
//         content.set("fields/myRef/referenceValue", doc_path.c_str());

//         // timestamp
//         content.set("fields/myTimestamp/timestampValue", "2014-10-02T15:01:23Z"); // RFC3339 UTC "Zulu" format

//         // bytes
//         content.set("fields/myBytes/bytesValue", "aGVsbG8="); // base64 encoded

//         // array
//         content.set("fields/myArray/arrayValue/values/[0]/stringValue", "test");
//         content.set("fields/myArray/arrayValue/values/[1]/integerValue", "20");
//         content.set("fields/myArray/arrayValue/values/[2]/booleanValue", true);

//         // map
//         content.set("fields/myMap/mapValue/fields/name/stringValue", "wrench");
//         content.set("fields/myMap/mapValue/fields/mass/stringValue", "1.3kg");
//         content.set("fields/myMap/mapValue/fields/count/integerValue", "3");

//         // lat long
//         content.set("fields/myLatLng/geoPointValue/latitude", 1.486284);
//         content.set("fields/myLatLng/geoPointValue/longitude", 23.678198);

//         count++;

//         Serial.print("Create a document... ");

//         if (Firebase.Firestore.createDocument(&fbdo, FIREBASE_PROJECT_ID, "" /* databaseId can be (default) or empty */, documentPath.c_str(), content.raw()))
//             Serial.printf("ok\n%s\n\n", fbdo.payload().c_str());
//         else
//             Serial.println(fbdo.errorReason());
//     }
// }

// void appendArray()
// {
//     if (Firebase.ready() && (millis() - dataMillis > 60000 || dataMillis == 0))
//     {
//         dataMillis = millis();
//         count++;

//         Serial.print("Commit a document (append array)... ");

//         // The dyamic array of write object fb_esp_firestore_document_write_t.
//         std::vector<struct fb_esp_firestore_document_write_t> writes;

//         // A write object that will be written to the document.
//         struct fb_esp_firestore_document_write_t transform_write;

//         // Set the write object write operation type.
//         // fb_esp_firestore_document_write_type_update,
//         // fb_esp_firestore_document_write_type_delete,
//         // fb_esp_firestore_document_write_type_transform
//         transform_write.type = fb_esp_firestore_document_write_type_transform;

//         // Set the document path of document to write (transform)
//         transform_write.document_transform.transform_document_path = "test_collection/test_document";

//         // Set a transformation of a field of the document.
//         struct fb_esp_firestore_document_write_field_transforms_t field_transforms;

//         // Set field path to write.
//         field_transforms.fieldPath = "appended_data";

//         // Set the transformation type.
//         // fb_esp_firestore_transform_type_set_to_server_value,
//         // fb_esp_firestore_transform_type_increment,
//         // fb_esp_firestore_transform_type_maaximum,
//         // fb_esp_firestore_transform_type_minimum,
//         // fb_esp_firestore_transform_type_append_missing_elements,
//         // fb_esp_firestore_transform_type_remove_all_from_array
//         field_transforms.transform_type = fb_esp_firestore_transform_type_append_missing_elements;

//         FirebaseJson content;

//         String txt = "Hello World! " + String(count);
//         content.set("values/[0]/integerValue", String(rand()).c_str());
//         content.set("values/[1]/stringValue", txt);

//         // Set the transformation content.
//         field_transforms.transform_content = content.raw();

//         // Add a field transformation object to a write object.
//         transform_write.document_transform.field_transforms.push_back(field_transforms);

//         // Add a write object to a write array.
//         writes.push_back(transform_write);

//         if (Firebase.Firestore.commitDocument(&fbdo, FIREBASE_PROJECT_ID, "" /* databaseId can be (default) or empty */, writes /* dynamic array of fb_esp_firestore_document_write_t */, "" /* transaction */))
//             Serial.printf("ok\n%s\n\n", fbdo.payload().c_str());
//         else
//             Serial.println(fbdo.errorReason());
//     }
// }

// void incrementFieldValue()
// {
//     if (Firebase.ready() && (millis() - dataMillis > 20 || dataMillis == 0))
//     {
//         dataMillis = millis();

//         Serial.print("Commit a document (field value increment)... ");

//         // The dyamic array of write object fb_esp_firestore_document_write_t.
//         std::vector<struct fb_esp_firestore_document_write_t> writes;

//         // A write object that will be written to the document.
//         struct fb_esp_firestore_document_write_t transform_write;

//         // Set the write object write operation type.
//         // fb_esp_firestore_document_write_type_update,
//         // fb_esp_firestore_document_write_type_delete,
//         // fb_esp_firestore_document_write_type_transform
//         transform_write.type = fb_esp_firestore_document_write_type_transform;

//         // Set the document path of document to write (transform)
//         transform_write.document_transform.transform_document_path = "test_collection/test_document";

//         // Set a transformation of a field of the document.
//         struct fb_esp_firestore_document_write_field_transforms_t field_transforms;

//         // Set field path to write.
//         field_transforms.fieldPath = "counter";

//         // Set the transformation type.
//         // fb_esp_firestore_transform_type_set_to_server_value,
//         // fb_esp_firestore_transform_type_increment,
//         // fb_esp_firestore_transform_type_maaximum,
//         // fb_esp_firestore_transform_type_minimum,
//         // fb_esp_firestore_transform_type_append_missing_elements,
//         // fb_esp_firestore_transform_type_remove_all_from_array
//         field_transforms.transform_type = fb_esp_firestore_transform_type_increment;

//         FirebaseJson values;

//         values.set("integerValue", "1"); // increase by 1

//         // Set the values of field
//         field_transforms.transform_content = values.raw();

//         // Add a field transformation object to a write object.
//         transform_write.document_transform.field_transforms.push_back(field_transforms);

//         // Add a write object to a write array.
//         writes.push_back(transform_write);

//         if (Firebase.Firestore.commitDocumentAsync(&fbdo, FIREBASE_PROJECT_ID, "" /* databaseId can be (default) or empty */, writes /* dynamic array of fb_esp_firestore_document_write_t */, "" /* transaction */))
//             Serial.println("ok");
//         else
//             Serial.println(fbdo.errorReason());
//     }
// }

// void setUpdateDelete()
// {
//     if (Firebase.ready() && (millis() - dataMillis > 60000 || dataMillis == 0))
//     {
//         dataMillis = millis();
//         count++;

//         Serial.print("Commit a document (set server value, update document)... ");

//         // The dyamic array of write object fb_esp_firestore_document_write_t.
//         std::vector<struct fb_esp_firestore_document_write_t> writes;

//         // A write object that will be written to the document.
//         struct fb_esp_firestore_document_write_t transform_write;

//         // Set the write object write operation type.
//         // fb_esp_firestore_document_write_type_update,
//         // fb_esp_firestore_document_write_type_delete,
//         // fb_esp_firestore_document_write_type_transform
//         transform_write.type = fb_esp_firestore_document_write_type_transform;

//         // Set the document path of document to write (transform)
//         transform_write.document_transform.transform_document_path = "test_collection/test_document";

//         // Set a transformation of a field of the document.
//         struct fb_esp_firestore_document_write_field_transforms_t field_transforms;

//         // Set field path to write.
//         field_transforms.fieldPath = "server_time";

//         // Set the transformation type.
//         // fb_esp_firestore_transform_type_set_to_server_value,
//         // fb_esp_firestore_transform_type_increment,
//         // fb_esp_firestore_transform_type_maaximum,
//         // fb_esp_firestore_transform_type_minimum,
//         // fb_esp_firestore_transform_type_append_missing_elements,
//         // fb_esp_firestore_transform_type_remove_all_from_array
//         field_transforms.transform_type = fb_esp_firestore_transform_type_set_to_server_value;

//         // Set the transformation content, server value for this case.
//         // See https://firebase.google.com/docs/firestore/reference/rest/v1/Write#servervalue
//         field_transforms.transform_content = "REQUEST_TIME"; // set timestamp to "test_collection/test_document/server_time"

//         // Add a field transformation object to a write object.
//         transform_write.document_transform.field_transforms.push_back(field_transforms);

//         // Add a write object to a write array.
//         writes.push_back(transform_write);

//         //////////////////////////////
//         // Add another write for update

//         /*
//         A write object that will be written to the document.
//         struct fb_esp_firestore_document_write_t update_write;
//         Set the write object write operation type.
//         fb_esp_firestore_document_write_type_update,
//         fb_esp_firestore_document_write_type_delete,
//         fb_esp_firestore_document_write_type_transform
//         update_write.type = fb_esp_firestore_document_write_type_update;
//         Set the document content to write (transform)
//         FirebaseJson content;
//         String documentPath = "test_collection/d" + String(count);
//         content.set("fields/count/integerValue", String(count).c_str());
//         content.set("fields/random/integerValue", String(rand()).c_str());
//         content.set("fields/status/booleanValue", count % 2 == 0);

//         Set the update document content
//         update_write.update_document_content = content.raw();
//         Set the update document path
//         update_write.update_document_path = documentPath.c_str();
//         Set the document mask field paths that will be updated
//         Use comma to separate between the field paths
//         update_write.update_masks = "count,random";
//         Set the precondition write on the document.
//         The write will fail if this is set and not met by the target document.
//         Th properties for update_write.current_document should set only one from exists or update_time
//         update_write.current_document.exists = "true";
//         update_write.current_document.update_time = "2021-05-02T15:01:23Z";
//         Add a write object to a write array.
//         writes.push_back(update_write);
//         */

//         //////////////////////////////
//         // Add another write for delete

//         /*
//         //A write object that will be written to the document.
//         struct fb_esp_firestore_document_write_t delete_write;
//         //Set the write object write operation type.
//         //fb_esp_firestore_document_write_type_update,
//         //fb_esp_firestore_document_write_type_delete,
//         //fb_esp_firestore_document_write_type_transform
//         delete_write.type = fb_esp_firestore_document_write_type_delete;
//         String documentPath = "test_collection/d" + String(count);
//         //Set the update document content
//         delete_write.delete_document_path = documentPath.c_str();
//         //don't apply any document mask for delete operation write.
//         //Add a write object to a write array.
//         writes.push_back(delete_write);
//         */

//         if (Firebase.Firestore.commitDocument(&fbdo, FIREBASE_PROJECT_ID, "" /* databaseId can be (default) or empty */, writes /* dynamic array of fb_esp_firestore_document_write_t */, "" /* transaction */))
//             Serial.printf("ok\n%s\n\n", fbdo.payload().c_str());
//         else
//             Serial.println(fbdo.errorReason());
//     }
// }

// void deleteFile()
// {
//     bool taskCompleted = false;

//     if (Firebase.ready() && !taskCompleted)
//     {
//         taskCompleted = true;

//         FirebaseJson content;

//         // aa is the collection id, bb is the document id in collection aa.
//         String documentPath = "aa/bb";

//         // If the document path contains space e.g. "a b c/d e f"
//         // It should encode the space as %20 then the path will be "a%20b%20c/d%20e%20f"

//         content.set("fields/v1/integerValue", "12345");
//         content.set("fields/v2/doubleValue", 123.456);

//         Serial.print("Create a document... ");

//         if (Firebase.Firestore.createDocument(&fbdo, FIREBASE_PROJECT_ID, "" /* databaseId can be (default) or empty */, documentPath.c_str(), content.raw()))
//             Serial.printf("ok\n%s\n\n", fbdo.payload().c_str());
//         else
//             Serial.println(fbdo.errorReason());

//         Serial.print("Delete a document... ");

//         if (Firebase.Firestore.deleteDocument(&fbdo, FIREBASE_PROJECT_ID, "" /* databaseId can be (default) or empty */, documentPath.c_str()))
//             Serial.printf("ok\n%s\n\n", fbdo.payload().c_str());
//         else
//             Serial.println(fbdo.errorReason());
//     }
// }