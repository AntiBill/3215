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
    while (counter < 2 && inFile >> t) {
        if(counter == 1) {
            numTasks = stoi(t);
        }
        counter++;
    }

    //Define 3 arrays on the heap, all big enough to hold the task data of every task
    string* taskNames = new string[numTasks];
    int* runtime = new int[numTasks];
    int* period = new int[numTasks];

    //Read in every task and add its data to the appropriate array
    for(int i = 0; i < numTasks; i++) {
        cin >> t;
        counter = 0;
        while (counter < 3 && inFile >> t) {
            if(counter == 0) {
                taskNames[i] = t;
            } else if (counter == 1) {
                runtime[i] = stoi(t);
            } else {
                period[i] = stoi(t);
            }
            counter++;
        }
    }

    //Perform RMS scheduling on the input tasks
    
    // prints the contents of the file
    cout << numTasks << endl;
    cin >> t;
    return 0;
}