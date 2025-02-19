#pragma once
#include <HardwareSerial.h>

#define MAIN_VERSION  0
#define SUB_VERSION   3

#define KEY_1_PIN   9
#define KEY_2_PIN   18
#define KEY_3_PIN   19
#define KEY_TYPE            BfButton::STANDALONE_DIGITAL
#define KEY_PULL_UP         true
#define KEY_ACTIVE_LEVEL    LOW
#define KEY_DOUBLE_CLICK_MS 500
#define KEY_LONG_CLICK_MS   2000

#define RGB_PIN     8 // 1
#define FILAMENT_DETECTION_PIN   0

#define Uart1 Serial0
#define Uart2 Serial1
#define UART_BAUDRATE 115200
#define UART_CONFIG   SERIAL_8N1
#define UART_1_RX_PIN 17
#define UART_1_TX_PIN 16
#define UART_2_RX_PIN 20
#define UART_2_TX_PIN 23

// #define UART_READ_LENGTH_LIMIT 255
#define GCODE_LINE_LENGTH   255

#define TOOLS_COUNT   12
// #define MMU_PER_EXTRUSION_DISTANCE    26.0  // 移动选线头后预挤出到断料传感器的距离(mm)
// #define MMU_PER_EXTRUSION_FEED_RATE   300 // 预挤出的速率(mm/s)
#define W_AXIS_FEEDRATE         20 * 60 // 预挤出的速率(mm/min)
#define MMU_FORWARD_DISTANCE    {   0.8,  20.4,  20.4,  20.4,  30.6,  20.4,  20.2,  20.4,  31.1,  20.6,  20.5, 20.4} // 第一个槽位的距离是从W轴原点开始计算
#define MMU_BACKWARD_DISTANCE   { -20.4, -20.4, -20.4, -30.6, -20.4, -20.2, -20.4, -31.1, -20.6, -20.5, -20.4, 0.0}
#define MMU_FILAMENT_BACKUP     {0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11}
#define MMU_FILAMENT_COLOR      {0xFFFFFF, 0xFFFFFF, 0xFFFFFF, 0xFFFFFF, 0xFFFFFF, 0xFFFFFF, 0xFFFFFF, 0xFFFFFF, 0xFFFFFF, 0xFFFFFF, 0xFFFFFF, 0xFFFFFF}


