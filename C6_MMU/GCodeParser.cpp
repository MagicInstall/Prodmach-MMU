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

#include "GCodeParser.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/// <summary>
/// Initializizes class.
/// </summary>
void GCodeParser::Initialize()
{
	codeCharLength = 0;
	codeChars[codeCharLength] = '\0';
	comments = NULL;
	// lastComment = comments;
	// blockDelete = false;
	// beginEnd = false;
	isAvailable = false;
  A = NULL; B = NULL; C = NULL; D = NULL; E = NULL; F = NULL; G = NULL;
  H = NULL; I = NULL; J = NULL; K = NULL; L = NULL; M = NULL; N = NULL;
  O = NULL; P = NULL; Q = NULL; R = NULL; S = NULL; T = NULL; 
  U = NULL; V = NULL; W = NULL; X = NULL; Y = NULL; Z = NULL;
}

/// <summary>
/// Class constructor.
/// </summary>
/// <remark>
/// The G Code language is based on the RS274/NGC language. The G Code 
/// language is based on lines of code. Each line (also called a 'block')
/// may include commands to do several different things. Lines of code
/// may be collected in a file to make a program.
/// </remark>
GCodeParser::GCodeParser()
{
	Initialize();
}

/// <summary>
/// 传入字符的同时解析GCode内容 
/// </summary>
/// <param name="c">1.目前只接受大写的GCode;<br>2.目前只接受以'\n'换行符结尾;<br>3.目前只接受';'作为注释起始符, 所有';'后面的字符都不进行Gcode解析.</param>
/// <returns>返回True 表示单行GCode解释结束.</returns>
/// <remarks>如果上一个是换行符, 此次调用会重置解析器, 上一行的解析数据将被清除.<br>如果单行GCode的长度超过GCODE_LINE_LENGTH, 超过部分将被忽略! 注意检查串口提示.</remarks>
bool GCodeParser::AddChar(char c)
{
	// Determine is a new line is being added.
	if (isAvailable)
		Initialize();

	if (c == '\n')
  {
    if (codeCharLength >= GCODE_LINE_LENGTH) printf("/******** GCode buffer overflow! ********/\n");

		isAvailable = true;
	}
	else
	{
    // 忽略超过缓冲区长度的部分
		if (codeCharLength < GCODE_LINE_LENGTH) 
    {
      // Add character to line.
      codeChars[codeCharLength] = c;

      // 注释符后面的字符不进行解析
      if (!comments) 
      {
        switch (c)
        {
          case ';': comments = &codeChars[codeCharLength]; break;
          case 'A': A = &codeChars[codeCharLength]; break;
          case 'B': B = &codeChars[codeCharLength]; break;
          case 'C': C = &codeChars[codeCharLength]; break;
          case 'D': D = &codeChars[codeCharLength]; break;
          case 'E': E = &codeChars[codeCharLength]; break;
          case 'F': F = &codeChars[codeCharLength]; break;
          case 'G': if (codeChars[codeCharLength + 1] >= '0' && codeChars[codeCharLength + 1] <= '9') G = &codeChars[codeCharLength]; break;
          case 'H': H = &codeChars[codeCharLength]; break;
          case 'I': I = &codeChars[codeCharLength]; break;
          case 'J': J = &codeChars[codeCharLength]; break;
          case 'K': K = &codeChars[codeCharLength]; break;
          case 'L': L = &codeChars[codeCharLength]; break;
          case 'M': if (codeChars[codeCharLength + 1] >= '0' && codeChars[codeCharLength + 1] <= '9') M = &codeChars[codeCharLength]; break;
          case 'N': N = &codeChars[codeCharLength]; break;
          case 'O': O = &codeChars[codeCharLength]; break;
          case 'P': P = &codeChars[codeCharLength]; break;
          case 'Q': Q = &codeChars[codeCharLength]; break;
          case 'R': R = &codeChars[codeCharLength]; break;
          case 'S': S = &codeChars[codeCharLength]; break;
          case 'T': if (codeChars[codeCharLength + 1] >= '0' && codeChars[codeCharLength + 1] <= '9') T = &codeChars[codeCharLength]; break;
          case 'U': U = &codeChars[codeCharLength]; break;
          case 'V': V = &codeChars[codeCharLength]; break;
          case 'W': W = &codeChars[codeCharLength]; break;
          case 'X': X = &codeChars[codeCharLength]; break;
          case 'Y': Y = &codeChars[codeCharLength]; break;
          case 'Z': Z = &codeChars[codeCharLength]; break;
        }

      }
      codeCharLength ++;
      codeChars[codeCharLength] = '\0';
    }

	}

	return isAvailable;
}

