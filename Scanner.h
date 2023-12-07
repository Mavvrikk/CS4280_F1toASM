#pragma once


extern char currentChar;
extern Token* token;
extern int lineNum;
extern ifstream dataFile;

Token* nextTK();
Token* testInput(char currnet);
void skipComment();
void printToken(Token* token);

