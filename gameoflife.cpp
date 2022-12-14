#include <iostream>
#include <fstream>
#include <sstream>
#include <bitset>

#include "supercell.h"
#include "boardmanager.h"

using namespace std;

#define PRINTED_GEN 1

int main(int argc, char *argv[])
{
    if (argc < 2)
    {
        cout << "No arguments; expected input file\n";
        return 1;
    }

    // Open and parse test file
    ifstream inputFile;
    inputFile.open(argv[1]);

    string line;
    int64_t x, y;
    BoardManager bm;
    while (inputFile >> line)
    { 
        stringstream ss(line); 
        string s;

        getline(ss, s, ',');
        x = strtoll(s.c_str(), NULL, 10);

        getline(ss, s, '\n');
        y = strtoll(s.c_str(), NULL, 10);

        bm.setCell(x, y);
    }
    
    // Generation count
    int c = 0;
    string s;
    while(true) 
    {    
        if (c++ % PRINTED_GEN == 0) {    
            cout << endl << endl << "******************************************************************" << endl
                << "GEN " << c << endl
                << "******************************************************************" << endl;
            
            bm.printBoard();
            cout << endl << endl << "******************************************************************" << endl
                << "GEN " << c << endl
                << "******************************************************************" << endl;
            
            getchar();
        }

        bm.simulateStep();
    }
}
