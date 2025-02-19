/*
 * @Author: wing zlcnkkm@21cn.com
 * @Date: 2025-01-05 17:20:05
 * @LastEditors: wing zlcnkkm@21cn.com
 * @LastEditTime: 2025-02-08 15:44:15
 * @FilePath: /C6_MMU/UART.h
 * @Description: 这是默认设置,请设置`customMade`, 打开koroFileHeader查看配置 进行设置: https://github.com/OBKoro1/koro1FileHeader/wiki/%E9%85%8D%E7%BD%AE
 */
#pragma once

#include <Arduino.h>
#include "Configuration.h"

extern uint16_t SDCard_Size;
extern uint16_t Flash_Size;

// #ifndef Uart1
//   #define Uart1 Serial1
//   #ifdef Uart2
//     #undef Uart2
//   #endif
//   #define Uart2 Serial0
// #endif

// #ifndef UART_READ_LENGTH_LIMIT
//   #define UART_READ_LENGTH_LIMIT 256
// #endif


// bool Uart_Begin (unsigned long baud, uint32_t config); //, int8_t rx1Pin = -1, int8_t tx1Pin = -1, int8_t rx2Pin = -1, int8_t tx2Pin = -1);


class UART {
private:
    static const size_t MAX_COMMAND_COUNT = 1 + (TOOLS_COUNT * 3) + 3/*Home W*/ + 1/*备用*/; // 总节点数

    // 定义链表节点
    struct CommandNode {
        char command[GCODE_LINE_LENGTH];  // 用 char 数组存储命令
        // HardwareSerial *serial; // 使用哪个串口发送
        // bool used;  // 标记该节点是否已被使用
    };

    CommandNode commands[MAX_COMMAND_COUNT];  // 固定大小的命令节点数组
    size_t head;  // 队列头部（指向当前正在发送的命令）
    size_t tail;  // 队列尾部（指向最新添加的命令）
    bool isProcessing;  // processReceivedData()正在处理接收数据时
    unsigned long lastSendTime;  // 上次发送的时间
    unsigned long timeout;  // 超时等待时间
    unsigned long delayMin;  // 两条命令之间最小发送间隔
    SemaphoreHandle_t queueMutex; // 队列操作的互斥锁


protected:
    // 发送链表中的下一条命令
    // void sendNextCommand();

public:
    // 构造函数
    UART();

    // 启动串口
    static bool begin (unsigned long baud, uint32_t config);

    /**
    * 将数据推送到发送队列中等待发送。
    * 
    * @note 只对Uart1 使用队列功能, push 到队列的code 全部由Uart1 发送.
    * 
    * @param format 与print() 系列的参数一样.
    * 
    * @return 0表示已成功入队, -1表示队列已满不能入队, -2表示格式化后的字符串长度超过GCODE_LINE_LENGTH, 不能入队。
    */
    int pushToSendQueue(const char *format, ...);

    // 在主循环中处理发送逻辑
    void checkSend();

    // 解释器调用此方法处理串口回复数据
    void processReceivedData(const char *reply);

    // 获取当前是否正在发送命令
    bool isBusy();
};

extern UART uart;

