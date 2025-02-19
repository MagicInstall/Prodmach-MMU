/*
 * @Author: wing
 * @Date: 2024-12-01
 * @LastEditors: wing zlcnkkm@21cn.com
 * @LastEditTime: 2025-02-19 23:41:56
 */
#include <Arduino.h>
#include <BfButton.h>
#include <FastLED.h>
#include "SD_Card.h"
#include "Display_ST7789.h"
#include "LVGL_Driver.h"
#include "LVGL_Example.h"
#include "UART.h"

BfButton Key_1(KEY_TYPE, KEY_1_PIN, KEY_PULL_UP, KEY_ACTIVE_LEVEL);
BfButton Key_2(KEY_TYPE, KEY_2_PIN, KEY_PULL_UP, KEY_ACTIVE_LEVEL);
BfButton Key_3(KEY_TYPE, KEY_3_PIN, KEY_PULL_UP, KEY_ACTIVE_LEVEL);

#define NUM_LEDS 1
CRGB ledColor[NUM_LEDS];
const uint8_t huePalette[] PROGMEM = {128, 129, 130, 131, 132, 133, 134, 135, 136, 137, 138, 139, 140, 141, 142, 143, 144, 145, 146, 147, 148, 149, 150, 151, 152, 153, 154, 155, 156, 157, 158, 159, 160, 161, 162, 163, 164, 165, 166, 167, 168, 169, 170, 171, 172, 173, 174, 175, 176, 177, 178, 179, 180, 181, 182, 183, 184, 185, 186, 187, 188, 189, 190, 191, 192, 193, 194, 195, 196, 197, 198, 199, 200, 201, 202, 203, 204, 205, 206, 207, 208, 209, 210, 211, 212, 213, 214, 215, 216, 217, 218, 219, 220, 221, 222, 223, 224, 225, 226, 227, 228, 229, 230, 231, 232, 233, 234, 235, 236, 237, 238, 239, 240, 241, 242, 243, 244, 245, 246, 247, 248, 249, 250, 251, 252, 253, 254, 255, 254, 253, 252, 251, 250, 249, 248, 247, 246, 245, 244, 243, 242, 241, 240, 239, 238, 237, 236, 235, 234, 233, 232, 231, 230, 229, 228, 227, 226, 225, 224, 223, 222, 221, 220, 219, 218, 217, 216, 215, 214, 213, 212, 211, 210, 209, 208, 207, 206, 205, 204, 203, 202, 201, 200, 199, 198, 197, 196, 195, 194, 193, 192, 191, 190, 189, 188, 187, 186, 185, 184, 183, 182, 181, 180, 179, 178, 177, 176, 175, 174, 173, 172, 171, 170, 169, 168, 167, 166, 165, 164, 163, 162, 161, 160, 159, 158, 157, 156, 155, 154, 153, 152, 151, 150, 149, 148, 147, 146, 145, 144, 143, 142, 141, 140, 139, 138, 137, 136, 135, 134, 133, 132, 131, 130, 129};
const uint8_t briPalette[] PROGMEM = {63, 67, 71, 75, 79, 83, 87, 91, 95, 99, 103, 107, 111, 115, 119, 123, 127, 131, 135, 139, 143, 147, 151, 155, 159, 163, 167, 171, 175, 179, 183, 187, 191, 195, 199, 203, 207, 211, 215, 219, 223, 227, 231, 235, 239, 243, 247, 251, 255, 251, 247, 243, 239, 235, 231, 227, 223, 219, 215, 211, 207, 203, 199, 195, 191, 187, 183, 179, 175, 171, 167, 163, 159, 155, 151, 147, 143, 139, 135, 131, 127, 123, 119, 115, 111, 107, 103, 99, 95, 91, 87, 83, 79, 75, 71, 67, 63};

void updateLEDColor() {
  // static uint32_t delay = 0;
  static uint32_t h = 0, b = 0;
  // if (delay ++ > 0) {
  //   delay = 0;
  
    if (h >= sizeof(huePalette)) h = 0;
    if (b >= sizeof(briPalette)) b = 0;
    ledColor[0] = CHSV(pgm_read_byte(&huePalette[h++]), 255, pgm_read_byte(&briPalette[b++]));
    FastLED.show();
  // }
}

void setup()
{       
  Flash_test();
  LCD_Init();
  Lvgl_Init();

  SD_Init();
  if (SD_Load_Persets()) 
    printf("Presets loaded !\n");
  else
  {
    Load_Default_Persets();
    // TODO: 载入默认设置后提示W轴回原点
  // MMU_Persets.TIndex = 0;
    if (SD_Save_Persets()) printf("Presets saved !\n");
  }

  UART::begin(MMU_Persets.UartBaudRate, UART_CONFIG); //, UART_2_RX, UART_2_TX);

  Lvgl_Example1(); //Wireless_Test2();
  // lv_demo_widgets();               
  // lv_demo_benchmark();          
  // lv_demo_keypad_encoder();     
  // lv_demo_music();  
  // lv_demo_stress();   

  FastLED.addLeds<WS2812B, RGB_PIN, GRB>(ledColor, NUM_LEDS);
}

void loop()
{
  Key_1.read();
  Key_2.read();
  Key_3.read();
  Timer_Loop();
  uart.checkSend();
  updateLEDColor();
  delay(10);
}