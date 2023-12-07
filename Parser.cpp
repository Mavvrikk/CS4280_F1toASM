#include <iostream>
#include <string>
#include <algorithm>
#include <vector>
#include <ctype.h>
#include "Token.h"
#include "Scanner.h"
#include "Parser.h"


/* struct Node {
	string function;
	vector<string> data;
	struct Node* left, * right, * center, * extra;
};*/

Node* root = new Node;

void initializeRoot(Node* root)
{
	root->function = "<Program>";
	root->data.push_back(" ");
	root->left = root->right = root->center = root->extra = NULL;
}

Token* parse(Token* token)
{
	initializeRoot(root);
	token = vars(token, root);
	/* token -> value should be main
 * 	   bypass main */
	token = nextTK();
	token = stats(token, root);
	/* token->value should be end now
 * 	   bypass end */
	token = nextTK(); // NOW SHOULD BE EOF FILE
	if (token->tokenType != TK_EOF)
	{
		cout << "ERROR! EXPECTED EOF TOKEN" << endl;
		errorMessage(token);
	}
	return token;
}

Token* vars(Token* token, Node* node)
{
	if (token->tokenType == KW_LET) { //correct Token, consume check next
		Node* Curr = new Node;
		Curr->left = Curr->right = Curr->center = Curr->extra = NULL;
		Curr->function = "<Vars>";
		Curr->lineNum = token->lineNum;
		Curr->data.push_back(token->value);
		node->left = Curr;
		token = nextTK();
		token = varslist(token, Curr);
	}
	if (token->tokenType >= 3 && token->tokenType <= 15 || token->tokenType == 0) // if the next token holds a keyword, return it to whoever called it
	{

		return token;//empty production OR Varslist returned Main TK
	}
	else
		errorMessage(token);

}
Token* varslist(Token* token, Node* node)
{
	Node* Curr = new Node;
	Curr->left = Curr->right = Curr->center = Curr->extra = NULL;
	if (token->tokenType == TK_IDENT)
	{
		Curr->function = "<VarList>";
		Curr->data.push_back(token->value);
		Curr->lineNum = token->lineNum;
		node->left = Curr;
		token = nextTK();
		if (token->tokenType == SB_EQ) // should be an equals not a number.. yet
		{
			Curr->data.push_back(token->value);
			token = nextTK();
			if (token->tokenType == TK_NUM)
			{
				Curr->data.push_back(token->value);
				token = nextTK(); // They match, Consume
			}
			else
			{
				cout << "ERROR, EXPECTED NUMBER TOKEN" << endl;
				errorMessage(token);
			}
		}
		else
		{
			cout << "ERROR! EXPECTED  =  TOKEN " << endl;
			errorMessage(token);
		}
	}
	else
	{
		cout << "ERROR! EXPECTED IDENTIFIER TOKEN " << endl;
		errorMessage(token);
	}
	if (token->tokenType == SB_PERIOD) // if the next token holds a '.'
	{
		token = nextTK();
		return token;
	}
	else // if its not main, then we need to call another Varslist
		varslist(token, Curr);

}

Token* stats(Token* token, Node* node)
{
	Node* Curr = new Node;
	Curr->left = Curr->right = Curr->center = Curr->extra = NULL;
	Curr->function = "<Stats>";
	Curr->data.push_back(" ");
	node->right = Curr;
	token = Stat(token, Curr);
	token = Mstat(token, Curr);
	return token;
}

Token* Stat(Token* token, Node* node)
{
	Node* Curr = new Node;
	Curr->left = Curr->right = Curr->center = Curr->extra = NULL;
	Curr->function = "<Stat>";
	Curr->data.push_back(" ");
	if (node->function == "<If>" || node->function == "<Loop>") // The Calling function changes <stat> position
		node->extra = Curr;
	else
		node->left = Curr;

	if (token->tokenType == KW_SCAN) // < in >
	{
		token = fIn(token, Curr);
	}
	else if (token->tokenType == KW_PRINT) // < out >
	{
		token = fOut(token, Curr);
	}
	else if (token->tokenType == KW_START) // < block >
	{
		token = fBlock(token, Curr);
	}
	else if (token->tokenType == KW_COND) // < if >
	{
		token = fIf(token, Curr);
	}
	else if (token->tokenType == KW_LOOP) // < loop >
	{
		token = fLoop(token, Curr);
	}
	else if (token->tokenType == TK_IDENT) // < assign >
	{
		token = fAssign(token, Curr);
	}
	else // It wasnt anything < stat > deals with
		errorMessage(token);
	return token;
}

Token* fIn(Token* token, Node* node)
{
	/* token->value = scan */
	Node* Curr = new Node;
	Curr->left = Curr->right = Curr->center = Curr->extra = NULL;
	Curr->function = "<In>";
	Curr->lineNum = token->lineNum;
	Curr->data.push_back(token->value);
	node->left = Curr;
	token = nextTK(); // now tk value should be ID
	if (token->tokenType == TK_IDENT)
	{
		Curr->data.push_back(token->value);
		token = nextTK();
		if (token->tokenType == SB_PERIOD)
		{
			token = nextTK(); // grab the next one
			return token; // return it for Mstats
		}
		else
			errorMessage(token);
	}
	else
		errorMessage(token);
}

