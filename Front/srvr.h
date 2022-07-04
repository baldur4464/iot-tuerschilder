#include <ArduinoJson.h>

/**
 ******************************************************************************
 * @file    srvr.h
 * @author  Waveshare Team
 * @version V1.0.0
 * @date    23-January-2018
 * @brief   ESP8266 WiFi server.
 *          This file provides firmware functions:
 *           + Sending web page of the tool to a client's browser
 *           + Uploading images from client part by part
 *
 ******************************************************************************
 */

/* Includes ------------------------------------------------------------------*/
//#include <ESP8266WiFi.h>// ESP8266 and WiFi classes
#include <WiFi.h>
#include "WifiCon.h"

#include "buff.h" // POST request data accumulator
#include "DEV_Config.h"
#include "EPD.h"
#include "GUI_Paint.h"

#include "scripts.h" // JavaScript code
#include "css.h"     // Cascading Style Sheets
#include "html.h"    // HTML page of the tool

/* SSID and password of your WiFi net ----------------------------------------*/
//const char *ssid = "FRITZ!Box 7530 XF"; //"your ssid";
//const char *password = "63650802977324380###";   //"your password";

UBYTE *BlackImage;

/* Static IP address Settings ------------------------------------------------*/
IPAddress staticIP(192, 168, 178, 113);
IPAddress gateway(192, 168, 178, 1);
IPAddress subnet(255, 255, 255, 0);
IPAddress dns(223, 5, 5, 5);

/* Server and IP address ------------------------------------------------------*/
WiFiServer server(80); // Wifi server exemplar using port 80
IPAddress myIP;        // IP address in your local wifi net

/* The 'index' page flag ------------------------------------------------------*/
bool isIndexPage = true; // true : GET  request, client needs 'index' page;
// false: POST request, server sends empty page.


 
  
/* Server initialization -------------------------------------------------------*/
void Srvr__setup()
{
    Serial.print("\r\nGo!\r\n");
    printf("EPD_7IN5_test Demo\r\n");
    DEV_Module_Init();

    printf("e-Paper Init and Clear...\r\n");
    EPD_7IN5_V2_Init();
    EPD_7IN5_V2_Clear();
    DEV_Delay_ms(500);

    Serial.println();
    Serial.println();
    Serial.print("Connecting to ");
    Serial.println(ssid);

    if (WiFi.config(staticIP, gateway, subnet, dns, dns) == false)
    {
        Serial.println("Configuration failed.");
    }

    // Applying SSID and password
    WiFi.begin(ssid, password);

    // Waiting the connection to a router
    while (WiFi.status() != WL_CONNECTED)
    {
        delay(500);
        Serial.print(".");
    }

    // Connection is complete
    Serial.println("");

    Serial.println("WiFi connected");    
    

    // Start the server
    server.begin();
    Serial.println("Server started");

    // Show obtained IP address in local Wifi net
    Serial.println(myIP = WiFi.localIP());

    UWORD Imagesize = ((EPD_7IN5_V2_WIDTH % 8 == 0) ? (EPD_7IN5_V2_WIDTH / 8) : (EPD_7IN5_V2_WIDTH / 8 + 1)) * EPD_7IN5_V2_HEIGHT;
    if ((BlackImage = (UBYTE *)malloc(Imagesize)) == NULL)
    {
        printf("Failed to apply for black memory...\r\n");
        while (1)
            ;
    }
    Paint_NewImage(BlackImage, EPD_7IN5_V2_WIDTH, EPD_7IN5_V2_HEIGHT, 0, WHITE);
}

