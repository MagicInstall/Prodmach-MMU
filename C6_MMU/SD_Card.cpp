#include "FS.h"
#include "SD_Card.h"

uint16_t SDCard_Size;
uint16_t Flash_Size;

External_Persets MMU_Persets;

bool SD_Init() {
  // SD
  pinMode(SD_CS, OUTPUT);    
  digitalWrite(SD_CS, HIGH);               
  // 已在LCD中初始化过SPI
  if (SD.begin(SD_CS, SPI)) {
    printf("SD card initialization successful!\r\n");
  } else {
    printf("SD card initialization failed!\r\n");
  }
  uint8_t cardType = SD.cardType();
  if(cardType == CARD_NONE){
    printf("No SD card attached\r\n");
    return false;
  }
  else{
    printf("SD Card Type: ");
    if(cardType == CARD_MMC){
      printf("MMC\r\n");
    } else if(cardType == CARD_SD){
      printf("SDSC\r\n");
    } else if(cardType == CARD_SDHC){
      printf("SDHC\r\n");
    } else {
      printf("UNKNOWN\r\n");
    }
    uint64_t totalBytes = SD.totalBytes();
    uint64_t usedBytes = SD.usedBytes();
    SDCard_Size = totalBytes/(1024*1024);
    printf("Total space: %llu\n", totalBytes);
    printf("Used space: %llu\n", usedBytes);
    printf("Free space: %llu\n", totalBytes - usedBytes);
  }
  return true;
}
bool File_Search(const char* directory, const char* fileName)    
{
  File Path = SD.open(directory);
  if (!Path) {
    printf("Path: <%s> does not exist\r\n",directory);
    return false;
  }
  File file = Path.openNextFile();
  while (file) {
    if (strcmp(file.name(), fileName) == 0) {                           
      if (strcmp(directory, "/") == 0)
        printf("File '%s%s' found in root directory.\r\n",directory,fileName);  
      else
        printf("File '%s/%s' found in root directory.\r\n",directory,fileName); 
      Path.close();                                                     
      return true;                                                     
    }
    file = Path.openNextFile();                                        
  }
  if (strcmp(directory, "/") == 0)
    printf("File '%s%s' not found in root directory.\r\n",directory,fileName);           
  else
    printf("File '%s/%s' not found in root directory.\r\n",directory,fileName);          
  Path.close();                                                         
  return false;                                                         
}
uint16_t Folder_retrieval(const char* directory, const char* fileExtension, char File_Name[][100],uint16_t maxFiles)    
{
  File Path = SD.open(directory);
  if (!Path) {
    printf("Path: <%s> does not exist\r\n",directory);
    return false;
  }
  
  uint16_t fileCount = 0;
  char filePath[100];
  File file = Path.openNextFile();
  while (file && fileCount < maxFiles) {
    if (!file.isDirectory() && strstr(file.name(), fileExtension)) {
      strncpy(File_Name[fileCount], file.name(), sizeof(File_Name[fileCount])); 
      if (strcmp(directory, "/") == 0) {                                      
        snprintf(filePath, 100, "%s%s", directory, file.name());   
      } else {                                                            
        snprintf(filePath, 100, "%s/%s", directory, file.name());
      }
      printf("File found: %s\r\n", filePath);
      fileCount++;
    }
    file = Path.openNextFile();                                      
  }
  Path.close();                                                         
  if (fileCount > 0) {
    printf(" %d <%s> files were retrieved\r\n",fileCount,fileExtension);
    return fileCount;                                                 
  } else {
    printf("No files with extension '%s' found in directory: %s\r\n", fileExtension, directory);
    return 0;                                                         
  }
}

void remove_file_extension(char *file_name) {
  char *last_dot = strrchr(file_name, '.');
  if (last_dot != NULL) {
    *last_dot = '\0'; 
  }
}

void Flash_test()
{
  printf("/********** RAM Test**********/\r\n");
  // Get Flash size
  uint32_t flashSize = ESP.getFlashChipSize();
  Flash_Size = flashSize/1024/1024;
  printf("Flash size: %d MB \r\n", flashSize/1024/1024);

  printf("/******* RAM Test Over********/\r\n\r\n");
}