Token* fOut(Token* token, Node* node)
{
	/* token->value = print */
	Node* Curr = new Node;
	Curr->left = Curr->right = Curr->center = Curr->extra = NULL;
	Curr->function = "<Out>";
	Curr->lineNum = token->lineNum;
	Curr->data.push_back(token->value);
	node->left = Curr;
	token = nextTK(); // grab < exp > 
	token = fExp(token, Curr);
	if (token->tokenType == SB_PERIOD) // after exp should be a .
	{
		token = nextTK(); // grab the next one
		return token; // return it for Mstats
	}
	else
		errorMessage(token);
}

Token* fExp(Token* token, Node* node)
{
	Node* Curr = new Node;
	Curr->left = Curr->right = Curr->center = Curr->extra = NULL;
	Curr->function = "<Exp>";
	Curr->data.push_back(" "); //CHECK THESE WHEN PRINTING TREE

	if (node->function == "<If>" || node->function == "<Loop>") {
		if (node->center == NULL) // IF WE HAVE A LOOP BUT HAVENT DONE THE <RO>
			node->left = Curr;
		else //IF WE'VE DONE <RO>
			node->right = Curr;
	}
	else
		node->right = Curr;

	token = M(token, Curr);
	if (token->tokenType == SB_PLUS) // < M > + < exp >
	{
		Curr->lineNum = token->lineNum;
		Curr->data.push_back(token->value); // push + into vector
		token = nextTK();
		token = fExp(token, Curr);
	}
	else if (token->tokenType == SB_MINUS) // < M > - < exp >
	{
		Curr->lineNum = token->lineNum;
		Curr->data.push_back(token->value);
		token = nextTK();
		token = fExp(token, Curr); // "popped" < m > - , evaluate < exp >
	}
	else // Must be < M > production
	{
		Curr->data.push_back("");
		return token;
	}
}

Token* M(Token* token, Node* node)
{
	Node* Curr = new Node;
	Curr->left = Curr->right = Curr->center = Curr->extra = NULL;
	Curr->function = "<M>";
	Curr->data.push_back("");
	if (node->function == "<M>")
		node->right = Curr;
	else
		node->left = Curr;
	token = N(token, Curr);
	if (token->tokenType == SB_AST) // <N> * <M>
	{
		Curr->lineNum = token->lineNum;
		Curr->data.push_back(token->value);
		token = nextTK(); // grab <M>
		token = M(token, Curr); // Process it
	}
	else // <N> Production
	{
		return token;
	}
}

Token* N(Token* token, Node* node)
{
	Node* Curr = new Node;
	Curr->left = Curr->right = Curr->center = Curr->extra = NULL;
	Curr->function = "<N>";
	Curr->data.push_back("");
	if(node->function == "<N>")
		node->right = Curr;
	else
		node->left = Curr;
	if (token->tokenType == SB_MINUS) // - <N>
	{
		Curr->lineNum = token->lineNum;
		Curr->data.push_back(token->value);
		token = nextTK();
		token = N(token, Curr);
	}
	token = R(token, Curr);
	if (token->tokenType == SB_SLASH) // <R> / <N>
	{
		Curr->lineNum = token->lineNum;
		Curr->data.push_back(token->value);
		token = nextTK();
		token = N(token, Curr);
	}
	else // <R>
	{
		return token;
	}
}

Token* R(Token* token, Node* node)
{
	if (token->tokenType == SB_PERIOD)
		return token;
	Node* Curr = new Node;
	Curr->left = Curr->right = Curr->center = Curr->extra = NULL;
	Curr->function = "<R>";		
	node->left = Curr;
	if (token->tokenType == SB_LBRACK) // [ <exp> ] 
	{
		Curr->data.push_back("");
		Curr->lineNum = token->lineNum;
		token = nextTK(); // grabs <exp>
		token = fExp(token, Curr);
		if (token->tokenType == SB_RBRACK)
		{
			token = nextTK();
			return token;
		}
		else
			errorMessage(token);
	}
	else if (token->tokenType == TK_IDENT) // id
	{
		/* Current token is Identifier */
		Curr->lineNum = token->lineNum;
		Curr->data.push_back(token->value);
		token = nextTK(); // grab whats after ID
		return token; // send it back
	}
	else if (token->tokenType == TK_NUM) // int
	{
		Curr->lineNum = token->lineNum;
		Curr->data.push_back(token->value);
		token = nextTK(); // grab whats after int
		return token; // send it back
	}
	else if (token->tokenType == SB_RBRACK) // end of production for [ <exp> ], just return out
	{
		return token;
	}
	else
		errorMessage(token);
}

