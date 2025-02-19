#include <sys/_stdint.h>
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

#pragma once

#include "Configuration.h"
#include "GCodeParser.h"
#include "UART.h"
#include "SD_Card.h"

class GCode
{
private:
  // float forwardDistance[TOOLS_COUNT];
  // float backwardDistance[TOOLS_COUNT];
  // float wPosistion = 0;
  bool m503_sending = false;

public:
  // enum ReportPort {
  //   UART_1 = 0b00000001,
  //   UART_2 = 0b00000010,
  // };
  uint32_t toolIndex = 0;

  GCode();
  // bool LoadPresets(char * filepart = NULL);
  // bool SavePresets(char * filepart);
  bool M403(GCodeParser *parser);
  void M503(GCodeParser *parser);
  bool isM503Sending() {return m503_sending;};
	// uint32_t T (GCodeParser *parset);
};

extern GCode gcode;