// /// <summary>
// /// Parses the line passed removing spaces, tabs and comments. Comments are shifted to the end of the line buffer.
// /// </summary>
// void GCodeParser::ParseLine(char* gCode)
// {
// 	Initialize();

//   int pointer = 0;
//   while (gCode[pointer] != '\0')
//   {
// 	  AddCharToLine(gCode[pointer]);
// 	  pointer ++;
//   }
  	
//   AddCharToLine('\n');
// }


// /// <summary>
// /// Parses the line removing spaces, tabs and comments. Comments are shifted to the end of the line buffer.
// /// </summary>
// void GCodeParser::Parse()
// {
// 	int pointer = 0;
// 	int lineLength = strlen(codeChars);
// 	// bool semicolonFound = false;
// 	// bool openParentheseFound = false;
// 	// int correctCommentsPointerBy = 0;

//   // 跳过前导空格
//   while (codeChars[pointer] == ' ')
//     pointer++;

// 	while (codeChars[pointer] != '\0')
// 	{
// 		char c = codeChars[pointer];

// 		// Look for start of comment.
// 		// if (!semicolonFound && c == '(')
// 		// 	openParentheseFound = true; // Open parenthese... start of comment.

// 		if (!openParentheseFound && c == ';')
// 			semicolonFound = true; // Semicolon... start of comment to end of line.

// 		// If we are inside a comment, we need to move it to the end of the buffer in order to seperate it.
// 		if (openParentheseFound) // || semicolonFound)
// 		{
// 			// Shift line left.
// 			for (int i = pointer; i < lineLength; i++)
// 			{
// 				codeChars[i] = codeChars[i + 1];
// 			}
// 			codeChars[lineLength] = c;
// 		}
// 		else
// 		{
// 			// Spaces and tabs are allowed anywhere on a line of code and do not change the meaning of 
// 			// the line, except inside comments. Remove spaces and tabs except in comments. 
// 			if (c == ' ' || c == '\t')
// 			{
// 				int removeCharacterPointer = pointer;

// 				while (codeChars[removeCharacterPointer] != '\0')
// 				{
// 					codeChars[removeCharacterPointer] = codeChars[removeCharacterPointer + 1];

// 					removeCharacterPointer++;
// 				}

// 				correctCommentsPointerBy++;
// 			}
// 			else
// 				pointer++;
// 		}

// 		// Look for end of comment.
// 		// if (!semicolonFound && c == ')')
// 		// {
// 		// 	openParentheseFound = false;

// 		// 	// Is this the end of the comment? Scan forward for second closing parenthese, but no opening parenthese first.
// 		// 	int scanAheadPointer = pointer;

// 		// 	whilCodeCharsine[scanAheadPointer] != '\0')
// 		// 	{
// 		// 		iCodeCharsine[scanAheadPointer] == '(')
// 		// 			break;

// 		// 		icodeCharsine[scanAheadPointer] == ')')
// 		// 		{
// 		// 			openParentheseFound = true;
// 		// 			break;
// 		// 		}

// 		// 		scanAheadPointer++;
// 		// 	}
// 		// }
// 	}

// 	// Set pointer to comments.
// 	comments = codeChars + strlen(codeChars) + correctCommentsPointerBy + 1;
// 	lastComment = comments;

// 	// There are several 'active' comments which look like comments but cause some action, like
// 	// '(debug,..)' or '(print,..)'. If there are several comments on a line, only the last comment
// 	// will be interpreted according to these rules. For this reason there is a pointer to the last comment.
// 	pointer = 0;
// 	openParentheseFound = false;

// 	while (comments[pointer] != '\0')
// 	{
// 		char c = comments[pointer];

// 		// Open parenthese... start of comment.
// 		if (c == '(')
// 		{
// 			lastComment = comments + pointer;
// 			openParentheseFound = true; 
// 		}

// 		// Semicolon... start of comment to end of line, the last comment.
// 		if (!openParentheseFound && c == ';')
// 		{
// 			lastComment = comments + pointer;
// 			break;
// 		}

// 		// Look for end of comment.
// 		if (c == ')')
// 		{
// 			openParentheseFound = false;

// 			// Is this the end of the comment? Scan forward for second closing parenthese, but no opening parenthese first.
// 			int scanAheadPointer = pointer + 1;

// 			while (comments[scanAheadPointer] != '\0')
// 			{
// 				if (comments[scanAheadPointer] == '(')
// 					break;

// 				if (comments[scanAheadPointer] == ')')
// 				{
// 					openParentheseFound = true;
// 					break;
// 				}

// 				scanAheadPointer++;
// 			}
// 		}

// 		pointer++;
// 	}

// 	// The optional block delete character the slash '/' when placed first on a line can be used
// 	// by some user interfaces to skip lines of code when needed.
// 	blockDelete = (codeChars[0] == '/');

