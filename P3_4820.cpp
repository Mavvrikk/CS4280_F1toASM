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
string fileName;

int main(int  argc, char** argv)
{

    if (argc == 1) //NO FILE WAS GIVEN AS ARGUMENT
    {
        string tester;
        ofstream outFile;
        string fileName = "scanMe.txt";
        outFile.open("scanMe.txt");
        if (!outFile)
        {
            cout << "THERE WAS AN ERROR OPENING THE FILE." << endl;
            exit(1);
        }
        while (cin >> tester)
        {
            outFile << tester;
            outFile << endl;
        }
        outFile.close();
        dataFile.open("scanMe.txt");
        if (!dataFile)
        {
            cout << "THERE WAS AN ERROR OPENING THE FILE." << endl;
            exit(1);
        }
    }
    else if (argc == 2) // FILE WAS GIVEN
    {
        dataFile.open(argv[1]);
        if (!dataFile)
        {
            cout << "THERE WAS AN ERROR OPENING THE FILE." << endl;
            exit(1);
        }
    }

    token = nextTK();
    while (token->tokenType != TK_EOF)
    {
        token = parse(token);
    }

    //printPreorder(root);
    insert("t1");
    PreorderVars(root);
    VerifyVars(root);
    outPutFile();
    if (fileName == "scanMe.txt")
        remove("scanMe.txt");
}


