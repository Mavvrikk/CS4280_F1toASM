#include <iostream>
#include <cstdio>
#include <string>
#include <fstream>
#include <stdlib.h>
#include "Token.h"
#include "Scanner.h"
#include "Parser.h"
#include "SS.h"
#include "output.h"

using namespace std;

static int loopCounter = 0;
ofstream asmFile;
string nLeft, nRight, nCenter, nExtra = "";

void outPutFile() 
{
	
	asmFile.open("outPut.txt");
	if (!asmFile)
	{
		cout << "THERE WAS AN ERROR CREATING THE ASM FILE." << endl;
		exit(1);
	}
	updatedTraversal(root,0);
	asmFile << "STOP\n";
	printVectortoFile(IDS);
}

string updatedTraversal(struct Node* node, int _cond)
{
	if (node == NULL)
		return "";


	if (node->function == "<Program>" || node->function == "<Vars>" || node->function == "<VarList>" ||
		node->function == "<Stats>" || node->function == "<Mstat>" || node->function == "<Stat>" || node->function == "<Block>")
	{ //These are the "non code generating" nodes, keep looking for another node
		nLeft = updatedTraversal(node->left,0);
		nCenter = updatedTraversal(node->center,0);
		nRight = updatedTraversal(node->right,0);
		nExtra = updatedTraversal(node->extra,0);
		return "";
	}
	else if (node->function == "<Exp>")
	{
		int size = node->data.size();
		if (node->data[size - 1] == "") // THERE IS NO OPERATOR
		{
			nLeft = updatedTraversal(node->left,0); //left tree 
			if (_cond == 0)
				asmFile << "LOAD " << nLeft << "\nSTORE t1\n";
			return nLeft;
		}
		else // theres an operator
		{
			nLeft = updatedTraversal(node->left,0); // Result of LEFT Node in ACC
			/*GETTING TWO PRINTS HERE FOR SOME REASON*/
			if(nLeft == "")
				asmFile << "PUSH\nSTACKW 0\n"; //LEFT node now in stack at P0
			else
				asmFile << "PUSH\nLOAD " << nLeft << "\nSTACKW 0\n"; //LEFT node now in stack at P0
			nRight = updatedTraversal(node->right,0); // PUT RIGHT NODE INTO ACC
			if(nRight == "")
				asmFile << "STACKR 0\nPOP\n"; //STORE IT IN TEMP VARIABLE
			else
				asmFile << "LOAD " << nRight << "\nSTORE t1\nSTACKR 0\nPOP\n"; //STORE IT IN TEMP VARIABLE
			if (node->data[size - 1] == "+")
				asmFile << "ADD t1\n";
			if (node->data[size - 1] == "-")
				asmFile << "SUB t1\n";
			asmFile << "STORE t1\n";
			return "";
		}
	}
	else if(node->function == "<M>")
	{
		int size = node->data.size();
		if (node->data[size - 1] == "") // THERE IS NO OPERATOR
		{
			nLeft = updatedTraversal(node->left, 0); // need to go down left tree 
			return nLeft;
		}
		else // theres an operator
		{
			nLeft = updatedTraversal(node->left, 0); // Result of LEFT Node in ACC
			asmFile << "PUSH\nLOAD " << nLeft << "\nSTACKW 0\n"; //LEFT node now in stack at P0
			nRight = updatedTraversal(node->right, 0); // PUT RIGHT NODE INTO ACC
			/*NEED TO CONDITION CHECK THIS*/
			if (nRight == "") // was grabbed by a different node
				asmFile <<"STACKR 0\nPOP\n";
			else
				asmFile << "LOAD " << nRight << "\nSTORE t1\nSTACKR 0\nPOP\n"; //STORE IT IN TEMP VARIABLE
			if (node->data[size - 1] == "*")
				asmFile << "MULT t1\n";
			asmFile << "STORE t1\n";
			return "";
		}
	}
	else if (node->function == "<N>")
	{
		int size = node->data.size();
		if (node->data[size - 1] == "") // THERE IS NO OPERATOR
		{
			nLeft = updatedTraversal(node->left, 0); // need to go down left tree 
			return nLeft;
		}
		else // theres an operator
		{
			if (node->data[size - 1] == "-") // urnary
			{
				nRight = updatedTraversal(node->right, 0); //NEED TO STOP SECOND PRINTING OF LOAD
				if (nRight == "") // was grabbed by a different node
					asmFile << "MULT -1\nSTORE t1\n";
				else
					asmFile << "LOAD " << nRight << "\nMULT -1\nSTORE t1\n"; //LEFT node now in stack at P0
			}
			else 
			{
				nLeft = updatedTraversal(node->left, 0); // Result of LEFT Node in ACC
				asmFile << "PUSH\nLOAD " << nLeft << "\nSTACKW 0\n"; //THIS IS GRABBING 7 M->R->L
				nRight = updatedTraversal(node->right, 0); // PUT RIGHT NODE INTO ACC
				asmFile << "LOAD " << nRight << "\nSTORE t1\nSTACKR 0\nPOP\n"; //STORE IT IN TEMP VARIABLE
				if (node->data[size - 1] == "/")
					asmFile << "DIV t1\n";
				asmFile << "STORE t1\n";
			}
			return "";
		} // FIND THE TREE ERRONOUS LOADS REMOVE THEM 
	}
	else if (node->function == "<RO>")
		return node->data[0];
	else if (node->function == "<R>")
	{
		nRight = updatedTraversal(node->right, 0); //IF <R> has any children, theyd be here
		if (nRight == "") // No Right Child
			return node->data[0];
		else
			return nRight;
		//asmFile << node->data[0];
	}
	else if (node->function == "<In>")
	{
		asmFile << "READ" << " " << node->data[1] << endl;
		return "";
	}
	else if (node->function == "<Out>")
	{
		nRight = updatedTraversal(node->right, 0); //THIS IS <EXP>, grab it first
		asmFile << "WRITE t1\n";
		return "";
	}
	else if (node->function == "<Loop>")
	{
		int _Counter = loopCounter;
		string placeHolder = "";
		loopCounter++;
		asmFile << "IN" << _Counter << ": ";
		nLeft = updatedTraversal(node->left, 1); // <EXP1>
		placeHolder = nLeft;
		nCenter = updatedTraversal(node->center, 0); // <RO>
		nRight = updatedTraversal(node->right, 1); //<EXP2>
		/* <EXP1> SHOULD BE IN STACK, <EXP2> IS ACC AND STORED IN T1 */
		asmFile << "LOAD " << placeHolder << "\nSUB " << nRight << "\n";
		if (nCenter == ">=")
		{
			asmFile << "BRNEG Out" << _Counter << "\n";
		}
		else if (nCenter == ">")
		{
			asmFile << "BRZNEG Out" << _Counter << "\n";
		}
		else if (nCenter == "<=")
		{
			asmFile << "BRPOS Out" << _Counter << "\n";
		}
		else if (nCenter == "<")
		{
			asmFile << "BRZNEG Out" << _Counter << "\n";
		}
		else if (nCenter == "~")
		{
			asmFile << "BRZERO Out" << _Counter << "\n";
		}
		else if (nCenter == "=")
		{
			asmFile << "ADD 1\nBRPOS Out" << _Counter << "\n"; // if they are equal, exp1 - exp2 = 0, + 1
		}
		nExtra = updatedTraversal(node->extra, 0); //<STAT>
		asmFile << "BR IN" << _Counter << "\n";
		asmFile << "Out" << _Counter << ": NOOP\n";
		return "";
	}
	else if (node->function == "<If>")
	{
		int _loopCounter = loopCounter;
		loopCounter++;
		nLeft = updatedTraversal(node->left, 1); // 2 things: 1, nLeft is global and being overwritten, need a local string, 2 have IF and LOOP pass a parameter to fix them
		string placeHolder = nLeft;
		nCenter = updatedTraversal(node->center, 0); // <RO>
		nRight = updatedTraversal(node->right, 1); //<EXP2>
		/* <EXP1> SHOULD BE IN STACK, <EXP2> IS ACC AND STORED IN T1 */
		asmFile << "LOAD " << placeHolder <<"\nSUB " << nRight << "\n";
		if (nCenter == ">=")
		{
			asmFile << "BRNEG Out" << _loopCounter << "\n";
		}
		else if (nCenter == ">")
		{
			asmFile << "BRZNEG Out" << _loopCounter << "\n";
		}
		else if (nCenter == "<=")
		{
			asmFile << "BRPOS Out" << _loopCounter << "\n";
		}
		else if (nCenter == "<")
		{
			asmFile << "BRZNEG Out" << _loopCounter << "\n";
		}
		else if (nCenter == "~")
		{
			asmFile << "BRZERO Out" << _loopCounter << "\n";
		}
		else if (nCenter == "=")
		{
			asmFile << "ADD 1\nBRPOS Out" << _loopCounter << "\n"; // if they are equal, exp1 - exp2 = 0, + 1
		}
		nExtra = updatedTraversal(node->extra, 0); //<STAT>
		asmFile << "Out" << _loopCounter << ": NOOP\n";
		return "";
	} 
	else if (node->function == "<Assign>")
	{
		updatedTraversal(node->right, 0);
		asmFile << "STORE " << node->data[0] << "\n";
		return "";

	}
	else
		errorMess(node->function);

}

void errorMess(string funct)
{
	cout << "ERROR: UNEXPECTED STRING " << funct << "FOUND" << endl;
	exit(1);
}

void printVectortoFile(vector<string> vars)
{
	for (int i = 0; i < vars.size(); i++)
	{
		asmFile << vars[i] << " 0" << endl;
	}
}