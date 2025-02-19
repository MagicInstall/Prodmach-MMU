#include <sys/_stdint.h>
#include <sys/types.h>
#include <sys/stat.h>
#pragma once
#include "Arduino.h"
#include <cstring>
#include "Display_ST7789.h"
#include "SD.h"
#include "FS.h"
#include "Configuration.h"

// Digital I/O used
#define SD_CS     4        //                SD_D3:

#define PERSET_FILE   "/pmmmu.conf"
#define LOG_FILE      "/pmmmu.log"

extern uint16_t SDCard_Size;
extern uint16_t Flash_Size;


struct External_Persets_0_3
{
  struct 
  {
    uint32_t  PersetSize;   //  整个预设占用的字节数, 用于当预设版本迭代时防止读写溢出
    uint16_t  MainVersion;
    uint16_t  SubVersion;    
  } Head;

  uint32_t  UartBaudRate;

  char      WifiSSID[256];
  char      WifiPassword[128];      

  uint8_t   ToolIndex;              // 从0开始, 0表示第一个槽位
  char      Place1;                 // 1字节占位符
  uint8_t   ChangingToolIndex;      // 用于检测上次换线是否完成, ChangingToolIndex == ToolIndex 表示上次换线正常完成.
  char      Place2;                 // 1字节占位符
  // float     PerExtrusionDistance;   // 移动选线头后预挤出到断料传感器的距离(mm)
  // uint16_t  PerExtrusionFeedRate;   // 预挤出的速率(mm/s)
  // uint16_t  WAxisFeedRate;          // W轴的速率(mm/s)
  float     ForwardDistance[TOOLS_COUNT];
  float     BackwardDistance[TOOLS_COUNT];
  uint8_t   FilamentBackup[TOOLS_COUNT];  // 每个槽可指定一个备用槽, 若备用槽号等于当前槽号表示没有指定备用槽.
  uint32_t  FilamentColor[TOOLS_COUNT];  
}; 

#define External_Persets External_Persets_0_3

extern External_Persets MMU_Persets;

bool SD_Init();
void Flash_test();

bool File_Search(const char* directory, const char* fileName);
uint16_t Folder_retrieval(const char* directory, const char* fileExtension, char File_Name[][100],uint16_t maxFiles);
void remove_file_extension(char *file_name);

void Load_Default_Persets(void);
bool SD_Load_Persets(void);
bool SD_Save_Persets(void);

