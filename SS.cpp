#include <iostream>
#include <cstdio>
#include <string>
#include <vector>
#include <stdlib.h>
#include "Token.h"
#include "Scanner.h"
#include "Parser.h"
#include "SS.h"

using namespace std;

vector <string> IDS;

string inputString;

void PreorderVars(struct Node* node)
{
	if (node == NULL)
		return;

	if(node->function == "<VarList>") // we are in a node that can contain Identifiers
	{
		inputString = node->data[0]; //  data[0] is id, data[1] is '=' and data [3] is numTK
		insert(inputString);
	}

	PreorderVars(node->left);
	PreorderVars(node->center);
	PreorderVars(node->right);
	PreorderVars(node->extra);

}

void insert(string id)
{
	for (int i = 0; i < IDS.size(); i++) // make sure its not a repeat definition
	{
		if (id == IDS[i])
		{
			cout << "ERROR: IDENTIFIER '" << id << "' PREVIOUSLY DEFINED" << endl;
			exit(0);
		}
	}
	IDS.push_back(id);
}

void printVector(vector<string> vars)
{
	for (int i = 0; i < vars.size(); i++)
	{
		cout << vars[i] << " 0" << endl;
	}
}

void VerifyVars(struct Node* node)
{
	bool isValid = false;
	if (node == NULL)
		return;

	if (node->function == "<R>" || node->function == "<Assign>") // the only other nodes that can hold ID
	{
		inputString = node->data[0]; //  data[0] is id, [, or numTK inside <R> and data[0] is always id inside <Assign>
		if (testString(inputString)) // if its not a numTK or [
		{
			for (int i = 0; i < IDS.size(); i++) // make sure its not a repeat definition
			{
				if (inputString == IDS[i])
				{
					isValid = true;
				}
			}
			if (isValid);
			else
			{
				cout << "ERROR: IDENTIFIER " << inputString << " NOT DEFINED" << endl;
			}
		}
	}

	VerifyVars(node->left);
	VerifyVars(node->center);
	VerifyVars(node->right);
	VerifyVars(node->extra);
}

bool testString(string Input)
{
	for (int i = 0; i < Input.size(); i++)
	{
		if (!isalpha(Input[i]))
			return false;
	}
	return true;
}