// 	// The '%' is used to demarcate the beginning (first line) and end (last line) of the program. It is optional if the file has an 'M2' or 'M30'. 
// 	beginEnd = (codeChars[0] == '%');
// }

// /// <summary>
// /// Removes the comment seperators for comments and last comment along with any leading spaces.
// /// </summary>
// /// <remark>Once removed they cannot be replaced.</remark>
// void GCodeParser::RemoveCommentSeparators()
// {
// 	int commentsLength = strlen(comments);

// 	int pointer = 0;
// 	bool openParentheseFound = false;
// 	int correctCommentsPointerBy = 0;

// 	while (comments[pointer] != '\0')
// 	{
// 		char c = comments[pointer];

// 		// Look for start of comment.
// 		if (c == '(')
// 		{
// 			comments[pointer] = ' ';
// 			openParentheseFound = true; // Open parenthese... start of comment.
// 		}

// 		if (!openParentheseFound && c == ';')
// 		{
// 			comments[pointer] = ' ';
// 			break;
// 		}

// 		// Look for end of comment.
// 		if (c == ')')
// 		{
// 			openParentheseFound = false;

// 			// Is this the end of the comment? Scan forward for second closing parenthese, but no opening parenthese first.
// 			int scanAheadPointer = pointer + 1;

// 			while (comments[scanAheadPointer] != '\0')
// 			{
// 				if (comments[scanAheadPointer] == '(')
// 					break;

// 				if (comments[scanAheadPointer] == ')')
// 				{
// 					openParentheseFound = true;
// 					break;
// 				}

// 				scanAheadPointer++;
// 			}

// 			if (!openParentheseFound)
// 			{
// 				// Shift line left.
// 				for (int i = pointer; i < commentsLength; i++)
// 				{
// 					comments[i] = comments[i + 1];
// 				}
// 			}
// 			else
// 				pointer++;
// 		}
// 		else
// 			pointer++;
// 	}

// 	while (comments[0] == ' ')
// 	{
// 		// Shift pointer right
// 		comments = comments + 1;
// 	}

// 	while (lastComment[0] == ' ')
// 	{
// 		// Shift pointer right
// 		lastComment = lastComment + 1;
// 	}
// }

/// <summary>
/// 注释的位置(comments 属性)跳过前面的‘;’, ‘\t’, ' '.
/// </summary>  
/// <returns> 返回跳过后的地址(comments 已跳过前缀).
char* GCodeParser::CommentSkipPerfix(void) {
  char *p = comments;
  while (*p != '\0')
  {
    if (*p == ';' || *p == '\t' || *p == ' ')
      p++;  
    else 
      break;
  }
  comments = p;
  return comments;
}

/// <summary>
/// Looks for a word in the line.
/// </summary>
/// <param name="c">The letter of the word to look for in the line.</param>
/// <returns>A pointer to where the word starts.  Points to \0 if the word was not found.</returns>
// int GCodeParser::FindWord(char letter)
// {
// 	int pointer = 0;
// 	bool openParentheseFound = false;
// 	bool semicolonFound = false;

// 	while (codeChars[pointer] != '\0')
// 	{
// 		// Look for the word.
// 		if (letter == codeChars[pointer])
// 		{
// 			return pointer;
// 		}

// 		pointer++;
// 	}

// 	return pointer;
// }

/// <summary>
/// Looks through the code block to determin if a word exist.
/// </summary>
/// <param name="letter">The letter of the GCode word.</param>
/// <returns>True if the word exist on the line.</returns>
bool GCodeParser::HasWord(char letter)
{
	switch (letter)
  {
    case ';': if (comments) return true; break;
    case 'A': if (A) return true; break;
    case 'B': if (B) return true; break;
    case 'C': if (C) return true; break;
    case 'D': if (D) return true; break;
    case 'E': if (E) return true; break;
    case 'F': if (F) return true; break;
    case 'G': if (G) return true; break;
    case 'H': if (H) return true; break;
    case 'I': if (I) return true; break;
    case 'J': if (J) return true; break;
    case 'K': if (K) return true; break;
    case 'L': if (L) return true; break;
    case 'M': if (M) return true; break;
    case 'N': if (N) return true; break;
    case 'O': if (O) return true; break;
    case 'P': if (P) return true; break;
    case 'Q': if (Q) return true; break;
    case 'R': if (R) return true; break;
    case 'S': if (S) return true; break;
    case 'T': if (T) return true; break;
    case 'U': if (U) return true; break;
    case 'V': if (V) return true; break;
    case 'W': if (W) return true; break;
    case 'X': if (X) return true; break;
    case 'Y': if (Y) return true; break;
    case 'Z': if (Z) return true; break;
  }

	return false;
}

