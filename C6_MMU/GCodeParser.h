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


/// <summary>
/// 简易GCode解释器<br>
/// 
/// 目前不支持代码嵌套, 单行中出现嵌套代码将引起意外结果!!!
/// </summary>
class GCodeParser
{
private:
	int codeCharLength;

public:
	char codeChars[GCODE_LINE_LENGTH + 1];
	char* comments;
  char* A, * B, * C, * D, * E, * F, * G, * H, * I, * J, * K, * L, * M, * N, * O, * P, * Q, * R, * S, * T, * U, * V, * W, * X, * Y, * Z;
	// char* lastComment;
	// bool blockDelete;
	// bool beginEnd;
	bool isAvailable;

	GCodeParser();
	void Initialize();
	bool AddChar(char c);
	// void ParseLine();
	// void ParseLine(char* gCode);
	// void RemoveCommentSeparators();  
  char* CommentSkipPerfix(void);

	// int FindWord(char letter);
	bool HasWord(char letter);
	// bool IsWord(char letter);
	// bool NoWords();

	float GetWordValue(char letter);
  int GetWordIntValue(char letter);
};