void Load_Default_Persets(void) 
{
  MMU_Persets = {
    {
      sizeof(External_Persets),
      MAIN_VERSION,
      SUB_VERSION  
    },

    UART_BAUDRATE,

    "\0",
    "\0",

    0, 0,
    1, 0, // ChangingToolIndex 初始化故意与ToolIndex 不一致,让用户手动执行一次归零
    // MMU_PER_EXTRUSION_DISTANCE,
    // MMU_PER_EXTRUSION_FEED_RATE,
    // W_AXIS_FEEDRATE,
    MMU_FORWARD_DISTANCE,
    MMU_BACKWARD_DISTANCE,
    MMU_FILAMENT_BACKUP,
    MMU_FILAMENT_COLOR
  };
  printf("The default presets are loaded\n");
}

#define PERSET_FILE_HEAD_LEN  sizeof(External_Persets::Head)

bool SD_Load_Persets(void)
{
  bool successful = false;
  File file = SD.open(PERSET_FILE, FILE_READ);
  if (file)
  {
    size_t count;
    // 1.读Head部分
    // while (!file.available()) {};
    count = file.read((uint8_t*)(&MMU_Persets), PERSET_FILE_HEAD_LEN);
    if (count == PERSET_FILE_HEAD_LEN) 
    {
      if (MMU_Persets.Head.MainVersion == MAIN_VERSION && MMU_Persets.Head.SubVersion == SUB_VERSION)
      {
        // 2.读余下部分
        // while (!file.available()) {};
        count = file.read((uint8_t*)(&MMU_Persets) + PERSET_FILE_HEAD_LEN, sizeof(External_Persets) - PERSET_FILE_HEAD_LEN);
        
        if (count == sizeof(External_Persets) - PERSET_FILE_HEAD_LEN)
        {
          successful = true;  

          printf("/****** Presets ******/\n");
          printf("PersetSize:%d\n", MMU_Persets.Head.PersetSize);
          printf("Version:%d.%d\n", MMU_Persets.Head.MainVersion, MMU_Persets.Head.SubVersion);
          printf("UartBaudRate:%d\n", MMU_Persets.UartBaudRate);
          printf("WifiSSID:%s\n", MMU_Persets.WifiSSID);
          printf("WifiPassword:%s\n", MMU_Persets.WifiPassword);
          printf("T%d\n", MMU_Persets.ToolIndex);
          // printf("PerExtrusionDistance:%f\n", MMU_Persets.PerExtrusionDistance);
          // printf("PerExtrusionFeedRate:%d\n", MMU_Persets.PerExtrusionFeedRate);
          // printf("WAxisFeedRate:%d\n", MMU_Persets.WAxisFeedRate);
          printf("ForwardDistance:{");
          for (int i = 0; i < TOOLS_COUNT; i++) printf("%f, ", MMU_Persets.ForwardDistance[i]);
          printf("}\n");
          printf("BackwardDistance:{");
          for (int i = 0; i < TOOLS_COUNT; i++) printf("%f, ", MMU_Persets.BackwardDistance[i]);
          printf("}\n");
        }
        else {
          printf("/! SD_Load: Wrong file length !/\n");
        }
      }
      else // 升级预设
      {
        printf("Need to update presets from %d.%d to %d.%d...\n", MMU_Persets.Head.MainVersion, MMU_Persets.Head.SubVersion, MAIN_VERSION, SUB_VERSION);
        // TODO
      }
    }
    else 
    {
      printf("/! SD_Load: Not a valid file head !/\n");
    }
    file.close();
  }

  return successful;
}

bool SD_Save_Persets(void)
{
  File file = SD.open(PERSET_FILE, FILE_WRITE, true);
  if (file)
  {
    size_t cnt = file.write((uint8_t*)(&MMU_Persets), sizeof(External_Persets));
    if (cnt != sizeof(External_Persets)) printf("/****** SD_Save: Wrong file length ! ******/\n");
    file.close();
    return true;  
  }
  return false;
}