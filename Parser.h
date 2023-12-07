#pragma once

#include<vector>

struct Node
{
	string function;
	int lineNum = 0;
	vector<string> data;
	struct Node* left, * right, * center, * extra;
};

extern Node* root;

Token* parse(Token* token);
Token* vars(Token* token, Node* node);
Token* varslist(Token* token, Node* node);
Token* fExp(Token* token, Node* node);
Token* M(Token* token, Node* node);
Token* N(Token* token, Node* node);
Token* R(Token* token, Node* node);
Token* stats(Token* token, Node* node);
Token* Mstat(Token* token, Node* node);
Token* Stat(Token* token, Node* node);
Token* fBlock(Token* token, Node* node);
Token* fIn(Token* token, Node* node);
Token* fOut(Token* token, Node* node);
Token* fIf(Token* token, Node* node);
Token* fLoop(Token* token, Node* node);
Token* fAssign(Token* token, Node* node);
Token* RO(Token* token, Node* node);
void initializeRoot(Node* root);
void errorMessage(Token* token);
void printPreorder(Node* node);
