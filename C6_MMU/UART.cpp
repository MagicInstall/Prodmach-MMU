#include "esp32-hal.h"
#include <stdio.h>
#include <string.h>
#include <cstddef>
#include <HardwareSerial.h>
#include "UART.h"
#include "SD_Card.h"
#include "GCodeParser.h"
#include "GCode.h"
#include "BfButton.h"

UART uart;

extern BfButton Key_1;
extern BfButton Key_2;
extern BfButton Key_3;
extern GCode gcode;

File _log_file;
// char _rx_1_buff[UART_READ_LENGTH_LIMIT];
// char _rx_2_buff[UART_READ_LENGTH_LIMIT];
GCodeParser *_parser_1 = new GCodeParser();
GCodeParser *_parser_2 = new GCodeParser();
SemaphoreHandle_t _mutex = NULL;

static bool log_to_sd(const char *format, ...) //, char *arg1) 
{
  _log_file = SD.open("/pmmmu.log", FILE_APPEND, true);
  if (!_log_file) {
    return false;
  }

  _log_file.printf("%lums ", millis());

  va_list arg;
  va_start(arg, format);
  _log_file.vprintf(format, arg);
  va_end(arg);
      
  _log_file.flush();
  _log_file.close();
  return true;
}

/// 返回true 表示已处理
static bool process_parsed_command(HardwareSerial *uart, GCodeParser *parser)
{
  if (parser == NULL) return false;

  bool processed = false;

  if (parser->HasWord('G'))
  {

  } 
  else if (parser->HasWord('M'))
  {
    switch (parser->GetWordIntValue('M'))
    {
      case 117: break; // Wifi 模块的文本输出
      case 403: processed = gcode.M403(parser); break;
      case 503: 
        gcode.M503(parser); 
        processed = true;
        break;
    }
  }
  else if (parser->HasWord('T'))
  {
    int param = (parser->HasWord('P')) ? parser->GetWordIntValue('P') : -1;
    int index = (parser->HasWord('T')) ? parser->GetWordIntValue('T') : -1;
    switch (param) {
      case 1:
        if (index > -1 && index < TOOLS_COUNT) {
          MMU_Persets.ToolIndex = index;
          SD_Save_Persets();
          // TODO: 结束换线状态
          Flash_Size = index;
          processed = true;
        }
        break;    
      case 2:
        if (index > -1 && index < TOOLS_COUNT) {
          MMU_Persets.ChangingToolIndex = index;
          SD_Save_Persets();
          // TODO: 
          // 1. 设置开始换线状态  
          // 2. 屏幕闪烁
          SDCard_Size = index;
          processed = true;
        }
        break;
    }
  }
  else return false;

  return processed;
}

static void uart_1_loop (void *pvParameters) {
  while (1) {
    if (Uart1.available()) {
      if (_parser_1->AddChar(Uart1.read()))
      {
        xSemaphoreTake(_mutex, portMAX_DELAY); 
        log_to_sd("M8>%s\n", _parser_1->codeChars);
        if (!process_parsed_command(&Uart1, _parser_1)) {
			    Uart2.printf(_parser_1->codeChars);

			    uart.processReceivedData(_parser_1->codeChars);
		    }
        xSemaphoreGive(_mutex);
      }
    }
    vTaskDelay(1);
  }
}

static void uart_2_loop (void *pvParameters) {
  // int len = 0;
  while (1) {
    // len = readline(&Uart2, _rx_2_buff);
    // if (len > 2) {
    //   xSemaphoreTake(_mutex, portMAX_DELAY); 

    //   _log_file.printf("Wi>%s", _rx_2_buff);
    //   _log_file.flush();

    //   xSemaphoreGive(_mutex);
    // }
    if (Uart2.available()) {
      if (_parser_2->AddChar(Uart2.read()))
      { 
        xSemaphoreTake(_mutex, portMAX_DELAY); 
        log_to_sd("Wi>%s\n", _parser_2->codeChars);
        if (!process_parsed_command(&Uart2, _parser_2)) uart.pushToSendQueue(_parser_2->codeChars);
        xSemaphoreGive(_mutex);
      }
    }
    vTaskDelay(1);
  }
}

static void onKeyCb(BfButton *btn, BfButton::press_pattern_t pattern) {
  if (btn == &Key_1) {
    printf("Key_1:"); 
  } else if ( btn == &Key_2 ) {
    printf("Key_2:"); 
  } else if ( btn == &Key_3 ){
    printf("Key_3:"); 
  }
  if (pattern == BfButton::SINGLE_PRESS) {
    printf("Single Press\n"); 
  } else if (pattern == BfButton::DOUBLE_PRESS) {
    printf("Double Press\n"); 
  } else if (pattern == BfButton::LONG_PRESS){
    printf("Long Press\n"); 
  }
}

static void onKey_1_SingleClickCb(BfButton *btn, BfButton::press_pattern_t pattern) {
  printf("Button 1 single click\n");
  if (MMU_Persets.ToolIndex + 1 < TOOLS_COUNT && MMU_Persets.ToolIndex == MMU_Persets.ChangingToolIndex) {
    uart.pushToSendQueue("T%d\n", MMU_Persets.ToolIndex + 1); 
  }
}