/* Sending a script to the client's browser ------------------------------------*/
bool Srvr__file(WiFiClient client, int fileIndex, char *fileName)
{
    // Print log message: sending of script file
    Serial.print(fileName);

    // Sent to the 'client' the header describing the type of data.
    client.print(fileIndex == 0
                     ? "HTTP/1.1 200 OK\r\nContent-Type: text/css\r\n\r\n"
                     : "HTTP/1.1 200 OK\r\nContent-Type: text/javascript\r\n\r\n");

    // Choose the index of script
    // (ESP8266 can't to send all of code by one file
    // and needs split it on a few parts)
    switch (fileIndex)
    {
    case 0:
        sendCSS(client);
        break;
    case 1:
        sendJS_A(client);
        break;
    case 2:
        sendJS_B(client);
        break;
    case 3:
        sendJS_C(client);
        break;
    case 4:
        sendJS_D(client);
        break;
    case 5:
        client.print("Die Ausgabe auf dem Display erfolgreich abgeschlossen! Schau auf das Schild.");
        break;
    }

    client.print("\r\n");
    delay(1);

    // Print log message: the end of request processing
    Serial.println(">>>");

    return true;
}

/* The server state observation loop -------------------------------------------*/
bool Srvr__loop()
{
    // Looking for a client trying to connect to the server
    WiFiClient client = server.available();

    // Exit if there is no any clients
    if (!client)
        return false;

    // Print log message: the start of request processing
    Serial.print("<<<");

    // Waiting the client is ready to send data
    while (!client.available())
        delay(1);

    // Set buffer's index to zero
    // It means the buffer is empty initially
    Buff__bufInd = 0;

    // While the stream of 'client' has some data do...
    while (client.available())
    {
        // Read a character from 'client'
        int q = client.read();

        // Save it in the buffer and increment its index
        Buff__bufArr[Buff__bufInd++] = (byte)q;

        // If the carachter means the end of line, then...
        if ((q == 10) || (q == 13))
        {
            // Clean the buffer
            Buff__bufInd = 0;
            continue;
        }

        // Requests of files
        if (Buff__bufInd >= 11)
        {

            if (Buff__signature(Buff__bufInd - 11, "/run.go"))
            {

                printf("SelectImage:BlackImage\r\n");
                Paint_SelectImage(BlackImage);
                Paint_Clear(WHITE);
                char json[]= "{\"meeting1\":{ \"titel\":\"Meeting 1 \", \"uhrzeit\":\"14:00-15:00\"}, \"meeting2\":{ \"titel\":\"Meeting 2 \", \"uhrzeit\":\"15:00-16:00\"}, \"systeminfo\":{ \"datum\":\"04.07.2022 \", \"updateUhrzeit\":\"15:42\"}, \"meeting3\":{ \"titel\":\"Meeting 3 \", \"uhrzeit\":\"17:00-18:00\"}, \"meeting4\":{ \"titel\":\"Meeting 4 \", \"uhrzeit\":\"18:00-19:00\"}, \"aktuellesMeeting\":{ \"titel\":\"Abschlussvortrag \", \"uhrzeit\":\"13:00-14:00\"}}";
    
                DynamicJsonDocument doc(2048);
                deserializeJson(doc, json);  
                              

                // 2.Drawing on the image
                printf("Drawing:BlackImage\r\n");
                Paint_DrawRectangle(0, 0, 800, 200, BLACK, DOT_PIXEL_1X1, DRAW_FILL_FULL);
                Paint_DrawRectangle(20, 50, 550, 180, WHITE, DOT_PIXEL_1X1, DRAW_FILL_FULL);
                Paint_DrawCircle(685, 115, 50, WHITE, DOT_PIXEL_3X3, DRAW_FILL_EMPTY);
                Paint_DrawString_EN(30, 20, doc["systeminfo"]["datum"], &Font20, BLACK, WHITE);
                Paint_DrawString_EN(380, 20, "letzte Aktualisierung:", &Font20, BLACK, WHITE);                
                Paint_DrawString_EN(700, 20, doc["systeminfo"]["updateUhrzeit"], &Font20, BLACK, WHITE);
                Paint_DrawString_EN(30, 70, doc["aktuellesMeeting"]["titel"], &Font24, WHITE, BLACK);
                Paint_DrawString_EN(30, 110, "Meeting: Abschlussvortrag IoT Tuerschild", &Font16, WHITE, BLACK);
                Paint_DrawString_EN(30, 130, "Ansprechpartner: Stefan Slooten", &Font16, WHITE, BLACK);
                Paint_DrawString_EN(30, 150, "Uhrzeit:", &Font16, WHITE, BLACK);
                Paint_DrawString_EN(130, 150, doc["aktuellesMeeting"]["uhrzeit"], &Font16, WHITE, BLACK);
                Paint_DrawString_EN(30, 210, "Folgende Meetings:", &Font24, WHITE, BLACK);
                Paint_DrawString_EN(30, 260, doc["meeting1"]["uhrzeit"], &Font20, WHITE, BLACK);
                Paint_DrawString_EN(200, 260, doc["meeting1"]["titel"], &Font20, WHITE, BLACK);
                Paint_DrawString_EN(30, 300, doc["meeting2"]["uhrzeit"], &Font20, WHITE, BLACK);
                Paint_DrawString_EN(200, 300, doc["meeting2"]["titel"], &Font20, WHITE, BLACK);
                Paint_DrawString_EN(30, 340, doc["meeting3"]["uhrzeit"], &Font20, WHITE, BLACK);
                Paint_DrawString_EN(200, 340, doc["meeting3"]["titel"], &Font20, WHITE, BLACK);
                Paint_DrawString_EN(30, 380, doc["meeting4"]["uhrzeit"], &Font20, WHITE, BLACK);
                Paint_DrawString_EN(200, 380, doc["meeting4"]["titel"], &Font20, WHITE, BLACK);              
                //Paint_DrawNum(110, 20, 2022, &Font20, WHITE, BLACK);                               
                Paint_DrawString_EN(650, 95, "Raum", &Font24, BLACK, WHITE);
                Paint_DrawNum(670, 125, 12, &Font24, WHITE, BLACK);

                printf("EPD_Display\r\n");
                EPD_7IN5_V2_Display(BlackImage);
                return Srvr__file(client, 5, "run.go");
            }
            if (Buff__signature(Buff__bufInd - 11, "/styles.css"))
                return Srvr__file(client, 0, "styles.css");

            if (Buff__signature(Buff__bufInd - 11, "/scriptA.js"))
                return Srvr__file(client, 1, "scriptA.js");

            if (Buff__signature(Buff__bufInd - 11, "/scriptB.js"))
                return Srvr__file(client, 2, "scriptB.js");

            if (Buff__signature(Buff__bufInd - 11, "/scriptC.js"))
                return Srvr__file(client, 3, "scriptC.js");

            if (Buff__signature(Buff__bufInd - 11, "/scriptD.js"))
                return Srvr__file(client, 4, "scriptD.js");
        }

        // If the buffer's length is larger, than 4 (length of command's name), then...
        if (Buff__bufInd > 4)
        {
            // It is probably POST request, no need to send the 'index' page
            isIndexPage = false;
            // e-Paper driver initialization
            if (Buff__signature(Buff__bufInd - 4, "EPD"))
            {

                break;
            }

            // Image loading
            if (Buff__signature(Buff__bufInd - 4, "LOAD"))
            {
                // Print log message: image loading
                Serial.print("LOAD");
                break;
            }

            // Select the next data channel
            if (Buff__signature(Buff__bufInd - 4, "NEXT"))
            {
                // Print log message: next data channel
                Serial.print("NEXT");
                break;
            }

            // If the loading is complete, then...
            if (Buff__signature(Buff__bufInd - 4, "SHOW"))
            {

                break;
            }

            // If the routine reaches this code,
            // it means the there is no known commands,
            // the server has to send the 'index' page
            isIndexPage = true;
        }
    }

    // Clear data stream of the 'client'
    client.flush();

    // Sent to the 'client' the header describing the type of data.
    // In this case 'Content-Type' is 'text/html'
    client.print("HTTP/1.1 200 OK\r\nContent-Type: text/html\r\n\r\n");

    // Send the 'index' page if it's needed
    if (isIndexPage)
        sendHtml(client, myIP);
    else
        client.print("Ok!");

    client.print("\r\n");
    delay(1);

    // Print log message: the end of request processing
    Serial.println(">>>");
    return true;
}
