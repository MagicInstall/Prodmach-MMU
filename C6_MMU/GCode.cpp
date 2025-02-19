#include "Configuration.h"
/*
MIT License

Copyright (c) 2021 Terence Golla

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
SOFTWARE.
*/

#include "GCode.h"

// #include <stdio.h>
// #include <stdlib.h>
// #include <string.h>
// #define SEND_CODE_DELAY_MS 10

GCode gcode;
extern External_Persets MMU_Persets;


/// <summary>
/// Class constructor.
/// </summary>
/// <remark>
/// The G Code language is based on the RS274/NGC language. The G Code 
/// language is based on lines of code. Each line (also called a 'block')
/// may include commands to do several different things. Lines of code
/// may be collected in a file to make a program.
/// </remark>
GCode::GCode()
{
	
}

// bool GCode::LoadPresets(char * filepart) 
// {
//   return true;
// }

// bool GCode::SavePresets(char * filepart)
// {
//   return true;
// }

/// 验证M8上的参数
///
/// 若M8发来的参数与MMU不一致, 则重新发一次
///
/// P1  : I[ToolIndex]
/// P2  : I[index] W[ForwardDistance]
/// P3  : I[index] W[BackwardDistance]
/// P4  : I[index] B[FilamentBackup]
/// P5  : I[index] C[FilamentColor]
///
/// 返回true 表示已处理
bool GCode::M403(GCodeParser *parser)
{
  int p = parser && parser->HasWord('P') ? parser->GetWordIntValue('P') : -1;
  if (p < 1) return false;

  switch (p)
  {
    case 1: // ToolIndex
      if (parser->HasWord('I')) {
        if (MMU_Persets.ToolIndex != (uint8_t)(parser->GetWordIntValue('I'))) {
          uart.pushToSendQueue("M403 P1 I%d\n", MMU_Persets.ToolIndex); 
        }
      }
      break;
    case 2: // ForwardDistance
      if (parser->HasWord('I') && (parser->HasWord('W')))
      {
        int i = parser->GetWordIntValue('I');
        if (i < 0 || i >= TOOLS_COUNT) return false;
        if (MMU_Persets.ForwardDistance[i] != parser->GetWordValue('W'))
          uart.pushToSendQueue("M403 P2 I%d W%.2f\r\n", i, MMU_Persets.ForwardDistance[i]); 
      }
      else return false;
      break;
    case 3: // BackwardDistance
      if (parser->HasWord('I') && (parser->HasWord('F')))
      {
        int i = parser->GetWordIntValue('I');
        if (i < 0 || i >= TOOLS_COUNT) return false;
        if (MMU_Persets.BackwardDistance[i] != parser->GetWordValue('F'))
          uart.pushToSendQueue("M403 P3 I%d W%.2f\n", i, MMU_Persets.BackwardDistance[i]); 
      }
      else return false;
      break;
    case 4: // FilamentBackup
      if (parser->HasWord('I') && (parser->HasWord('D')))
      {
        int i = parser->GetWordIntValue('I');
        if (i < 0 || i >= TOOLS_COUNT) return false;
        if (MMU_Persets.FilamentBackup[i] != (uint8_t)parser->GetWordIntValue('D'))
          uart.pushToSendQueue("M403 P4 I%d B%d\n", i, MMU_Persets.FilamentBackup[i]); 
      }
      else return false;
      break;
    case 5: // FilamentColor
      if (parser->HasWord('I') && (parser->HasWord('D')))
      {
        int i = parser->GetWordIntValue('I');
        if (i < 0 || i >= TOOLS_COUNT) return false;
        if (MMU_Persets.FilamentColor[i] != (uint32_t)parser->GetWordIntValue('D'))
          uart.pushToSendQueue("M403 P5 I%d C%d\n", MMU_Persets.FilamentColor[i]); 
      }
      else return false;
      break;

    // default: 
      // M503(NULL); // 参数错误的话全部参数重发一次
      // break;
  }
  return true;
}