static void onKey_2_LongClickCb(BfButton *btn, BfButton::press_pattern_t pattern) {
  printf("Button 2 long click\n");
  if (1) {// TODO: 若切换头有线则不允许回原点
    uart.pushToSendQueue("G28 W\n"); 
    uart.pushToSendQueue("G1 W%f F%d\n", MMU_Persets.ForwardDistance[0], W_AXIS_FEEDRATE); 
    uart.pushToSendQueue("M403 P1 I0\n");
    // TODO: 归零过程目前不能等M8同步
    MMU_Persets.ToolIndex = 0;
    MMU_Persets.ChangingToolIndex = 0;
    SD_Save_Persets();
    Flash_Size = 0;
  }
}

static void onKey_3_SingleClickCb(BfButton *btn, BfButton::press_pattern_t pattern) {
  printf("Button 3 single click\n");
  if (MMU_Persets.ToolIndex > 0 && MMU_Persets.ToolIndex == MMU_Persets.ChangingToolIndex) {
    uart.pushToSendQueue("T%d\n", MMU_Persets.ToolIndex - 1); 
  }
}

UART::UART() : head(0), tail(0), timeout(2000), delayMin(20) {
    queueMutex = xSemaphoreCreateMutex();
    assert(queueMutex);

    for (size_t i = 0; i < MAX_COMMAND_COUNT; ++i) {
        // commands[i].used = false;
        // commands[i].serial = NULL;
    }
}

bool UART::begin (unsigned long baud, uint32_t config) {
  Uart1.begin(baud, config, UART_1_RX_PIN, UART_1_TX_PIN); 
  Uart2.begin(baud, config, UART_2_RX_PIN, UART_2_TX_PIN);

  _mutex = xSemaphoreCreateMutex();
  assert(_mutex);

  xTaskCreatePinnedToCore(uart_1_loop, "UART 1 task", 4096, NULL, 1, NULL, 0);
  xTaskCreatePinnedToCore(uart_2_loop, "UART 2 task", 4096, NULL, 1, NULL, 0);

  Key_1.onPress(onKey_1_SingleClickCb)
       .onDoublePress(onKeyCb, 500)     
       .onPressFor(onKeyCb, 3000);  
  Key_2.onPress(onKeyCb)
       .onDoublePress(onKeyCb, 500)     
       .onPressFor(onKey_2_LongClickCb, 3000);  
  Key_3.onPress(onKey_3_SingleClickCb)
       .onDoublePress(onKeyCb, 500)     
       .onPressFor(onKeyCb, 3000);

  return true;
}

int UART::pushToSendQueue(const char *format, ...) {
    xSemaphoreTake(queueMutex, portMAX_DELAY); 

    if ((tail + 1) % MAX_COMMAND_COUNT == head) {
        // 队列满了，无法再添加新命令
        log_to_sd("UART push failed: Queue full!\n");
        return -1;
    }

    va_list args;
    va_start(args, format);
    // 将命令复制到数组中
    int len = vsnprintf(commands[tail].command, GCODE_LINE_LENGTH, format, args);
    va_end(args);

    if (len >= GCODE_LINE_LENGTH) {
      // 不允许发送不完整的gcode
      log_to_sd("UART push failed: Chars too long!\n");

      xSemaphoreGive(queueMutex);
      return -2;
    }
    // commands[tail].used = true;
    // commands[tail].serial = serial;

    // 更新尾部索引
    tail = (tail + 1) % MAX_COMMAND_COUNT;

    xSemaphoreGive(queueMutex);
    return 0;
}

void UART::checkSend() {
    if (isBusy()) {
        // 超时处理
        if (millis() - lastSendTime > timeout) {
          // 超时则发送下一条 
          xSemaphoreTake(queueMutex, portMAX_DELAY); 

          Uart1.print(commands[head].command);
          // String command = String(commands[head].command);
          // Serial.println(command);  // 发送命令
          // Serial.flush();           // 等待命令发送完成
          // isWaitingReply = true;
          lastSendTime = millis();  // 记录发送时间

          // commands[head].command[0] = '\0';
          // commands[head].serial = NULL;

          // 更新头部索引，指向下一条命令
          head = (head + 1) % MAX_COMMAND_COUNT;

          xSemaphoreGive(queueMutex);
        }
    }
}

void UART::processReceivedData(const char* reply) {
    String responseBuffer(reply);

    // 如果缓冲区中有完整的回复（假设以换行符结束）
    if (responseBuffer.indexOf('\n') != -1) {
        responseBuffer.trim();  // 去掉首尾的空白字符

		// 如果是`ok`或`busy`之类的回复，继续发送下一个命令
        if (responseBuffer.indexOf("ok") != -1 ) { //|| responseBuffer.indexOf("busy") != -1) {
            if (isBusy()) {
                _log_file.printf("Got an OK\n");
                lastSendTime -= lastSendTime > timeout ? timeout : lastSendTime;  // 让handle() 能马上发送下一条指令
            } else {
                // 如果队列中没有更多命令
                // isSending = false;  // 停止发送
            }
        } else {
            
        }
    }
}

bool UART::isBusy() {
    return head != tail;
}