// char wordLetter[] = { 'A', 'B', 'C', 'D', 'E', 'F', 'G', 'H', 'I', 'J', 'K', 'L', 'M', 'N', 'P', 'Q', 'R', 'S', 'T', 'U', 'V', 'W', 'X', 'Y', 'Z', '\0' };

/// <summary>
/// Determine if the letter provided represents a valid GCode word.
/// </summary>
/// <param name="letter">The letter to be tested.</param>
/// <returns>True if the letter represents a valid word.</returns>
/// <remark>
/// Words may begin with any of the letters shown in the following
/// Table. The table includes N, @, ^ and / for completeness, even 
/// though, line numbers, polar coordinates and the block delete  
/// character are not considered words. Several letters (I, J, K,
/// L, P, R) may have different meanings in different contexts.
/// Letters which refer to axis names are not valid on a machines
/// which do not have the corresponding axis.
/// 
/// A - A axis of machine.
/// B - B axis of machine.
/// C - C axis of machine.
/// D - Tool radius compensation number.
/// E - E axis of machine.
/// F - Feed rate.
/// G - General function(See table Modal Groups).
/// H - Tool length offset index.
/// I - X offset for arcsand G87 canned cycles.
/// J - Y offset for arcsand G87 canned cycles.
/// K - Z offset for arcsand G87 canned cycles. Spindle - Motion Ratio for G33 synchronized movements.
/// L - generic parameter word for G10, M66and others.
/// M - Miscellaneous function(See table Modal Groups).
/// N - Line number. Line numbers are not considered words.
/// P - Dwell time in canned cyclesand with G4. Key used with G10.
/// Q - Feed increment in G73, G83 canned cycles.
/// R - Arc radius or canned cycle plane.
/// S - Spindle speed.
/// T - Tool selection.
/// U - U axis of machine.
/// V - V axis of machine.
/// W - W axis of machine.
/// X - X axis of machine.
/// Y - Y axis of machine.
/// Z - Z axis of machine
/// @ - Polar coordinate for the distance. Polar coordinates are not considered words.
/// ^ - Polar coordinate for the angle. Polar coordinates are not considered words.
/// / - The block delete character causes the processor to skips the line and is not considered a word.
/// % - Indicated the beginning and end of a program and is not considered a word.
/// </remark>
// bool GCodeParser::IsWord(char letter)
// {	
// 	int pointer = 0;
// 	while (wordLetter[pointer] != '\0')
// 	{
// 		if (letter == wordLetter[pointer])
// 		{
// 			return true;
// 		}

// 		pointer++;
// 	}

// 	return false;
// }

/// <summary>
/// Determine if the line contains any GCode words.
/// </summary>
/// <returns>True if there are no words.</returns>
/// <remarks>Words are not validated.<remark>
// bool GCodeParser::NoWords()
// {codeChars
// 	if (line[0] == '\0' || blockDelete || beginEnd)
// 	{
// 		return true;
// 	}

// 	int pointer = 0;
// 	while (wordLetter[pointer] != '\0')
// 	{
// 		if (HasWord(wordLetter[pointer]))
// 		{
// 			return false;
// 		}

// 		pointer++;
// 	}

// 	return true;
// }

/// <summary>
/// Gets the value following the word.
/// </summary>
/// <param name="letter">传入['A-Z'].</param>
/// <returns>The value following the letter for the word.</returns>
/// <remarks>
/// Currently the parser is not sophisticated enough to deal with parameters, 
/// Boolean operators, expressions, binary operators, functions and repeated items.
/// </remarks>
float GCodeParser::GetWordValue(char letter)
{
  char *val = NULL;
  if (letter >= 'A' && letter <= 'Z')
  {
    switch (letter)
    {
      case 'A': val = A; break;
      case 'B': val = B; break;
      case 'C': val = C; break;
      case 'D': val = D; break;
      case 'E': val = E; break;
      case 'F': val = F; break;
      case 'G': val = G; break;
      case 'H': val = H; break;
      case 'I': val = I; break;
      case 'J': val = J; break;
      case 'K': val = K; break;
      case 'L': val = L; break;
      case 'M': val = M; break;
      case 'N': val = N; break;
      case 'O': val = O; break;
      case 'P': val = P; break;
      case 'Q': val = Q; break;
      case 'R': val = R; break;
      case 'S': val = S; break;
      case 'T': val = T; break;
      case 'U': val = U; break;
      case 'V': val = V; break;
      case 'W': val = W; break;
      case 'X': val = X; break;
      case 'Y': val = Y; break;
      case 'Z': val = Z; break;
    }
  }

  if (val && *val != '\0') {
    return strtof(val + 1, NULL);
  }

	return 0.0;
}

int GCodeParser::GetWordIntValue(char letter) 
{
  return (int)GetWordValue(letter);
}
