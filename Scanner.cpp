#include <iostream>
#include <string>
#include <cctype>
#include <fstream>
#include <stdlib.h>
#include <vector>
#include "Token.h"
#include "Scanner.h"
#include "Parser.h"


using namespace std;

Token* token;
char currentChar;
int* linePos;
int lineNum = 1;
const int MAX_TK = 8;
Token* TK;
ifstream dataFile;

Token* nextTK()
{
	dataFile.get(currentChar);
	while (isspace(currentChar))
	{
		if (currentChar == '\n')
		{
			lineNum++;
		}
		dataFile.get(currentChar);
		if (currentChar == EOF || dataFile.peek() == EOF)
		{
			TK = createToken(TK_EOF, lineNum);
			TK->value = "EOF";
			return TK;
		}
	}
	if (currentChar == '#')
	{
		skipComment();
		dataFile.get(currentChar);
		while (isspace(currentChar)) // need to skip all white space characters again
		{
			if (currentChar == '\n')
			{
				lineNum++;
			}
			dataFile.get(currentChar);
			if (currentChar == EOF || dataFile.peek() == EOF)
			{
				TK = createToken(TK_EOF, lineNum);
				TK->value = "EOF";
				return TK;
			}
		}
	}
	testInput(currentChar);
	return TK;
}

Token* testInput(char currentChar)
{
	if (dataFile.peek() == EOF)
	{
		TK = createToken(TK_EOF, lineNum);
		TK->value = "EOF";
		return TK;
	}

	else if (isdigit(currentChar))
	{
		int loops = 0;
		string input;
		while (isdigit(currentChar) && !dataFile.eof())
		{
			input.append(1, currentChar);
			if (isdigit(dataFile.peek()))
				dataFile.get(currentChar);
			else
				break;
			if (dataFile.eof())
				break;
			loops++;
			if (loops >= 8)
			{
				cout << "ERROR, TOKEN TO LARGE" << endl;
				exit(0);
			}
		}
		TK = createToken(TK_NUM, lineNum);
		TK->value = input;
		return TK;
	}
	else if (isalpha(currentChar) && !dataFile.eof())
	{
		int loops = 0;
		string input;
		while (isalpha(currentChar))
		{
			input.append(1, currentChar);
			if (isalpha(dataFile.peek()))
				dataFile.get(currentChar);
			else
				break;
			if (dataFile.eof())
				break;
			loops++;
			if (loops >= 8)
			{
				cout << "ERROR, TOKEN TO LARGE " << input << endl;
				exit(0);
			}
		}

		TK = createToken(checkKW(input), lineNum);

		TK->value = input;
		return TK;
	}
	else if (ispunct(currentChar))
	{
		switch (currentChar)
		{
		case '=':
			TK = createToken(SB_EQ, lineNum);
			TK->value = currentChar;
			return TK;
		case '>':
			if (dataFile.peek() == '=')
			{
				string input;
				input.append(1, currentChar);
				dataFile.get(currentChar);
				input.append(1, currentChar);
				TK = createToken(SB_GEQ, lineNum);
				TK->value = input;

			}
			else
			{
				TK = createToken(SB_GT, lineNum);
				TK->value = currentChar;
			}
			return TK;
		case '<':
			if (dataFile.peek() == '=')
			{
				string input;
				input.append(1, currentChar);
				dataFile.get(currentChar);
				input.append(1, currentChar);
				TK = createToken(SB_LEQ, lineNum);
				TK->value = input;

			}
			else
			{
				TK = createToken(SB_LT, lineNum);
				TK->value = currentChar;
			}
			return TK;
		case '~':
			TK = createToken(SB_TILD, lineNum);
			TK->value = currentChar;
			return TK;
		case ':':
			TK = createToken(SB_COLON, lineNum);
			TK->value = currentChar;
			return TK;
		case '+':
			TK = createToken(SB_PLUS, lineNum);
			TK->value = currentChar;
			return TK;
		case '-':
			TK = createToken(SB_MINUS, lineNum);
			TK->value = currentChar;
			return TK;
		case '*':
			TK = createToken(SB_AST, lineNum);
			TK->value = currentChar;
			return TK;
		case '/':
			TK = createToken(SB_SLASH, lineNum);
			TK->value = currentChar;
			return TK;
		case '%':
			TK = createToken(SB_MOD, lineNum);
			TK->value = currentChar;
			return TK;
		case '.':
			TK = createToken(SB_PERIOD, lineNum);
			TK->value = currentChar;
			return TK;
		case '(':
			TK = createToken(SB_LPAR, lineNum);
			TK->value = currentChar;
			return TK;
		case ')':
			TK = createToken(SB_RPAR, lineNum);
			TK->value = currentChar;
			return TK;
		case ',':
			TK = createToken(SB_COMMA, lineNum);
			TK->value = currentChar;
			return TK;
		case '{':
			TK = createToken(SB_LCURLY, lineNum);
			TK->value = currentChar;
			return TK;
		case '}':
			TK = createToken(SB_RCURLY, lineNum);
			TK->value = currentChar;
			return TK;
		case ';':
			TK = createToken(SB_SEMICOLON, lineNum);
			TK->value = currentChar;
			return TK;
		case '[':
			TK = createToken(SB_LBRACK, lineNum);
			TK->value = currentChar;
			return TK;
		case ']':
			TK = createToken(SB_RBRACK, lineNum);
			TK->value = currentChar;
			return TK;
		default:
			cout << "ERROR: UNEXPECTED INPUT: " << currentChar << endl;
			exit(0);
		}

	}
}

void skipComment()
{
	char commentChar;
	dataFile.get(commentChar);
	while (commentChar != '#')
		dataFile.get(commentChar);
}

void printToken(Token* token)
{
	cout << "tokenID: " << token->tokenType << " tokenInstance: " << token->value << " on Line: " << token->lineNum << endl;
}


