static const char* tag = "tuerschild_eink_esp32";

#include <ArduinoJson.h>

extern "C" {
#include "tuerschild.h"
}
#include "DEV_Config.h"
#include "EPD.h"
#include "GUI_Paint.h"

extern "C" void setup_display()
{
    DEV_Module_Init();
    EPD_7IN5_V2_Init();
    EPD_7IN5_V2_Clear();
    
}

extern "C" void process_and_show()
{
    char* json = data_in_buf;
    UWORD Imagesize = ((EPD_7IN5_V2_WIDTH % 8 == 0) ? (EPD_7IN5_V2_WIDTH / 8) : (EPD_7IN5_V2_WIDTH / 8 + 1)) * EPD_7IN5_V2_HEIGHT;
    uint8_t* BlackImage = (UBYTE *)malloc(Imagesize);


    //Paint_SelectImage(BlackImage);
    Paint_NewImage(BlackImage, EPD_7IN5_V2_WIDTH, EPD_7IN5_V2_HEIGHT, 0, WHITE);
    Paint_Clear(WHITE);
    //char json[]= "{\"meeting1\":{ \"titel\":\"Meeting 1 \", \"uhrzeit\":\"14:00-15:00\"}, \"meeting2\":{ \"titel\":\"Meeting 2 \", \"uhrzeit\":\"15:00-16:00\"}, \"systeminfo\":{ \"datum\":\"04.07.2022 \", \"updateUhrzeit\":\"15:42\"}, \"meeting3\":{ \"titel\":\"Meeting 3 \", \"uhrzeit\":\"17:00-18:00\"}, \"meeting4\":{ \"titel\":\"Meeting 4 \", \"uhrzeit\":\"18:00-19:00\"}, \"aktuellesMeeting\":{ \"titel\":\"Abschlussvortrag \", \"verantwortlicher\":\"Stefan Slooten\", \"uhrzeit\":\"13:00-14:00\"}}";
    
    DynamicJsonDocument doc(2048);
    deserializeJson(doc, json);  
                    

    // 2.Drawing on the image
    printf("Drawing:BlackImage\r\n");
    Paint_DrawRectangle(0, 0, 800, 200, BLACK, DOT_PIXEL_1X1, DRAW_FILL_FULL);
    TUERSCHILD_LOGI(tag, "drew big rect");
    Paint_DrawRectangle(20, 50, 550, 180, WHITE, DOT_PIXEL_1X1, DRAW_FILL_FULL);
    TUERSCHILD_LOGI(tag, "cleared rect");
    Paint_DrawCircle(685, 115, 70, WHITE, DOT_PIXEL_3X3, DRAW_FILL_EMPTY);
    TUERSCHILD_LOGI(tag, "cleared circle");
    Paint_DrawString_EN(30, 20, doc["systeminfo"]["datum"], &Font20, BLACK, WHITE);
    TUERSCHILD_LOGI(tag, "wrote date");
    Paint_DrawString_EN(380, 20, "letzte Aktualisierung:", &Font20, BLACK, WHITE);                
    TUERSCHILD_LOGI(tag, "wrote last udpate date");
    Paint_DrawString_EN(700, 20, doc["systeminfo"]["updateUhrzeit"], &Font20, BLACK, WHITE);
    TUERSCHILD_LOGI(tag, "wrote uddate time");
    if(doc["aktuellesMeeting"]["titel"]) {
        Paint_DrawString_EN(30, 70, doc["aktuellesMeeting"]["titel"], &Font24, WHITE, BLACK);
    }
    TUERSCHILD_LOGI(tag, "wrote current meeting");
    //Paint_DrawString_EN(30, 110, "Meeting: Abschlussvortrag IoT Tuerschild", &Font16, WHITE, BLACK);

    String str1 = "Verantwortlicher: ";
    String str2 = doc["aktuellesMeeting"]["verantwortlicher"];
    str1.concat(str2);
    Paint_DrawString_EN(30, 130,  str1.c_str(), &Font16, WHITE, BLACK);
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
    Paint_DrawString_EN(630, 100, topic_in_buf, &Font24, BLACK, WHITE);
    //Paint_DrawNum(670, 125, 12, &Font24, WHITE, BLACK);

    printf("EPD_Display\r\n");
    EPD_7IN5_V2_Display(BlackImage);

    free(BlackImage);
}