/// 打印机查询MMU参数
/// 不带参数表示请求全部设置;
/// 回复:M403 P[param] I[index] W/B/C[val] 
/// P 参数具体含意参照M403;
void GCode::M503(GCodeParser *parset)
{
  // -1 表示没有P 参数
  int param = (parset != NULL && parset->HasWord('P')) ? parset->GetWordIntValue('P') : -1;
  int index = (parset != NULL && parset->HasWord('I')) ? parset->GetWordIntValue('I') : -1;
  
  // Tool Index
  if (param == -1 || param == 1) {
    if (MMU_Persets.ToolIndex == MMU_Persets.ChangingToolIndex)
      uart.pushToSendQueue("M403 P1 I%d\n", MMU_Persets.ToolIndex);
    // else // 若上次换线未完成则发-1 让M8 无法ready
    //   uart.pushToSendQueue("M403 P1 I-1\n");
  }

  // Forward Distance
  if (param == -1 || (param == 2 && index == -1)) 
  {
    for (int i = 0; i < TOOLS_COUNT; i++)
    {
      uart.pushToSendQueue("M403 P2 I%d W%.2f\n", i, MMU_Persets.ForwardDistance[i]);
    }  
  } 
  else if (param == 2) 
  {
    if (index > -1 && index < TOOLS_COUNT)
      uart.pushToSendQueue("M403 P2 I%d W%.2f\n", index, MMU_Persets.ForwardDistance[index]);
    // else // 索引号超标
  }
  
  // BackwardDistance
  if (param == -1 || (param == 2 && index == -1)) {
    for (int i = 0; i < TOOLS_COUNT; i++)
    {
      uart.pushToSendQueue("M403 P3 I%d W%.2f\n", i, MMU_Persets.BackwardDistance[i]);
    } 
  }
  else if (param == 3) {
    if (index > -1 && index < TOOLS_COUNT)
      uart.pushToSendQueue("M403 P3 I%d W%.2f\n", index, MMU_Persets.BackwardDistance[index]);
    // else // 索引号超标
  }

  // FilamentBackup
  if (param == -1 || (param == 2 && index == -1)) {
    for (int i = 0; i < TOOLS_COUNT; i++)
    {
      uart.pushToSendQueue("M403 P4 I%d B%d\n", i, MMU_Persets.FilamentBackup[i]);
    }
  }
  else if (param == 4){
    if (index > -1 && index < TOOLS_COUNT)
      uart.pushToSendQueue("M403 P4 I%d B%d\n", index, MMU_Persets.FilamentBackup[index]);
    // else // 索引号超标
  }

}

/// <summary>
/// 换料
///
/// 目前MMU此端的T代码只负责接收换线事件
///
/// T[槽号]
/// 槽号从1开始;
/// 
/// 参数: 
/// S 新线的温度
/// P1:换线完成
/// P2:正在换线
/// </summary>
// uint32_t GCode::T (GCodeParser *parser)
// {
  /* 已迁移到UART 中处理 */
  
  // int new_tool = parser->GetWordIntValue('T'); 
  // if (new_tool == MMU_Persets.ToolIndex) return MMU_Persets.ToolIndex; 

  // // TODO: 检查传感器, 如果线未退出就报错;或者先发 M702 ?

  // // int direction = (new_tool > MMU_Persets.ToolIndex) ? 1 : -1;

  // int i;
  // if (new_tool > MMU_Persets.ToolIndex)
  // {
  //   // 向前
  //   for (i = MMU_Persets.ToolIndex; i < new_tool; i++)
  //   {
  //     wPosistion += ForwardDistance[i + 1];
  //   }
  // }
  // else
  // {
  //   // 向后
  //   for (i = MMU_Persets.ToolIndex; i > new_tool; i--)
  //   {
  //     wPosistion += ForwardDistance[i - 1];
  //   }
  // }
  
  
  // uart.pushToSendQueue("G1 W%.2lf F%d\n", wPosistion, MMU_Persets.WAxisFeedRate);
  // // TODO: 等待M8回复

  // // 挤出到切换头传感器
	// uart.pushToSendQueue("G1 E%.2lf F%d\n", MMU_Persets.PerExtrusionDistance, MMU_Persets.PerExtrusionFeedRate);
  // // TODO: 等待M8回复

  // // TODO: 检查传感器

//   return true;
// }