Token* fBlock(Token* token, Node* node)
{
	/* start <vars> <stats> stop
 * 	   <vars> is always left, <stats> is always right
 * 	   	   token is currently Start */
	Node* Curr = new Node;
	Curr->left = Curr->right = Curr->center = Curr->extra = NULL;
	Curr->function = "<Block>";
	Curr->data.push_back(" ");
	node->left = Curr;
	token = nextTK(); // grab <vars>
	token = vars(token, Curr); // process <vars> ( Which returns <stats> )
	token = stats(token, Curr); // process <stats> ( which returns stop )
	if (token->tokenType == KW_STOP)
	{
		token = nextTK();
		return token;
	}
	else
		errorMessage(token);
}

Token* fIf(Token* token, Node* node)
{
	/* cond(<exp> <RO> <exp>) <stat>
 * 	   token is current cond */
	Node* Curr = new Node;
	Curr->left = Curr->right = Curr->center = Curr->extra = NULL;
	Curr->function = "<If>";
	Curr->lineNum = token->lineNum;
	Curr->data.push_back(token->value);
	node->left = Curr;
	token = nextTK();
	if (token->tokenType == SB_LPAR) // token == (
	{
		token = nextTK();
		token = fExp(token, Curr);
		token = RO(token, Curr);
		token = fExp(token, Curr);
		if (token->tokenType == SB_RPAR)
		{
			token = nextTK();
			token = Stat(token, Curr);
			return token;
		}
		else
			errorMessage(token);
	}
	else
		errorMessage(token);
}

Token* fLoop(Token* token, Node* node)
{
	/* loop(<exp> <RO> <exp>) <stat>
 * 	   currently token is loop */
	Node* Curr = new Node;
	Curr->left = Curr->right = Curr->center = Curr->extra = NULL;
	Curr->function = "<Loop>";
	Curr->lineNum = token->lineNum;
	Curr->data.push_back(token->value);
	node->left = Curr;
	token = nextTK();
	if (token->tokenType == SB_LPAR) // token == (
	{
		token = nextTK();
		token = fExp(token, Curr);
		token = RO(token, Curr);
		token = fExp(token, Curr);
		if (token->tokenType == SB_RPAR) // token == )
		{
			token = nextTK();
			token = Stat(token, Curr);
			return token;
		}
		else
			errorMessage(token);
	}
	else
		errorMessage(token);
}

Token* RO(Token* token, Node* node)
{
	Node* Curr = new Node;
	Curr->left = Curr->right = Curr->center = Curr->extra = NULL;
	Curr->function = "<RO>";
	Curr->lineNum = token->lineNum;
	Curr->data.push_back(token->value);
	node->center = Curr;

	if (token->tokenType == SB_LEQ)
	{
		token = nextTK();
		return token;
	}
	else if (token->tokenType == SB_GEQ)
	{
		token = nextTK();
		return token;
	}
	else if (token->tokenType == SB_LT)
	{
		token = nextTK();
		return token;
	}
	else if (token->tokenType == SB_GT)
	{
		token = nextTK();
		return token;
	}
	else if (token->tokenType == SB_EQ)
	{
		token = nextTK();
		return token;
	}
	else if (token->tokenType == SB_TILD)
	{
		token = nextTK();
		return token;
	}
	else
		errorMessage(token);
}

Token* fAssign(Token* token, Node* node)
{
	/* id ~<exp> .
 * 	  token is currently id */
	Node* Curr = new Node;
	Curr->left = Curr->right = Curr->center = Curr->extra = NULL;
	Curr->function = "<Assign>";
	Curr->lineNum = token->lineNum;
	Curr->data.push_back(token->value);
	node->left = Curr;
	token = nextTK(); // grab ~
	Curr->data.push_back(token->value);
	if (token->tokenType == SB_TILD)
	{
		token = nextTK(); // grab <exp>
		token = fExp(token, Curr);
		if (token->tokenType == SB_PERIOD)
		{
			token = nextTK();
			return token;
		}
		else
			errorMessage(token);
	}
	else
		errorMessage(token);
}

Token* Mstat(Token* token, Node* node)
{
	Node* Curr = new Node;
	Curr->left = Curr->right = Curr->center = Curr->extra = NULL;
	Curr->function = "<Mstat>";
	Curr->data.push_back(" ");
	node->right = Curr;
	if (token->tokenType == KW_END || token->tokenType == KW_STOP) // only 2 ways this can be an empty production
		return token;
	else
		token = Stat(token, Curr);
	token = Mstat(token, Curr);
}

void errorMessage(Token* token)
{
	cout << "ERROR! UNEXPECTED TOKEN " << token->value << " FOUND AT " << token->lineNum << endl;
	exit(1);
}

void printPreorder(struct Node* node)
{
	if (node == NULL)
		return;


	cout << node->function << " ";
	for (int i = 0; i < node->data.size(); i++)
	{
		cout << node->data[i] << " ";
	}
	cout << endl;


	printPreorder(node->left);
	printPreorder(node->center);
	printPreorder(node->right);
	printPreorder(node->extra);

}

