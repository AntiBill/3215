//Real-Time Coursework

#include <fstream>
#include <iostream>
#include <string> 
using namespace std;

int main() {
    int counter = 0;
    int numTasks = 0;
    char task[5];
    string t;

    ifstream inFile;

    inFile.open("input.txt");
    
    if (!inFile) {
        cerr << "Unable to open file datafile.txt";
        exit(1);   // call system to stop with an error
    }

    //Read the first 2 strings of the file. Ignore the string 'NumTasks' and convert the number of tasks to an integer
    while (inFile >> t && counter < 2) {
        if(counter == 1) {
            numTasks = stoi(t);
        }
        counter++;
    }

    //At this point, numTasks = the number of tasks
    for(int i = 0; i < numTasks; i++) {

    }

    
    // prints the contents of the file
    cout << numTasks << endl;
    cin >> t;
    return 0;
}